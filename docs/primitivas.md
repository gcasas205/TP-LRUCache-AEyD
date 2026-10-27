# Contratos de primitivas

Copia en el repo de la pestaña "Checklist de primitivas" del doc compartido. Es el contrato que revisan
los agentes: la firma, lo que hace y lo que devuelve tienen que coincidir exactamente con el código. El
avance se marca en `tasks.md` de cada spec, no acá.

Cada primitiva dice:

- **Hace:** el efecto exacto sobre la estructura.
- **Devuelve:** el valor de retorno y qué queda en los parámetros de salida en cada caso.
- **Stub:** una versión hardcodeada mínima para que el resto compile mientras la real no existe. Un stub
  nunca se entrega.

Las funciones `static` son internas del `.c` y no van en el `.h`. "Copia con `MIN`" significa
`memcpy(destino, origen, MIN(tamGuardado, tamDestino))`.

---

## Spec 001 — Persona 1

### comun.h

- **Códigos de retorno** con `#define`: `EXITO 0`, `ERROR_MALLOC 1`, `NO_ENCONTRADO 2`, `DUPLICADO 3`,
  `VACIA 4`, `ERROR_ARCHIVO 5`, `FIN_ARCHIVO 6`, `ERROR_PARAMETRO 7`.
  - Todos distintos y `EXITO` en 0, así `if(ret != EXITO)` funciona siempre. Se usan siempre por nombre.
- **Estados de lista** con `#define` (los devuelven `listaVacia`, `listaLlena` y `listaVaciaD`):
  `LISTA_DISPONIBLE 0`, `LISTA_VACIA 8`, `LISTA_LLENA 9`.
  - Estilo de la cátedra (`PILA_VACIA`, `PILA_LLENA`, `PILA_DISPONIBLE`): pedido de los docentes para
    no devolver 1/0 sueltos. `LISTA_VACIA` y `LISTA_LLENA` no repiten ningún código de retorno (antes
    `LISTA_VACIA 1` chocaba con `ERROR_MALLOC`). Se comparan siempre por nombre:
    `if(listaVacia(&lista) == LISTA_VACIA)`.
- **Macro `MIN(a, b)`**
  - Hace: `((a) < (b) ? (a) : (b))`, con paréntesis en cada argumento.
- **`typedef int (*tCmp)(const void*, const void*)`**
  - Devuelve: negativo, 0 o positivo si la primera clave es menor, igual o mayor. Solo lee los primeros
    bytes de cada puntero, los de la clave.
- **`typedef void (*tAccion)(void *info, void *contexto)`**
  - Hace: lo que quiera el que recorre (imprimir, contar). No devuelve nada.

### lista (simple)

- **`void crearLista(tLista *pLista)`**
  - Hace: `*pLista = NULL`.
  - Stub: es la versión real, una línea.
- **`int listaVacia(const tLista *pLista)`**
  - Devuelve: `LISTA_VACIA` si no hay nodos, `LISTA_DISPONIBLE` si hay.
- **`int listaLlena(const tLista *pLista, size_t tamInfo)`**
  - Hace: pide un nodo y un bloque de `tamInfo` bytes con `malloc` y los libera enseguida. No modifica
    la lista. Es la primitiva que da la cátedra; `insPrinLista` la consulta antes de reservar (decisión
    del grupo), y además sigue chequeando sus propios `malloc`.
  - Devuelve: `LISTA_LLENA` si alguno de los dos `malloc` falló (no hay memoria para otro nodo),
    `LISTA_DISPONIBLE` si los dos salieron bien.
  - Stub: `return LISTA_DISPONIBLE;`
- **`int insPrinLista(tLista *pLista, const void *info, size_t tamInfo)`**
  - Hace: si `listaLlena` da `LISTA_LLENA`, devuelve `ERROR_MALLOC` sin tocar la lista. Si no, reserva nodo e
    `info`, copia `tamInfo` bytes, engancha el nodo como primero.
  - Devuelve: `EXITO`; o `ERROR_MALLOC` si falla cualquiera de los dos `malloc`, liberando lo que haya
    pedido y sin modificar la lista.
  - Stub: `return EXITO;` sin insertar.
- **`int buscarLista(const tLista *pLista, const void *clave, void *info, size_t tamInfo, tCmp cmp)`**
  - Hace: recorre hasta `cmp(nodo->info, clave) == 0`. No modifica la lista.
  - Devuelve: `EXITO` y copia con `MIN` en `info`; o `NO_ENCONTRADO` sin tocar `info`.
  - Stub: `return NO_ENCONTRADO;`
- **`int llevarAlPrinLista(tLista *pLista, const void *clave, tCmp cmp)`**
  - Hace: si encuentra la clave, desengancha ese nodo y lo engancha primero, sin `malloc` ni `free`. Si
    ya es el primero, no cambia nada.
  - Devuelve: `EXITO` o `NO_ENCONTRADO` (lista intacta).
  - Stub: `return NO_ENCONTRADO;`
- **`int verPrimeroLista(const tLista *pLista, void *info, size_t tamInfo)`**
  - Hace: copia con `MIN` el `info` del primer nodo. La usa `obtenerLruS` después de `llevarAlPrinLista`.
  - Devuelve: `EXITO` o `VACIA`.
  - Stub: `return VACIA;`
- **`int sacarPorClaveLista(tLista *pLista, const void *clave, void *info, size_t tamInfo, tCmp cmp)`**
  - Hace: desengancha el nodo de esa clave, copia con `MIN` si `info != NULL`, libera `info` y nodo.
  - Devuelve: `EXITO` o `NO_ENCONTRADO`.
  - Stub: `return NO_ENCONTRADO;`
- **`int sacarUltLista(tLista *pLista, void *info, size_t tamInfo)`**
  - Hace: avanza con puntero a puntero hasta el último, copia con `MIN` si `info != NULL`, libera y deja
    el `sig` anterior en `NULL`.
  - Devuelve: `EXITO` o `VACIA`.
  - Stub: `return VACIA;`
- **`void recorrerLista(const tLista *pLista, tAccion accion, void *contexto)`**
  - Hace: llama a `accion(nodo->info, contexto)` del primero al último.
  - Stub: cuerpo vacío.
- **`int vaciarLista(tLista *pLista)`**
  - Hace: libera `info` y nodo de todos; deja `*pLista = NULL`.
  - Devuelve: la cantidad de nodos liberados (sirve en las pruebas para detectar fugas).
  - Stub: `*pLista = NULL; return 0;`

### entrada

Módulo chico en `cache/` que usan las dos cachés: arma y lee el bloque `[clave][valor]`.

- **`int crearEntrada(void **bloque, const void *clave, size_t tamClave, const void *valor, size_t tamValor)`**
  - Hace: `*bloque = malloc(tamClave + tamValor)`, copia la clave al principio y el valor a
    continuación. Quien la llama libera el bloque con `free` después de insertarlo en la lista (la lista
    hace su propia copia).
  - Devuelve: `EXITO`; o `ERROR_MALLOC` con `*bloque = NULL`.
- **`size_t copiarValorEntrada(const void *bloque, size_t tamBloque, size_t tamClave, void *valor, size_t tamValor)`**
  - Hace: `memcpy(valor, (const char*)bloque + tamClave, MIN(tamBloque - tamClave, tamValor))`.
  - Devuelve: la cantidad de bytes copiados.
  - Stub: `memset(valor, 0, tamValor); return 0;`

### lru_simple (Parte 1)

Estructura: `tLruS` con `tLista lista`, `unsigned capacidad`, `unsigned cantidad` y `tCmp cmp`.

- **`int crearLruS(tLruS *pLru, unsigned capacidad, tCmp cmp)`**
  - Hace: crea la lista vacía, guarda capacidad y `cmp`, `cantidad = 0`.
  - Devuelve: `EXITO`; o `ERROR_PARAMETRO` si `capacidad == 0` o `cmp == NULL`.
- **`int agregarLruS(tLruS *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor)`**
  - Hace: arma la entrada. Si la clave existe, `sacarPorClaveLista` e `insPrinLista` (cantidad igual).
    Si no existe, `insPrinLista`, `cantidad++`, y si `cantidad > capacidad`, `sacarUltLista` y
    `cantidad--`.
  - Devuelve: `EXITO`; o `ERROR_MALLOC`. Si falla al reemplazar, la clave queda fuera de la caché: es
    aceptable y se aclara en el informe.
  - Stub: `return EXITO;` (todo será fallo: equivale a "sin caché").
- **`int obtenerLruS(tLruS *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)`**
  - Hace: `llevarAlPrinLista`; si encontró, pide un buffer temporal de `tamClave + tamValor`, copia
    `valor` (los `tamValor` bytes del llamador) a partir de `tamClave` en el temporal, hace
    `verPrimeroLista` al temporal y `copiarValorEntrada` a `valor`. La precarga hace que, si el valor
    guardado es más corto que `tamValor`, el resto del buffer del llamador quede como estaba (ver
    `plan.md` de la spec 001).
  - Devuelve: `EXITO` con el valor copiado y la entrada primera; `NO_ENCONTRADO` sin tocar `valor` ni el
    orden; o `ERROR_MALLOC` si no pudo pedir el buffer.
  - Stub: `return NO_ENCONTRADO;`
- **`int borrarLruS(tLruS *pLru, const void *clave, size_t tamClave)`**
  - Hace: `sacarPorClaveLista` con `info = NULL`; si borró, `cantidad--`.
  - Devuelve: `EXITO` o `NO_ENCONTRADO`.
- **`void vaciarLruS(tLruS *pLru)`**
  - Hace: `vaciarLista`, `cantidad = 0`. La caché queda reutilizable con la misma capacidad.

---

## Spec 002 — Persona 2

### lista_doble

Estructura: `tListaD` con `tNodoD *pri` y `tNodoD *ult`; cada `tNodoD` con `info`, `tamInfo`, `ant` y
`sig`. La posición se expone como `typedef void* tPosListaD`.

- **`void crearListaD(tListaD *pLista)`**
  - Hace: `pri` y `ult` en `NULL`.
- **`int listaVaciaD(const tListaD *pLista)`**
  - Devuelve: `LISTA_VACIA` si `pri == NULL`, `LISTA_DISPONIBLE` si no.
- **`int insPrinListaD(tListaD *pLista, const void *info, size_t tamInfo, tPosListaD *pos)`**
  - Hace: crea el nodo con `ant = NULL` y `sig = pri`; si la lista estaba vacía, también `ult` apunta al
    nuevo.
  - Devuelve: `EXITO` y `*pos` = el nodo nuevo; o `ERROR_MALLOC` con `*pos = NULL` y la lista intacta.
  - Stub: `*pos = NULL; return ERROR_MALLOC;` (fuerza a probar el camino de error).
- **`size_t verInfoListaD(tPosListaD pos, void *info, size_t tamInfo)`**
  - Hace: copia con `MIN` el `info` de ese nodo.
  - Devuelve: el `tamInfo` guardado en el nodo, aunque haya copiado menos. Así la caché sabe cuánto mide
    el valor.
  - Stub: `return 0;`
- **`int actualizarInfoListaD(tPosListaD pos, const void *info, size_t tamInfo)`**
  - Hace: pide la memoria nueva, copia, libera la vieja y actualiza `tamInfo`. No mueve el nodo.
  - Devuelve: `EXITO`; o `ERROR_MALLOC` y el nodo conserva su `info` anterior.
  - Stub: `return EXITO;` sin cambiar nada.
- **`void moverAlPrinListaD(tListaD *pLista, tPosListaD pos)`**
  - Hace: desengancha el nodo (arreglando `sig` del anterior y `ant` del siguiente, o `ult` si era el
    último) y lo engancha primero. Si ya era el primero, nada.
  - Devuelve: nada; no puede fallar.
  - Stub: cuerpo vacío (la caché funciona, pero desaloja mal).
- **`int sacarNodoListaD(tListaD *pLista, tPosListaD pos, void *info, size_t tamInfo)`**
  - Hace: desengancha (casos único, primero, último, del medio), copia con `MIN` si `info != NULL`,
    libera `info` y nodo. `pos` queda inválida.
  - Devuelve: `EXITO`.
  - Stub: `return EXITO;`
- **`int verUltListaD(const tListaD *pLista, void *info, size_t tamInfo)`**
  - Hace: copia con `MIN` el `info` de `ult`.
  - Devuelve: `EXITO` o `VACIA`.
  - Stub: `return VACIA;`
- **`int sacarUltListaD(tListaD *pLista, void *info, size_t tamInfo)`**
  - Hace: como `sacarNodoListaD` sobre `ult`, sin recorrer.
  - Devuelve: `EXITO` o `VACIA`.
  - Stub: `return VACIA;`
- **`void recorrerListaD(const tListaD *pLista, tAccion accion, void *contexto)`**
  - Hace: `accion(info, contexto)` de `pri` a `ult`.
- **`int vaciarListaD(tListaD *pLista)`**
  - Hace: libera todos; `pri` y `ult` en `NULL`.
  - Devuelve: cantidad de nodos liberados.
  - Stub: `pLista->pri = pLista->ult = NULL; return 0;`

### lru_doble (Parte 2)

Estructura: `tLruD` con `tListaD lista`, `tArbol indice`, `capacidad`, `cantidad` y `cmp`. El `info` del
árbol es `[clave][tPosListaD]`.

- **`int crearLruD(tLruD *pLru, unsigned capacidad, tCmp cmp)`**
  - Hace y devuelve: igual que `crearLruS`, creando también el árbol vacío.
- **`int agregarLruD(tLruD *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor)`**
  - Hace: `buscarArbol`. Si existe: `actualizarInfoListaD` y `moverAlPrinListaD`. Si no existe y está
    llena: `verUltListaD`, `eliminarArbol` con esa clave, `sacarUltListaD`, `cantidad--`. Después
    `insPrinListaD`, `insArbol` con `[clave][pos]`, `cantidad++`.
  - Devuelve: `EXITO`; o `ERROR_MALLOC` dejando lista y árbol consistentes (si falla `insArbol`, saca el
    nodo recién insertado en la lista).
  - Stub: `return EXITO;`
- **`int obtenerLruD(tLruD *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)`**
  - Hace: `buscarArbol` a un buffer de `tamClave + sizeof(tPosListaD)`, saca la posición con `memcpy`,
    `moverAlPrinListaD` y copia el valor.
  - Devuelve: `EXITO`, `NO_ENCONTRADO` o `ERROR_MALLOC`, igual que la Parte 1.
  - Stub: `return NO_ENCONTRADO;`
- **`int borrarLruD(tLruD *pLru, const void *clave, size_t tamClave)`**
  - Hace: `buscarArbol` (para la posición), `eliminarArbol`, `sacarNodoListaD`, `cantidad--`.
  - Devuelve: `EXITO`, `NO_ENCONTRADO`, o `ERROR_MALLOC` si no pudo pedir el buffer para buscar en el
    árbol (sin tocar nada).
- **`void vaciarLruD(tLruD *pLru)`**
  - Hace: `vaciarArbol` y `vaciarListaD`, `cantidad = 0`.
- **`int alturaIndiceLruD(const tLruD *pLru)`**
  - Devuelve: `alturaArbol(&pLru->indice)`.
  - Stub: `return 0;`
- **`int verificarLruD(const tLruD *pLru)`**
  - Devuelve: 1 si `cantNodosArbol` = nodos de la lista = `cantidad` y `cantidad <= capacidad`; 0 si
    no. Se llama en las pruebas después de cada operación.

---

## Spec 003 — Persona 3

### arbol

Estructura: `typedef tNodoA* tArbol`; cada `tNodoA` con `info`, `tamInfo`, `izq` y `der`. Stub completo
para que la Persona 2 avance sin árbol: un vector estático de 1000 infos con búsqueda lineal detrás de
las mismas firmas.

- **`void crearArbol(tArbol *pArbol)`**
  - Hace: `*pArbol = NULL`.
- **`int insArbol(tArbol *pArbol, const void *info, size_t tamInfo, tCmp cmp)`**
  - Hace: baja iterativamente con puntero a puntero comparando `cmp(info, nodo->info)` y engancha como
    hoja.
  - Devuelve: `EXITO`; `DUPLICADO` si la clave ya está (no inserta ni modifica); o `ERROR_MALLOC`.
  - Stub: `return EXITO;`
- **`int buscarArbol(const tArbol *pArbol, const void *clave, void *info, size_t tamInfo, tCmp cmp)`**
  - Hace: baja iterativamente por dato parcial; no modifica.
  - Devuelve: `EXITO` y copia con `MIN`; o `NO_ENCONTRADO` sin tocar `info`.
  - Stub: `return NO_ENCONTRADO;`
- **`int eliminarArbol(tArbol *pArbol, const void *clave, tCmp cmp)`**
  - Hace: hoja: libera y pone `NULL`. Un hijo: el hijo ocupa su lugar. Dos hijos: copia el `info` del
    menor del subárbol derecho y elimina ese menor. Después sigue siendo ABB.
  - Devuelve: `EXITO` o `NO_ENCONTRADO`.
  - Stub: `return NO_ENCONTRADO;`
- **`int alturaArbol(const tArbol *pArbol)`**
  - Devuelve: 0 si está vacío, 1 si solo hay raíz, `1 + max(izq, der)` en general.
  - Stub: `return 0;`
- **`int cantNodosArbol(const tArbol *pArbol)`**
  - Devuelve: 0 si está vacío; `1 + izq + der` en general.
  - Stub: `return 0;`
- **`void recorrerInOrdenArbol(const tArbol *pArbol, tAccion accion, void *contexto)`**
  - Hace: izquierdo, `accion(info, contexto)`, derecho; queda en orden ascendente de clave.
- **`int vaciarArbol(tArbol *pArbol)`**
  - Hace: pos-orden; libera `info` y nodo de cada uno; `*pArbol = NULL`. Nunca toca lo que apunten los
    `info`.
  - Devuelve: cantidad de nodos liberados.
- **`static tArbol *buscarNodoArbol(tArbol *pArbol, const void *clave, tCmp cmp)`**
  - Devuelve: la dirección del puntero que apunta al nodo de esa clave; si no existe, la del `NULL`
    donde iría. Nunca devuelve `NULL` ella misma.
- **`static tArbol *menorNodoArbol(tArbol *pArbol)`**
  - Devuelve: la dirección del puntero al nodo más a la izquierda del subárbol (que no es vacío).

Nota: `alturaArbol`, `cantNodosArbol`, `recorrerInOrdenArbol` y `vaciarArbol` son recursivas. Con el
árbol degenerado de 5000 nodos la profundidad es 5000, que entra en la pila por defecto; buscar e
insertar se hacen iterativas porque se llaman millones de veces.

### cache (interfaz común)

Estructura: `tCache` con `void *impl`, cinco punteros a adaptadores (`agregar`, `obtener`, `borrar`,
`vaciar` y `altura`) y `const char *nombre`. Cada operación devuelve exactamente lo que devuelve la
implementación de abajo. Las firmas de los punteros son `typedef` en `cache.h` (`tAgregarImpl`,
`tObtenerImpl`, `tBorrarImpl`, `tVaciarImpl`, `tAlturaImpl`); todas reciben la implementación como
`void *impl`, salvo `tAlturaImpl`, que recibe `const void *impl`. `cache.h` solo incluye `<stddef.h>` y
`comun.h`: no conoce `lru_simple.h` ni `lru_doble.h`.

- **`int crearCacheSimple(tCache *pCache, unsigned capacidad, tCmp cmp)`**
  - Hace: `malloc` de una `tLruS`, `crearLruS`, carga los adaptadores de la Parte 1 y el nombre.
  - Devuelve: `EXITO`, `ERROR_MALLOC` o `ERROR_PARAMETRO` (liberando lo pedido).
- **`int crearCacheDoble(tCache *pCache, unsigned capacidad, tCmp cmp)`**
  - Igual, con `tLruD` y los adaptadores de la Parte 2.
- **`void crearSinCache(tCache *pCache)`**
  - Hace: `impl = NULL`; adaptadores que no hacen nada.
  - Comportamiento: `agregar` devuelve `EXITO`; `obtener` y `borrar` devuelven `NO_ENCONTRADO`; `altura`
    devuelve -1.
- **`int agregarCache(tCache *pCache, const void *clave, size_t tamClave, const void *valor, size_t tamValor)`**
- **`int obtenerCache(tCache *pCache, const void *clave, size_t tamClave, void *valor, size_t tamValor)`**
- **`int borrarCache(tCache *pCache, const void *clave, size_t tamClave)`**
  - Hacen: llaman al adaptador correspondiente. Devuelven lo mismo que `agregarLruX`, `obtenerLruX` y
    `borrarLruX`.
- **`void vaciarCache(tCache *pCache)`**
  - Hace: vacía la implementación, hace `free(impl)` e `impl = NULL`. Después de esto la `tCache` no se
    usa más hasta un nuevo `crear`.
- **`const char *nombreCache(const tCache *pCache)`**
  - Devuelve: "Sin cache" (sin tilde), "Lista simple" o "Lista doble + ABB".
- **`int alturaIndiceCache(const tCache *pCache)`**
  - Hace: llama al adaptador `altura` (sin `if` sobre la variante).
  - Devuelve: la altura del árbol en la Parte 2; -1 en las otras dos.
- **Adaptadores `static`** (`agregarAdapS`, `obtenerAdapS`, `borrarAdapS`, `vaciarAdapS`, `alturaAdapS` y
  los cinco `AdapD`)
  - Hacen: castean `void *impl` a `tLruS*` o `tLruD*` y llaman a la primitiva. Devuelven lo que ella
    devuelve. `alturaAdapD` llama a `alturaIndiceLruD`; `alturaAdapS` devuelve -1 (la Parte 1 no tiene
    índice).

---

## Spec 004 — Persona 4

### posteo y usuario

- **`int leerPosteoPorId(FILE *fp, int id, tPosteo *posteo)`**
  - Hace: `fseek(fp, (long)id * sizeof(tPosteo), SEEK_SET)` y un `fread`.
  - Devuelve: `EXITO`; `NO_ENCONTRADO` si `id < 0` o el `fread` no leyó un registro (id fuera del
    archivo); `ERROR_ARCHIVO` si falla el `fseek`.
  - Stub: `posteo->id = id;` y `sprintf(posteo->texto, "Posteo %d", id); return EXITO;` sin tocar el
    archivo.
- **`typedef int (*tLectorPosteo)(FILE *fp, int id, tPosteo *posteo)`**
  - El modo de acceso que usa el simulador en cada fallo. `leerPosteoPorId` y `leerPosteoConSeek` tienen
    esta firma; otro modo (por ejemplo en la defensa) es otra función igual.
- **`int leerPosteoConSeek(FILE *fp, int id, tPosteo *posteo)`**
  - Hace: `fseek(fp, 0L, SEEK_END)` (seek simulado) y después lo mismo que `leerPosteoPorId`.
  - Devuelve: lo mismo que `leerPosteoPorId`; `ERROR_ARCHIVO` si falla el `fseek` al final.
  - Stub: `return leerPosteoPorId(fp, id, posteo);`
- **`int escribirPosteo(FILE *fp, const tPosteo *posteo)`**
  - Devuelve: `EXITO` si `fwrite` devolvió 1; si no, `ERROR_ARCHIVO`.
- **`int cmpIdPosteo(const void *a, const void *b)`**
  - Hace: compara dos `int` con `(x > y) - (x < y)`. No usa resta, que puede desbordar.
  - Devuelve: -1, 0 o 1.
- **`int leerUsuario(FILE *fp, tUsuario *usuario)`**
  - Devuelve: `EXITO` si leyó un registro; `FIN_ARCHIVO` si `fread` devolvió 0.
  - Stub: devuelve 3 usuarios hardcodeados (feeds 0 a 19, 0 a 19 y 20 a 39, para ver aciertos) con un
    contador `static`, y después `FIN_ARCHIVO`.
- **`int escribirUsuario(FILE *fp, const tUsuario *usuario)`**
  - Devuelve: `EXITO` o `ERROR_ARCHIVO`.

### generador

- **`int generarPosteos(const char *ruta, int cantidad)`**
  - Hace: escribe `cantidad` registros con `id = i` y un texto de menos de 141 caracteres terminado en
    `'\0'`.
  - Devuelve: `EXITO` o `ERROR_ARCHIVO`.
- **`int generarLote(const char *ruta, int cantUsuarios, tEstrategiaFeed estrategia, void *contexto)`**
  - Hace: por cada usuario, nombre `usuario0001` (cabe en 15 con el `'\0'`) y
    `estrategia(feed, nUsuario, contexto)`.
  - Devuelve: `EXITO`, `ERROR_ARCHIVO`, o `ERROR_PARAMETRO` si `cantUsuarios < 200`.
- **`void feedConcentrado(int feed[20], int nUsuario, void *contexto)`**
  - Hace: 20 ids al azar en `[0, grupo)`, con `grupo = *(int*)contexto` (por ejemplo 300).
- **`void feedDisperso(int feed[20], int nUsuario, void *contexto)`**
  - Hace: 20 ids al azar en `[0, 49999]`.
- **`void feedOrdenado(int feed[20], int nUsuario, void *contexto)`**
  - Hace: con `usuarios = *(int*)contexto`, el pedido `j = nUsuario × 20 + i` recibe el id
    `(int)((double)j * 50000.0 / ((double)usuarios * 20.0))`. Los ids nunca decrecen y cubren el rango
    de forma pareja. El cálculo va en `double` porque `j × 50000` desborda el `long` de 32 bits de
    MinGW con 50000 usuarios. No usa azar.
- **`static int aleatorioEntre(int min, int max)`**
  - Devuelve: un entero en `[min, max]`.
  - Ojo: en MinGW `RAND_MAX` vale 32767, menos que 49999. Hay que combinar dos `rand()` (por ejemplo
    `rand() * (RAND_MAX + 1L) + rand()`) antes del `%`, o el lote disperso nunca toca los ids altos.

### metricas

Estructura: `tMetricas` con `long aciertos, fallos, lecturas, posteos, usuarios` y `double segundos`.
Invariantes: con caché, `aciertos + fallos = posteos` y `lecturas = fallos`; sin caché, `aciertos = 0` y
`lecturas = posteos`.

- **`void iniciarMetricas(tMetricas *pMetricas)`**
  - Hace: todo en 0.
- **`registrarAcierto`, `registrarFallo`, `registrarLectura`, `registrarUsuario`, `registrarPosteo`**
  (`void`, reciben `tMetricas*`)
  - Hacen: incrementan su contador en 1.
- **`void fijarTiempo(tMetricas *pMetricas, clock_t inicio, clock_t fin)`**
  - Hace: `segundos = (double)(fin - inicio) / CLOCKS_PER_SEC`.
- **`double promedioPorPosteo(const tMetricas *pMetricas)`** y
  **`double promedioPorUsuario(const tMetricas *pMetricas)`**
  - Devuelven: `segundos / posteos` y `segundos / usuarios`; 0 si el divisor es 0.

### simulador

- **`int simular(const char *rutaPosteos, const char *rutaLote, tCache *pCache, tLectorPosteo lector, tMetricas *pMetricas)`**
  - Hace: abre los dos archivos, `calentarArchivo`, `iniciarMetricas`, toma `clock()`, recorre cada
    usuario y cada id de su feed con `obtenerPosteo`, toma `clock()`, `fijarTiempo`, cierra los
    archivos. No imprime nada durante la medición y no tiene ningún `if` sobre el modo de acceso.
  - Devuelve: `EXITO`; `ERROR_ARCHIVO` si no pudo abrir alguno (cerrando el que sí abrió).
- **`static void calentarArchivo(FILE *fpPosteos)`**
  - Hace: lee el archivo completo de forma secuencial con `fread` (de a un registro o de a bloques) y
    vuelve al principio. Así todas las corridas encuentran el archivo en la memoria del sistema
    operativo. No toca las métricas.
- **`static int obtenerPosteo(tCache *pCache, FILE *fpPosteos, tLectorPosteo lector, int id, tPosteo *posteo, tMetricas *pMetricas)`**
  - Hace: `obtenerCache`; si da `EXITO`, `registrarAcierto`. Si no, `registrarFallo`,
    `lector(fpPosteos, id, posteo)`, `registrarLectura` y `agregarCache`. Siempre `registrarPosteo`.
  - Devuelve: `EXITO` o el código del lector.

### reporte

- **`void imprimirEncabezado(void)`**
  - Hace: imprime los títulos de columna: acceso, lote, caché, capacidad, total (s), por usuario (s),
    por posteo (s), aciertos, fallos, lecturas, altura.
- **`void imprimirFila(const char *acceso, const char *lote, const tCache *pCache, unsigned capacidad, int altura, const tMetricas *pMetricas)`**
  - Hace: una fila alineada con `printf` y anchos fijos. `acceso` es "Directo" o "Con seek"; la
    capacidad de "sin caché" se muestra como "-". `altura` la consulta `main_simulador` con
    `alturaIndiceCache` antes de vaciar; con -1 imprime "-".
- **`void exportarCsv(FILE *fp, const char *acceso, const char *lote, const tCache *pCache, unsigned capacidad, int altura, const tMetricas *pMetricas)`**
  - Hace: la misma fila, 11 campos separados por `;` (Excel en español usa coma decimal).

---

## Programas — todos

- **`main_generador`**
  - Hace: `generador <semilla> <usuarios> <grupo_concentrado>`. Valida `argc`; `srand(semilla)` una
    sola vez; genera `posteos.dat` y los tres lotes en `datos/`.
  - Devuelve: 0 si todo salió bien; 1 con un mensaje de uso si faltan argumentos o falló un archivo.
- **`main_simulador`**
  - Hace: `simulador [directo | seek | ambos]` (sin argumento, `ambos`). Para cada modo, cada lote, la
    corrida sin caché y cada variante con cada capacidad: crear la caché, `simular` con el lector del
    modo, consultar la altura, `imprimirFila`, `exportarCsv`, `vaciarCache`.
  - Devuelve: 0, o 1 si no encontró los archivos o el argumento no es válido.
- **`main_pruebas`**
  - Hace: llama a `pruebas001()` … `pruebas004()` (una por spec, en `pruebas/`), imprime el total de
    casos OK y FALLA.
  - Devuelve: 0 si no hubo fallas; 1 si hubo alguna.
