# Casos de prueba — Spec 001

Cada caso tiene el mismo ID en `pruebas/pruebas_001.c`. Método **prueba** = lo verifica el programa de
pruebas; **revisión** = lo verifica un agente o una persona leyendo el código (o con el sanitizer). El
validador actualiza la columna Estado (`pendiente`, `OK dd/mm`, `FALLA dd/mm`).

Notación: claves `int`, valores cadenas con su `'\0'` (`"A"` mide 2 bytes), `cmp` = comparador de `int`.
"Presente" = `obtener` devuelve `EXITO`; "ausente" = devuelve `NO_ENCONTRADO`. Ojo: obtener cambia el
orden de uso, así que los casos de descarte comprueban primero los ausentes y después los presentes.

## Caché (caja negra: solo primitivas `LruS`)

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-001-01 | RF-01 | prueba | — | crear cap 3 | `EXITO`; 1 ausente | OK 25/10 |
| CP-001-02 | RF-02 | prueba | — | crear cap 0 | `ERROR_PARAMETRO` | OK 25/10 |
| CP-001-03 | RF-02 | prueba | — | crear cap 3 con `cmp = NULL` | `ERROR_PARAMETRO` | OK 25/10 |
| CP-001-04 | RF-03, RF-06 | prueba | cap 3 | agregar(1,"A"); obtener(1) | `EXITO` y el buffer queda "A" | OK 25/10 |
| CP-001-05 | RF-04 | prueba | cap 3; agregar 1, 2, 3 | agregar(4) | 1 ausente; 2, 3 y 4 presentes | OK 25/10 |
| CP-001-06 | RF-04, RF-06 | prueba | cap 3; agregar 1, 2, 3 | obtener(1); agregar(4) | 2 ausente; 1, 3 y 4 presentes | OK 25/10 |
| CP-001-07 | RF-05 | prueba | cap 3; agregar 1, 2, 3 | agregar(1,"Z"); agregar(4) | 2 ausente; obtener(1) da "Z"; 3 y 4 presentes | OK 25/10 |
| CP-001-08 | RF-05 | prueba | cap 2; agregar(1,"A"), agregar(2,"B") | agregar(2,"B2") | 1 presente con "A"; 2 presente con "B2" (no se descartó nada) | OK 25/10 |
| CP-001-09 | RF-05 | prueba | cap 3; agregar(1,"corto") | agregar(1,"un texto mas largo"); obtener(1) con buffer de 32 | buffer = "un texto mas largo" | OK 25/10 |
| CP-001-10 | RF-05 | prueba | cap 3; agregar(1,"un texto mas largo") | agregar(1,"ab"); obtener(1) con buffer de 32 lleno de 'X' | buffer empieza con "ab" y su `'\0'`; el resto sigue en 'X' | OK 25/10 |
| CP-001-11 | RF-07 | prueba | cap 3; agregar 1, 2, 3; buffer = "XXXX" | obtener(9); agregar(4) | `NO_ENCONTRADO`, buffer sigue "XXXX"; después 1 ausente (el orden no cambió) | OK 25/10 |
| CP-001-12 | RF-07 | prueba | cap 3, vacía | obtener(1) | `NO_ENCONTRADO` | OK 25/10 |
| CP-001-13 | RF-08 | prueba | cap 3; agregar 1, 2, 3 | borrar(2); agregar(4) | `EXITO`; 2 ausente; 1, 3 y 4 presentes (había lugar) | OK 25/10 |
| CP-001-14 | RF-08 | prueba | cap 3; agregar 1, 2, 3 | borrar(3); agregar 4, 5 | 1 ausente (seguía siendo el menos reciente); 2, 4 y 5 presentes | OK 25/10 |
| CP-001-15 | RF-08 | prueba | cap 1; agregar(1) | borrar(1); agregar(2) | `EXITO`, `EXITO`; 1 ausente; 2 presente | OK 25/10 |
| CP-001-16 | RF-09 | prueba | cap 3; agregar 1, 2 | borrar(9) | `NO_ENCONTRADO`; 1 y 2 presentes | OK 25/10 |
| CP-001-17 | RF-09 | prueba | cap 3, vacía | borrar(1) | `NO_ENCONTRADO` | OK 25/10 |
| CP-001-18 | RF-10 | prueba | cap 3; agregar 1, 2, 3 | vaciar; agregar 4, 5, 6, 7 | 1, 2 y 3 ausentes; 4 ausente; 5, 6 y 7 presentes | OK 25/10 |
| CP-001-19 | RF-11 | prueba | cap 3, clave `char[15]` con `strcmp`, valor `tPosteo` | agregar("ana", posteo 7); obtener("ana") | `EXITO`; id 7 y mismo texto | OK 25/10 |
| CP-001-20 | RF-11 | prueba | cap 3, clave `int`, valor `double` | agregar(5, 2.5); obtener(5) | `EXITO`; valor 2.5 | OK 25/10 |
| CP-001-21 | RF-12 | prueba | cap 3; agregar(1,"ABCDEFGH") | obtener(1) con buffer de 4 + 1 byte centinela '#' | buffer = 'A','B','C','D'; centinela sigue '#' | OK 25/10 |
| CP-001-22 | RF-04 | prueba | cap 1; agregar(1) | agregar(2) | 1 ausente; 2 presente | OK 25/10 |
| CP-001-23 | RF-04 | prueba | cap 100 | agregar claves 0 a 999 en orden | 0, 450 y 899 ausentes; 900, 950 y 999 presentes | OK 25/10 |
| CP-001-24 | RF-13 | revisión | — | leer `agregarLruS` y `obtenerLruS` | cada `malloc` chequeado; con error se libera lo pedido, se devuelve `ERROR_MALLOC` y la lista no queda con un nodo sin `info` | OK 25/10 |
| CP-001-25 | RF-10, RNF-02 | revisión | — | correr todas las pruebas de 001 con el sanitizer (o leer el código si no hay) | sin fugas ni dobles liberaciones | OK 25/10 |
| CP-001-26 | RNF-01 | revisión | — | leer los `.h` y `.c` de `tda/` y `cache/` | ninguna función, pública o interna, usa snake_case: todas en camelCase (los nombres de archivo como `lru_simple.c` no cuentan) | OK 25/10 |

## Primitivas internas (contratos de `docs/primitivas.md`)

Cubren el plan, no un RF directo. Se prueban con `recorrerLista` imprimiendo las claves del primero al
último. Los datos de la lista son `tReg {int clave; char nombre[10];}` y la búsqueda es por dato parcial
(solo la clave).

| ID | Primitiva | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- |
| CP-001-L01 | `crearLista`, `listaVacia` | — | crear | `listaVacia` = `LISTA_VACIA` | OK 25/10 |
| CP-001-L02 | `insPrinLista` | vacía | insertar 3, 2, 1 al principio | recorrido: 1 2 3; `listaVacia` = `LISTA_DISPONIBLE` | OK 25/10 |
| CP-001-L03 | `buscarLista` | 1 "uno", 2 "dos", 3 "tres" | buscar clave 2 | `EXITO`; registro {2, "dos"}; recorrido sigue 1 2 3 | OK 25/10 |
| CP-001-L04 | `buscarLista` | igual a L03; destino = {0, "XXX"} | buscar clave 9 | `NO_ENCONTRADO`; destino sigue {0, "XXX"} | OK 25/10 |
| CP-001-L05 | `llevarAlPrinLista` | 1 2 3 | llevar 2 | `EXITO`; recorrido 2 1 3 | OK 25/10 |
| CP-001-L06 | `llevarAlPrinLista` | 1 2 3 | llevar 3 (el último) | `EXITO`; recorrido 3 1 2 | OK 25/10 |
| CP-001-L07 | `llevarAlPrinLista` | 1 2 3 | llevar 1 (el primero) | `EXITO`; recorrido 1 2 3 | OK 25/10 |
| CP-001-L08 | `llevarAlPrinLista` | 1 2 3 | llevar 9 | `NO_ENCONTRADO`; recorrido 1 2 3 | OK 25/10 |
| CP-001-L09 | `sacarPorClaveLista` | 1 2 3 4 | sacar 1, después 3, después 4 | cada uno `EXITO` con el registro copiado; recorridos 2 3 4 → 2 4 → 2 | OK 25/10 |
| CP-001-L10 | `sacarPorClaveLista` | 1 2 | sacar 9; sacar 2 con `info = NULL` | `NO_ENCONTRADO`; después `EXITO`; recorrido 1 | OK 25/10 |
| CP-001-L11 | `sacarUltLista` | 1 2 3 | sacar último tres veces y una cuarta | 3, 2, 1 con `EXITO`; la cuarta `VACIA` | OK 25/10 |
| CP-001-L12 | `verPrimeroLista` | vacía, después 5 7 | ver primero en cada caso | `VACIA`; después `EXITO` con 5 | OK 25/10 |
| CP-001-L13 | `vaciarLista` | 5 nodos, después vacía | vaciar dos veces | devuelve 5 y después 0; `listaVacia` = `LISTA_VACIA` | OK 25/10 |
| CP-001-L14 | `crearEntrada`, `copiarValorEntrada` | clave `int` 7, valor "hola" | crear entrada; copiar con buffer 16; copiar con buffer 2 | bloque de `sizeof(int) + 5`; primera copia "hola" y devuelve 5; segunda copia "ho" y devuelve 2 | OK 25/10 |
| CP-001-L15 | `listaLlena` | vacía, después 1 2 3 | preguntar si está llena con `sizeof(tReg)` antes y después de insertar | `LISTA_DISPONIBLE` las dos veces; la lista sigue 1 2 3 (no la modifica) | OK 25/10 |
