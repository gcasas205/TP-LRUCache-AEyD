# Casos de prueba — Spec 003

Cada caso tiene el mismo ID en `pruebas/pruebas_003.c`. Métodos y columna Estado: igual que en la spec
001.

## Árbol

Árbol de referencia **T7**: insertar en este orden 50, 30, 70, 20, 40, 60, 80. Queda completo, con raíz
50 y altura 3. Datos: `tReg {int clave; char nombre[10];}`, con nombre "n" + la clave ("n50", "n30"...);
la búsqueda es por dato parcial (solo `clave` cargada). "In-orden" = claves que imprime
`recorrerInOrdenArbol`.

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-003-01 | RF-06 | prueba | árbol vacío | altura; cantidad | 0 y 0 | pendiente |
| CP-003-02 | RF-01, RF-06 | prueba | vacío | construir T7 | in-orden 20 30 40 50 60 70 80; altura 3; cantidad 7 | pendiente |
| CP-003-03 | RF-02 | prueba | T7 | insertar 40 ("otro") | `DUPLICADO`; cantidad 7; buscar 40 da "n40" | pendiente |
| CP-003-04 | RF-03 | prueba | T7 | buscar {60, ""} | `EXITO`; destino {60, "n60"} | pendiente |
| CP-003-05 | RF-03 | prueba | T7; destino {0, "XXX"} | buscar 65 | `NO_ENCONTRADO`; destino sigue {0, "XXX"} | pendiente |
| CP-003-06 | RF-04 | prueba | T7 | eliminar 20 (hoja) | `EXITO`; in-orden 30 40 50 60 70 80; altura 3; cantidad 6 | pendiente |
| CP-003-07 | RF-04 | prueba | T7 menos 20 (30 quedó con un hijo: 40) | eliminar 30 | `EXITO`; in-orden 40 50 60 70 80; cantidad 5 | pendiente |
| CP-003-08 | RF-04 | prueba | T7 | eliminar 70 (dos hijos) | `EXITO`; in-orden 20 30 40 50 60 80; altura 3; buscar 60 y 80 da `EXITO` | pendiente |
| CP-003-09 | RF-04 | prueba | T7 | eliminar 50 (raíz con dos hijos) | `EXITO`; in-orden 20 30 40 60 70 80; altura 3; cantidad 6 | pendiente |
| CP-003-10 | RF-04, RF-06 | prueba | árbol con solo 5 | eliminar 5 | `EXITO`; altura 0; cantidad 0 | pendiente |
| CP-003-11 | RF-05 | prueba | T7; después vacío | eliminar 99 en T7; eliminar 1 en vacío | `NO_ENCONTRADO` en los dos; T7 sin cambios | pendiente |
| CP-003-12 | RF-06 | prueba | vacío | insertar 1, 2, 3 (en orden); después 0 | altura 3; después sigue 3; cantidad 4 | pendiente |
| CP-003-13 | RF-07 | prueba | vacío | insertar 0 a 4999 en orden; buscar 4999; eliminar 0; vaciar | altura 5000; `EXITO`; `EXITO` y altura 4999; vaciar devuelve 4999 | pendiente |
| CP-003-14 | RF-08 | prueba | T7 | vaciar | devuelve 7; altura 0 | pendiente |
| CP-003-15 | RF-08 | prueba | árbol de `{clave, int *dato}` donde cada `dato` es un `int` pedido con `malloc` y guardado aparte | vaciar el árbol; después leer y liberar cada `dato` desde la prueba | los `dato` siguen válidos después de vaciar (el árbol no los liberó); sanitizer sin reportes | pendiente |
| CP-003-16 | RF-01 a RF-08 | revisión | — | leer `arbol.c` | insertar y buscar iterativos con puntero a puntero; recursivas con caso base primero; cada `malloc` chequeado | pendiente |

## Interfaz común

Notación igual que en la spec 001. "Las tres variantes" = sin caché, simple y doble con la misma
capacidad.

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-003-17 | RF-10, RF-12 | prueba | `crearSinCache` | agregar(1,"A"); obtener(1); borrar(1); nombre; altura | `EXITO`; `NO_ENCONTRADO`; `NO_ENCONTRADO`; "Sin caché"; -1 | pendiente |
| CP-003-18 | RF-09, RF-11 | prueba | simple y doble, cap 3 | la secuencia de CP-001-06 con cada una | mismos códigos y mismos presentes/ausentes que la primitiva directa (2 ausente; 1, 3 y 4 presentes) | pendiente |
| CP-003-19 | RF-09, RF-11 | prueba | simple y doble, cap 3 | la secuencia de CP-001-07 con cada una | obtener(1) da "Z" en las dos; 2 ausente en las dos | pendiente |
| CP-003-20 | RF-12 | prueba | simple y doble | consultar nombre | "Lista simple" y "Lista doble + ABB" | pendiente |
| CP-003-21 | RF-12 | prueba | simple y doble, cap 10; agregar 2, 1, 3 en cada una | altura | -1 en simple; 2 en doble | pendiente |
| CP-003-22 | RF-13 | prueba | — | crear simple con cap 0; crear doble con `cmp = NULL` | `ERROR_PARAMETRO` en los dos; sanitizer sin fugas | pendiente |
| CP-003-23 | RF-14 | prueba | simple y doble, cap 3, con 3 elementos | vaciar; volver a crear cap 2; agregar 1, 2, 3 | 1 ausente; 2 y 3 presentes; sanitizer sin fugas | pendiente |
| CP-003-24 | RF-09 | prueba | una función de prueba que recibe `tCache*` | correrla con las tres variantes | la función no tiene ningún `if` sobre la variante; en "sin caché" todo obtener da `NO_ENCONTRADO` | pendiente |
| CP-003-25 | RF-09, RNF-02 | revisión | — | leer `cache.c` y los `#include` del simulador | solo `cache.c` incluye `lru_simple.h` y `lru_doble.h`; el simulador solo incluye `cache.h` | pendiente |
