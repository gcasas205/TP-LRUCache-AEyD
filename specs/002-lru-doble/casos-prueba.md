# Casos de prueba — Spec 002

Cada caso tiene el mismo ID en `pruebas/pruebas_002.c`. Métodos y columna Estado: igual que en la spec
001. Notación: la misma de `specs/001-lru-simple/casos-prueba.md`.

## Mismo comportamiento que la Parte 1 (RF-01 a RF-13)

Los casos CP-002-01 a CP-002-25 son **exactamente** los CP-001-01 a CP-001-25 (misma preparación, acción
y resultado esperado), corridos contra las primitivas `LruD`. Además, en cada uno de ellos se llama a
`verificarLruD` después de **cada** operación y tiene que devolver 1 (eso cubre RF-15).

Consejo de implementación: escribir esos casos una sola vez como una función que recibe la caché a probar
(por ejemplo, contra la interfaz `tCache` de la spec 003) y correrla con las dos versiones. Si la interfaz
todavía no existe, se duplican y después se unifican.

| ID | Cubre | Método | Equivale a | Estado |
| --- | --- | --- | --- | --- |
| CP-002-01 a CP-002-23 | RF-01 a RF-12, RF-15 | prueba | CP-001-01 a CP-001-23 + `verificarLruD` = 1 tras cada operación | pendiente |
| CP-002-24 | RF-13 | revisión | CP-001-24, sobre `agregarLruD` y `obtenerLruD`: si falla `insArbol`, se saca el nodo recién insertado en la lista | pendiente |
| CP-002-25 | RF-10, RF-16 | revisión | CP-001-25 con el sanitizer sobre todas las pruebas de 002 | pendiente |

## Propios de la Parte 2

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-002-26 | RF-14 | prueba | cap 10, vacía | altura; agregar(1); altura | 0; después 1 | pendiente |
| CP-002-27 | RF-14 | prueba | cap 10 | agregar claves 2, 1, 3 | altura 2 | pendiente |
| CP-002-28 | RF-14 | prueba | cap 1000 | agregar claves 0 a 999 en orden creciente | altura 1000; `verificarLruD` = 1 | pendiente |
| CP-002-29 | RF-14, RF-15 | prueba | cap 3; agregar 2, 1, 3 | agregar(4) (descarta 2, la raíz del índice, que tiene dos hijos) | altura 2 (la raíz pasa a ser 3, el menor del subárbol derecho, y 4 cuelga a su derecha); 2 ausente; 1, 3 y 4 presentes; `verificarLruD` = 1 | pendiente |
| CP-002-30 | RF-15 | prueba | cap 3 | agregar(1,"A"); después agregar(1,"B") diez veces | altura 1; obtener(1) da "B"; `verificarLruD` = 1 | pendiente |
| CP-002-31 | RF-15 | prueba | cap 2; agregar 1, 2, 3 (1 descartado) | agregar(1,"otra vez") | `EXITO`; 2 ausente; 1 presente con "otra vez"; 3 presente; `verificarLruD` = 1 | pendiente |
| CP-002-32 | RF-15 | prueba | cap 3; agregar 1, 2, 3 | borrar 1, 2 y 3 | cada uno `EXITO`; altura 0; `verificarLruD` = 1 | pendiente |
| CP-002-33 | RF-16 | revisión | — | leer `vaciarLruD`, `borrarLruD` y el descarte en `agregarLruD` | el árbol libera solo sus `info` (`[clave][posición]`); la lista libera nodos y datos; ningún bloque tiene dos dueños | pendiente |
| CP-002-34 | RF-17, RF-18 | revisión | — | leer `obtenerLruD`, `agregarLruD` y `borrarLruD` | buscan con `buscarArbol`; el descarte usa `ult`; no hay ningún recorrido de la lista doble | pendiente |

## Primitivas internas de la lista doble (contratos de `docs/primitivas.md`)

Se verifican con `recorrerListaD` (de `pri` a `ult`) y con `sacarUltListaD` repetido, que recorre de
`ult` a `pri` y por eso prueba los enlaces `ant`. Datos: enteros 1, 2, 3...

| ID | Primitiva | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- |
| CP-002-D01 | `crearListaD`, `listaVaciaD`, `verUltListaD` | — | crear; ver último | `listaVaciaD` = 1; `VACIA` | pendiente |
| CP-002-D02 | `insPrinListaD` | vacía | insertar 7 | `EXITO`; posición no nula; ver último = 7 (`pri` y `ult` son el mismo nodo) | pendiente |
| CP-002-D03 | `insPrinListaD`, `sacarUltListaD` | vacía | insertar 1, 2, 3; sacar último cuatro veces | recorrido 3 2 1; sacadas 1, 2, 3 y la cuarta `VACIA` | pendiente |
| CP-002-D04 | `moverAlPrinListaD` | insertar 1, 2, 3 (recorrido 3 2 1), guardando las posiciones | mover la posición de 1 (el último) | recorrido 1 3 2; ver último = 2; sacar último ×3 da 2, 3, 1 | pendiente |
| CP-002-D05 | `moverAlPrinListaD` | igual a D04 | mover la posición de 2 (del medio) | recorrido 2 3 1; sacar último ×3 da 1, 3, 2 | pendiente |
| CP-002-D06 | `moverAlPrinListaD` | igual a D04 | mover la posición de 3 (el primero) | recorrido 3 2 1 sin cambios | pendiente |
| CP-002-D07 | `sacarNodoListaD` | insertar 1, 2, 3, 4 (recorrido 4 3 2 1) | sacar 2 (medio), después 4 (primero), después 1 (último) | recorridos 4 3 1 → 3 1 → 3; ver último = 3 | pendiente |
| CP-002-D08 | `sacarNodoListaD` | insertar 5 | sacar su posición | `EXITO`; `listaVaciaD` = 1; `verUltListaD` da `VACIA` | pendiente |
| CP-002-D09 | `actualizarInfoListaD`, `verInfoListaD` | insertar "corto" | actualizar con "un texto mas largo"; ver con buffer 4 | `verInfoListaD` devuelve 19 y copia "un t"; la lista sigue con un nodo | pendiente |
| CP-002-D10 | `vaciarListaD` | insertar 4 nodos; después vacía | vaciar dos veces | devuelve 4 y después 0; `listaVaciaD` = 1 | pendiente |
