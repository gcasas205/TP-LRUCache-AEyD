# Cómo funciona el TP de punta a punta

Este documento sigue una ejecución real del programa, desde que se generan los archivos hasta que sale el
CSV del informe, con los nombres de las funciones tal como están en el código. Sirve para estudiar el TP
antes de la defensa y para saber dónde tocar cuando haya que agregar algo. Los contratos exactos de cada
función están en [`primitivas.md`](primitivas.md) y las decisiones de diseño, en
[`arquitectura.md`](arquitectura.md).

## 1. El recorrido en una imagen

```
 generador 2026 80000 300                         simulador [directo | seek | ambos]
 (apps/main_generador.c)                          (apps/main_simulador.c)
        |                                                 |
        |  generarPosteos ----> datos/posteos.dat ------->|  por cada modo x lote x variante x capacidad:
        |  generarLote x 3 ---> datos/lote_*.dat -------->|    crearCacheX  ->  simular  ->  reporte  ->  vaciarCache
        |                                                 |
                                                          v
                                        simular (sim/simulador.c)
                                          por cada usuario, por cada id del feed:
                                            obtenerPosteo
                                              obtenerCache ----> acierto: listo
                                              (fallo) lector ---> lee del archivo
                                                      agregarCache
                                                          |
                                                          v
                                        tCache (cache/cache.c): despacha a la variante
                                          sin caché | Parte 1 (lru_simple) | Parte 2 (lru_doble)
                                                          |
                                                          v
                                        TDA base (tda/): lista, lista doble, árbol
```

Las capas solo se llaman hacia abajo: `apps` → `sim`/`dominio` → `cache` → `tda`. El simulador nunca
sabe qué variante de caché está usando.

## 2. Los datos

Todo se guarda en archivos binarios de registros de tamaño fijo.

| Archivo | Registro | Tamaño | Lo escribe | Lo lee |
| --- | --- | --- | --- | --- |
| `datos/posteos.dat` | `tPosteo {int id; char texto[141]}` (148 bytes con relleno) | 50.000 registros, 7,4 MB | `generarPosteos` | el lector del simulador |
| `datos/lote_concentrado.dat`, `lote_disperso.dat`, `lote_ordenado.dat` | `tUsuario {char usuario[15]; int feed[20]}` (96 bytes) | 80.000 registros, 7,7 MB cada uno | `generarLote` | `simular`, con `leerUsuario` |
| `informe/resultados.csv` | una línea por corrida, 11 campos con `;` | 54 líneas + encabezado | `exportarCsv` | el informe |

El id de un posteo es su número de registro. Por eso se lo encuentra sin buscar: el registro `id` empieza
en el byte `id × sizeof(tPosteo)`.

## 3. Generar los archivos: `main_generador`

`generador <semilla> <usuarios> <G>`, por ejemplo `generador 2026 80000 300`.

1. **Valida los argumentos antes de crear nada.** `leerEntero` usa `strtol` y revisa `errno` (`ERANGE`),
   porque `strtol` satura sin avisar. Usuarios entre `MIN_USUARIOS` (200) y `MAX_USUARIOS`
   (1.000.000); G entre 1 y 50.000. Si algo falla, muestra el uso y sale con 1 sin tocar `datos/`.
2. **`srand(semilla)` una sola vez.** Después genera siempre en el mismo orden (posteos, concentrado,
   disperso, ordenado). Con la misma semilla en la misma máquina salen los mismos archivos byte a byte.
3. **`generarPosteos(ruta, 50000)`:** escribe el registro `i` con `id = i` y el texto
   `"Posteo numero i"`. Antes hace `memset` del registro, así no quedan bytes basura en el archivo.
4. **`generarLote(ruta, usuarios, estrategia, contexto)`, una vez por lote.** Arma cada usuario con
   nombre `usuario0001`, `usuario0002`, … y le pide el feed a la **estrategia**, que llega como
   puntero a función (`tEstrategiaFeed`). `generarLote` no sabe qué lote está generando.

Las tres estrategias:

| Estrategia | Contexto | Qué hace | Para qué |
| --- | --- | --- | --- |
| `feedConcentrado` | `int*` con G | 20 ids al azar entre 0 y G − 1 | muchos repetidos: la caché debería ganar |
| `feedDisperso` | no usa | 20 ids al azar entre 0 y 49.999 | casi sin repetidos: la caché solo agrega costo |
| `feedOrdenado` | `int*` con la cantidad de usuarios | el pedido número j recibe ⌊j × 50000 / pedidos⌋ | ids crecientes: degenera el árbol índice |

Dos detalles que se defienden:

- **`aleatorioEntre(min, max)`** combina dos `rand()` (`rand() * (RAND_MAX + 1L) + rand()`) porque en
  MinGW `RAND_MAX` vale 32.767, menos que 49.999: con un solo `rand()` nunca saldrían ids altos.
- **`feedOrdenado` calcula en `double`.** Con 80.000 usuarios, j llega a 1.599.999, y j × 50.000 no
  entra en un `long` de 32 bits (MinGW), y C89 no tiene `long long`. Con 80.000 usuarios cada id se
  repite 32 veces seguidas.

## 4. Simular: `main_simulador`

`simulador [directo | seek | ambos]` (sin argumento, `ambos`).

1. Valida el argumento y que existan `posteos.dat` y los tres lotes. Si falta algo, avisa cuál y sale
   con 1.
2. Abre `informe/resultados.csv`, escribe el encabezado (`exportarEncabezadoCsv`) y el de la tabla
   (`imprimirEncabezado`).
3. Recorre todas las combinaciones con cuatro arrays:
   - `modos[]`: `{"directo", "Directo", leerPosteoPorId}` y `{"seek", "Con seek", leerPosteoConSeek}`.
   - `lotes[]`: concentrado, disperso y ordenado (nombre y ruta).
   - `variantes[]`: `crearCacheSimple` y `crearCacheDoble`, como punteros a función.
   - `capacidades[]`: 10, 100, 1000 y 5000.
4. Por cada modo y cada lote: una corrida **sin caché** (`crearSinCache`, capacidad 0) y una por cada
   variante y capacidad, o sea 1 + 2 × 4 = 9 corridas. Con los dos modos y los tres lotes son 54.
5. Cada corrida pasa por `correr`: `simular` → `alturaIndiceCache` → `imprimirFila` → `exportarCsv` →
   `vaciarCache`. La altura se consulta **antes** de vaciar.
6. Si falla crear una caché o una corrida, lo informa por `stderr` con modo, lote, caché, capacidad y
   código, y sigue. Al final, si hubo fallas, sale con 1 y avisa que el CSV quedó incompleto.

## 5. Una corrida: `simular`

```c
int simular(const char *rutaPosteos, const char *rutaLote, tCache *pCache, tLectorPosteo lector, tMetricas *pMetricas)
```

1. `iniciarMetricas` (todo en cero).
2. Abre los dos archivos en `"rb"`. Si el lote no abre, cierra el de posteos antes de devolver
   `ERROR_ARCHIVO`.
3. **`calentarArchivo`:** lee `posteos.dat` entero de forma secuencial y vuelve al principio (`rewind`).
   Así el archivo queda en la memoria del sistema operativo y ninguna corrida paga el disco real por
   haber corrido primero. No cuenta en las métricas ni en el tiempo.
4. `inicio = clock()`.
5. **El bucle medido:** `while(leerUsuario(fpLote, &usuario) == EXITO)`, y para cada uno de los 20 ids
   del feed, `obtenerPosteo(...)`; al terminar el feed, `registrarUsuario`. Entre los dos `clock()` no
   hay ningún `printf` (RF-09).
6. `fijarTiempo(pMetricas, inicio, clock())` y cierra los dos archivos.

El simulador recibe la caché **ya creada** y el modo de acceso como **puntero a función** (`lector`): no
tiene ningún `if` sobre la variante ni sobre el modo. Por eso un lote, un modo o una variante nueva no lo
cambian.

## 6. Un pedido: `obtenerPosteo`

La clave de la caché es el id (`int`, se compara con `cmpIdPosteo`) y el valor, el `tPosteo` completo.

```
obtenerCache(id)  ── EXITO ──>  registrarAcierto
       │
   NO_ENCONTRADO
       │
       v
registrarFallo
lector(fpPosteos, id, &posteo)   <- leerPosteoPorId o leerPosteoConSeek
registrarLectura
si el lector dio EXITO: agregarCache(id, posteo)
       │
       v
registrarPosteo (siempre)
```

- **`leerPosteoPorId`:** `fseek` al byte `id × sizeof(tPosteo)` y un `fread`. Si `id < 0` o el `fread`
  no lee un registro (id más allá del final), devuelve `NO_ENCONTRADO`.
- **`leerPosteoConSeek`:** primero `fseek(fp, 0L, SEEK_END)` y después lo mismo. Es un **costo de acceso
  simulado** (lo recomendó la cátedra): la lectura cuesta más, pero lee lo mismo.
- **Id fuera de rango (RF-19):** cuenta como fallo y como lectura, no entra en la caché y la corrida
  sigue.

Invariantes que se cumplen en las 54 corridas: aciertos + fallos = posteos, lecturas = fallos, y sin
caché aciertos = 0.

## 7. La interfaz común: `tCache`

```c
typedef struct
{
    void *impl;            // tLruS*, tLruD* o NULL (sin caché)
    tAgregarImpl agregar;  // punteros a los adaptadores de la variante
    tObtenerImpl obtener;
    tBorrarImpl borrar;
    tVaciarImpl vaciar;
    tAlturaImpl altura;
    const char *nombre;    // "Sin cache", "Lista simple", "Lista doble + ABB"
} tCache;
```

Cada función pública es una sola línea que llama al puntero. Por ejemplo, un `agregarCache` con la
Parte 1 recorre esta cadena:

```
agregarCache(&cache, &id, ...)
  └─> cache.agregar(cache.impl, &id, ...)        el campo guarda agregarAdapS
        └─> agregarAdapS(impl, &id, ...)          adaptador static de cache.c
              └─> agregarLruS((tLruS*)impl, &id, ...)   la Parte 1 de verdad
```

- **Los adaptadores existen** porque `agregarLruS` recibe `tLruS*` y `agregarLruD` recibe `tLruD*`: para
  guardarlas en el mismo campo, las dos necesitan la misma firma (`void *impl`). El adaptador castea y
  llama.
- **Crear:** `crearCacheSimple` pide **una** `tLruS` con `malloc`, la inicializa con `crearLruS` (que
  valida capacidad > 0 y `cmp` no nulo), la engancha en `impl` y carga los 5 adaptadores y el nombre. Si
  `crearLruS` falla, libera la `tLruS` y devuelve su mismo código. `crearCacheDoble` es igual con
  `tLruD`.
- **Sin caché:** `impl = NULL` y adaptadores que no hacen nada: agregar devuelve `EXITO`, obtener y
  borrar `NO_ENCONTRADO`, altura -1. Así el simulador mide la línea base con el mismo código.
- **Vaciar:** `pCache->vaciar(impl)` (vacía la lista y el árbol), `free(impl)` (libera la `tLruS` o
  `tLruD`) y `crearSinCache(pCache)`, que deja la caché segura ante un segundo `vaciarCache`. Por eso,
  después de vaciar, el nombre pasa a ser "Sin cache".
- **Altura:** el adaptador `altura` devuelve la del árbol en la Parte 2 y -1 en las otras dos, sin
  preguntar qué variante es.

## 8. Cada elemento guardado: el bloque `[clave][valor]`

Las dos partes guardan cada elemento como **un solo bloque** con la clave primero:

```
[ id (4 bytes) ][ tPosteo (148 bytes) ]     tamInfo = tamClave + tamValor
```

- `crearEntrada` arma el bloque con dos `memcpy`; `copiarValorEntrada` copia de vuelta solo el valor,
  desde `(char*)bloque + tamClave`.
- **La clave va primero** para que el `cmp` del usuario, que compara claves, sirva directo para buscar
  en la lista y en el árbol por dato parcial.
- **Nunca se castea** `(char*)bloque + tamClave` a `tPosteo*`: se copia con `memcpy` (alineación).

## 9. Parte 1 por dentro: `lru_simple` sobre la lista simple

`tLruS {tLista lista; unsigned capacidad, cantidad; tCmp cmp}`. La lista está ordenada por uso: el
primero es el más reciente y el último, el próximo a descartar.

**`obtenerLruS(id)`**
1. `llevarAlPrinLista`: recorre con puntero a puntero comparando la clave; si la encuentra,
   **reengancha ese nodo al principio** sin pedir memoria. Si no, `NO_ENCONTRADO`.
2. Pide un temporal de `tamClave + tamValor`, lo precarga con el buffer del llamador, copia ahí el
   primero (`verPrimeroLista`) y devuelve el valor con `copiarValorEntrada`. La precarga hace que, si el
   valor guardado es más corto, el resto del buffer del llamador quede como estaba.

**`agregarLruS(id, valor)`**
1. `crearEntrada` arma el bloque.
2. `sacarPorClaveLista`: si la clave ya estaba, saca ese nodo (es un reemplazo).
3. `insPrinLista` inserta el bloque nuevo al principio (antes consulta `listaLlena`, que prueba dos
   `malloc` y los libera).
4. Si la clave era nueva: `cantidad++`, y si pasó la capacidad, `sacarUltLista` (recorre hasta el
   último) y `cantidad--`.
5. Libera el bloque temporal: la lista guardó su propia copia.

**Costo:** cada fallo recorre la lista entera dos veces (una en `obtener`, que no encuentra, y otra en
`sacarPorClaveLista` al agregar), más el recorrido de `sacarUltLista`. Con capacidad grande es lo que la
hunde: en el lote disperso con capacidad 5000, 126 s contra 6,6 s sin caché.

## 10. Parte 2 por dentro: lista doble + árbol índice

```
árbol índice (ABB por clave)                 lista doble (orden de uso)
nodo: [clave][posición] ─────────────────>   pri <-> nodo <-> nodo <-> ... <-> ult
                                              cada nodo: [clave][valor]
```

`tLruD {tListaD lista; tArbol indice; unsigned capacidad, cantidad; tCmp cmp}`. La lista doble tiene
`pri` y `ult`, así que mover un nodo al principio y sacar el último no recorren nada. El árbol solo sirve
para **encontrar** el nodo de la lista sin recorrerla: cada nodo del árbol guarda `[clave][posición]`, y
la posición (`tPosListaD`, un `void*`) es la dirección del nodo de la lista.

**`obtenerLruD(id)`**
1. `buscarArbol` con dato parcial (la clave) a un buffer de `tamClave + sizeof(tPosListaD)`. Si no está,
   `NO_ENCONTRADO`.
2. Saca la posición con `memcpy` desde `buffer + tamClave`.
3. Pide el temporal, `moverAlPrinListaD(lista, pos)` (reengancha `ant` y `sig`, actualiza `pri` y
   `ult`), `verInfoListaD(pos, …)` y `copiarValorEntrada`.

**`agregarLruD(id, valor)`**: tres casos.
- **La clave ya estaba:** `buscarArbol` da la posición, `actualizarInfoListaD` cambia el bloque del nodo
  (pide el nuevo antes de liberar el viejo) y `moverAlPrinListaD`. El árbol no cambia: la clave y la
  posición son las mismas.
- **Clave nueva con la caché llena:** primero se descarta el último. `verUltListaD` copia su clave,
  `eliminarArbol` la saca del índice y `sacarUltListaD` la saca de la lista; `cantidad--`.
- **Después, en los dos casos de clave nueva:** `insPrinListaD` inserta y devuelve la posición del nodo
  nuevo; `crearEntrada` arma `[clave][posición]` y `insArbol` lo agrega al índice; `cantidad++`. Si
  `insArbol` falla, se saca de la lista el nodo recién insertado para que las dos estructuras sigan
  iguales.

**`borrarLruD(id)`:** `buscarArbol` (para la posición), `eliminarArbol`, `sacarNodoListaD`, `cantidad--`.

**`vaciarLruD`:** `vaciarArbol` y `vaciarListaD`.

**`verificarLruD`** (para las pruebas): comprueba que el árbol y la lista tengan la misma cantidad de
nodos, igual a `cantidad` y no mayor que `capacidad`.

**El árbol (`tda/arbol.c`):**
- `insArbol` y `buscarArbol` son **iterativos** y comparten `buscarNodoArbol`, que baja con puntero a
  puntero y devuelve la dirección del puntero al nodo (o la del `NULL` donde iría). Se llaman millones de
  veces, y con el árbol degenerado la recursión apilaría miles de llamadas.
- `eliminarArbol`: con 0 o 1 hijo, el hijo (o `NULL`) ocupa su lugar; con dos hijos, intercambia
  `info`/`tamInfo` con el **menor del subárbol derecho** (`menorNodoArbol`) y elimina ese, que tiene a
  lo sumo un hijo.
- `alturaArbol`, `cantNodosArbol`, `recorrerInOrdenArbol` y `vaciarArbol` (pos-orden) son recursivas.

**Por qué degenera en el lote ordenado:** cada id nuevo es mayor que todos los del árbol y se engancha a
la derecha del anterior, así que la altura termina igual a la capacidad. Además, el posteo que se repite
32 veces es justo el último agregado, el del fondo de la cadena: cada acierto recorre todos los niveles.
Con capacidad 5000, la Parte 2 tarda 41,4 s contra 1,13 s sin caché. La Parte 1, en cambio, lo encuentra
en el primer nodo de la lista.

## 11. Ejemplo a mano: el lote mini de las pruebas

Tres usuarios: A pide los ids 0 a 19, B los mismos 0 a 19 y C del 20 al 39. Son 60 pedidos sobre un
archivo de 50 posteos.

| Corrida | Qué pasa | Aciertos / fallos / lecturas |
| --- | --- | --- |
| Sin caché | Los 60 pedidos van al archivo | 0 / 60 / 60 |
| Parte 1 o 2, capacidad 20 | A trae los 20 a la caché; B los encuentra todos; C trae 20 nuevos y descarta los de A | 20 / 40 / 40 |
| Parte 1 o 2, capacidad 10 | Cuando B pide el 0, ya se descartó (entraron del 10 al 19 después): LRU descarta justo lo que se va a pedir | 0 / 60 / 60 |
| Parte 2, capacidad 20 | Altura del índice al final: 20 (C llega en orden creciente) | — |

Son los casos CP-004-13 a 17 de `specs/004-simulacion/casos-prueba.md`.

## 12. Medir y reportar

- **`tMetricas`:** `aciertos`, `fallos`, `lecturas`, `posteos`, `usuarios` (`long`) y `segundos`
  (`double`). Cada evento suma con su `registrarXxx`.
- **`fijarTiempo`:** `(fin - inicio) / CLOCKS_PER_SEC`. En Windows `clock()` mide de a 1 ms, por eso
  las corridas tienen que ser largas: con 80.000 usuarios, la más corta sin caché dura 1,13 s.
- **Promedios (`promedioPorPosteo`, `promedioPorUsuario`):** se calculan dividiendo el total; una sola
  lectura dura menos que el reloj. Devuelven 0 si el divisor es 0.
- **`imprimirFila`:** una fila con anchos fijos. La capacidad 0 (sin caché) y la altura -1 (sin índice)
  se muestran como `-`.
- **`exportarCsv`:** la misma fila, 11 campos con `;` y coma decimal (`textoDecimal` cambia el punto por
  coma), para que Excel en español lo abra bien.

## 13. Las pruebas

- `apps/main_pruebas.c` llama a `pruebas001()` … `pruebas004()` y a `imprimirResumen()`; sale con 1 si
  hubo alguna falla.
- Cada `pruebas/pruebas_00N.c` tiene los casos de `specs/00N-*/casos-prueba.md` con el mismo ID. Cada
  caso arma un texto con lo obtenido y lo compara con lo esperado (`verificarTexto`, `verificarInt` de
  `pruebas/verificar.c`), que imprime `[OK]` o `[FALLA] … (esperado …, obtenido …)`.
- Las de la spec 002 llaman a `verificarLruD` después de **cada** operación; las de la 003 corren las
  mismas secuencias por la interfaz y por la primitiva directa y exigen el mismo resultado.
- Estado al 27/10/2026: **128 OK, 0 FALLA**, sin reportes del sanitizer (corrido en Linux; MinGW no lo
  trae).

## 14. Quién libera cada cosa

| Memoria | La pide | La libera |
| --- | --- | --- |
| La `tLruS` / `tLruD` de una caché | `crearCacheSimple` / `crearCacheDoble` | `vaciarCache` (`free(impl)`) |
| Nodo y bloque `[clave][valor]` de la lista | `insPrinLista` / `insPrinListaD` | `sacarPorClaveLista`, `sacarUltLista`, `sacarNodoListaD`, `sacarUltListaD`, `vaciarLista`, `vaciarListaD` |
| Nodo y bloque `[clave][posición]` del árbol | `insArbol` | `eliminarArbol`, `vaciarArbol` |
| Bloques temporales (`crearEntrada`, buffers de búsqueda) | la función que los usa | la misma función, en todos los caminos |

La lista es dueña de los datos y el árbol solo de su índice: ningún bloque tiene dos dueños, así que
nada se libera dos veces.

## 15. Dónde tocar para cambiar algo

| Quiero… | Archivos | El simulador cambia |
| --- | --- | --- |
| Una operación nueva del árbol o de una lista | `tda/*.c` y su `.h` | no |
| Una operación nueva de la caché que use el simulador | primitiva en `lru_simple`/`lru_doble`, adaptadores y campo en `cache.h`/`cache.c` | no (solo la llamada nueva) |
| Una variante o política nueva (FIFO, MRU) | una implementación nueva + `crearCacheX` en `cache.c` + agregarla en `variantes[]` de `main_simulador` | no |
| Un lote nuevo | una estrategia en `generador.c`/`.h` + `main_generador` + `lotes[]` de `main_simulador` | no |
| Un modo de acceso nuevo | un lector en `posteo.c`/`.h` + `modos[]` de `main_simulador` | no |
| Una columna nueva en el reporte | `sim/reporte.c` (tabla y CSV) | no |
