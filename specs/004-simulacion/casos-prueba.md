# Casos de prueba — Spec 004

Cada caso de método **prueba** tiene el mismo ID en `pruebas/pruebas_004.c`. Método **comando** = lo
corre el validador desde la terminal y compara la salida o el código de salida; **revisión** = leyendo el
código o el informe. Columna Estado: igual que en la spec 001.

Los archivos que crean las pruebas van en `bin/` con prefijo `cp004_` (no se versionan).

**Lote mini** (lo escriben las pruebas con `escribirUsuario`, sin el generador): posteos =
`generarPosteos("bin/cp004_posteos.dat", 50)`; 3 usuarios con feeds A = ids 0 a 19, B = ids 0 a 19,
C = ids 20 a 39, en ese orden. Total: 60 pedidos. Salvo que el caso diga otra cosa, se simula con el
modo directo (`leerPosteoPorId`).

## Generador y archivos

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-004-01 | RF-01 | prueba | — | `generarPosteos(…, 50000)`; leer los ids 0, 25000 y 49999 | archivo de `50000 × sizeof(tPosteo)` bytes; cada registro leído tiene su mismo id; cada texto tiene un `'\0'` dentro de los 141 bytes | pendiente |
| CP-004-02 | RF-02 | prueba | — | `srand(123)` + lote disperso de 200 en `a`; `srand(123)` + lote igual en `b`; `srand(124)` + lote igual en `c` | `a` y `b` idénticos byte a byte; `c` distinto de `a` | pendiente |
| CP-004-03 | RF-03 | prueba | los tres lotes de 200 usuarios | leer cada lote completo | 200 registros cada uno; cada nombre con `'\0'` dentro de los 15 bytes; todos los ids entre 0 y 49999 | pendiente |
| CP-004-04 | RF-04 | prueba | lote concentrado de 200 con G = 300 | leer todos los ids | todos menores que 300 | pendiente |
| CP-004-05 | RF-05 | prueba | lote disperso de 200 | contar ids distintos y por décima parte del rango | al menos 3600 de 4000 distintos; al menos un id en cada décima parte, incluida 45000-49999 | pendiente |
| CP-004-06 | RF-06 | prueba | lote ordenado de 200 (4000 pedidos) | leer los ids en orden de archivo | primero 0; último 49987; cada id es el anterior + 12 o + 13 | pendiente |
| CP-004-07 | RF-07 | prueba | — | `generarLote("bin/cp004_chico.dat", 150, …)` | `ERROR_PARAMETRO`; el archivo no existe después | pendiente |
| CP-004-08 | RF-07 | comando | — | `./bin/generador` sin argumentos; después con 150 usuarios | código de salida 1 y mensaje de uso en los dos; no se crea ningún `.dat` nuevo | pendiente |
| CP-004-09 | RF-08 | prueba | archivo de CP-004-01 | `leerPosteoPorId` con 49999, 50000 y -1 | `EXITO` con id 49999; `NO_ENCONTRADO`; `NO_ENCONTRADO` | pendiente |
| CP-004-10 | RF-08 | prueba | — | `cmpIdPosteo` con (3, 7), (7, 7), (7, 3) y (`INT_MIN`, 1) | negativo, 0, positivo, negativo (sin desborde) | pendiente |
| CP-004-27 | RF-18 | prueba | archivo de CP-004-01 | `leerPosteoConSeek` con 0, 25000, 49999 y 50000 | los tres primeros dan `EXITO` con el mismo registro que `leerPosteoPorId`; 50000 da `NO_ENCONTRADO` | pendiente |
| CP-004-28 | RF-06 | prueba | lote ordenado de 50000 usuarios (un millón de pedidos) | leer los ids en orden de archivo | primero 0; último 49999; nunca decrece; cada id aparece exactamente 20 veces seguidas (prueba el cálculo sin desborde) | pendiente |

## Métricas

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-004-11 | RF-10 | prueba | `iniciarMetricas` | leer contadores y promedios | todo en 0; los promedios dan 0 (no hay división por cero) | pendiente |
| CP-004-12 | RF-10 | prueba | 4000 `registrarPosteo`, 200 `registrarUsuario`; `fijarTiempo(0, 2 × CLOCKS_PER_SEC)` | promedios | por posteo 0.0005; por usuario 0.01 | pendiente |

## Simulación (con el lote mini)

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-004-13 | RF-08, RF-11, RF-17 | prueba | sin caché | simular | posteos 60; usuarios 3; aciertos 0; fallos 60; lecturas 60 (el calentamiento no suma lecturas) | pendiente |
| CP-004-14 | RF-11 | prueba | Parte 1, cap 20 | simular | aciertos 20; fallos 40; lecturas 40 (B encuentra todo lo que trajo A) | pendiente |
| CP-004-15 | RF-11 | prueba | Parte 1, cap 10 | simular | aciertos 0; fallos 60; lecturas 60 (cada feed es más largo que la caché: LRU descarta justo lo que se va a pedir) | pendiente |
| CP-004-16 | RF-14 | prueba | Parte 2, cap 20 y cap 10 | simular cada una | mismos números que CP-004-14 y CP-004-15 | pendiente |
| CP-004-17 | RF-12 | prueba | Parte 2, cap 20 | simular; altura del índice | 20 (los ids llegan en orden creciente: el índice es una cadena) | pendiente |
| CP-004-18 | RF-14 | prueba | Parte 1, cap 20 | simular dos veces con cachés nuevas | mismos aciertos, fallos y lecturas | pendiente |
| CP-004-19 | RF-10, RNF-03 | prueba | métricas de CP-004-14 | promedios | por posteo = segundos / 60; por usuario = segundos / 3 | pendiente |
| CP-004-29 | RF-14, RF-18 | prueba | Parte 1, cap 20, modo con seek (`leerPosteoConSeek`) | simular | mismos aciertos, fallos y lecturas que CP-004-14 | pendiente |
| CP-004-20 | RF-15 | prueba | — | `simular("bin/cp004_no_existe.dat", …)` | `ERROR_ARCHIVO` | pendiente |
| CP-004-21 | RF-09, RF-15 | revisión | — | leer `simular` y `obtenerPosteo` | no hay `printf` ni escrituras entre los dos `clock()`; si falla abrir el lote, se cierra el archivo de posteos | pendiente |
| CP-004-30 | RF-17 | revisión | — | leer `simular` | el calentamiento lee el archivo de posteos completo en forma secuencial, antes del primer `clock()`, sin tocar las métricas | pendiente |
| CP-004-22 | RNF-02 | revisión | — | leer los `#include` de `sim/` | `simulador.c` y `reporte.c` incluyen `cache.h`, nunca `lru_simple.h` ni `lru_doble.h` | pendiente |
| CP-004-31 | RF-18, RNF-05 | revisión | — | leer `simular` y `main_simulador` | el modo llega como puntero a función (`tLectorPosteo`); `simular` no tiene ningún `if` sobre el modo | pendiente |

## Programa completo e informe

| ID | Cubre | Método | Preparación | Acción | Esperado | Estado |
| --- | --- | --- | --- | --- | --- | --- |
| CP-004-23 | RF-13, RF-18 | comando | `./bin/generador` con la semilla y los parámetros del informe | `./bin/simulador directo` | tabla con 27 filas (3 lotes × [1 sin caché + 2 variantes × 4 capacidades]); CSV con encabezado + 27 líneas de 11 campos separados por `;`; columna de acceso "Directo" en todas | pendiente |
| CP-004-32 | RF-13, RF-18 | comando | igual a CP-004-23 | `./bin/simulador ambos` (o sin argumento) | 54 filas: las 27 de cada modo; columna de acceso "Directo" o "Con seek" | pendiente |
| CP-004-24 | RF-11, RF-12, RF-14 | comando | CSV de CP-004-32 | revisar columnas | en cada fila con caché, aciertos + fallos = posteos y lecturas = fallos; altura "-" salvo en Parte 2; cada corrida tiene los mismos aciertos en los dos modos | pendiente |
| CP-004-33 | RNF-04 | comando | CSV de CP-004-32, en la máquina del informe | revisar las filas sin caché en modo directo | tiempo total ≥ 1 s en las tres; si no, se sube la cantidad de usuarios y se vuelve a generar | pendiente |
| CP-004-25 | RF-16 | revisión | `informe/` | leer el informe | no más de 3 páginas; tablas o gráficos de las corridas; responde las tres preguntas con números; anota semilla y parámetros; declara el seek simulado con el costo por lectura de cada modo y que el archivo se lee desde la memoria del sistema operativo | pendiente |
| CP-004-26 | Duda abierta | prueba | lote mini con un id 9999 en el feed de A (fuera de los 50 posteos) | simular | según lo que decida el grupo; propuesta: cuenta un fallo, no lo agrega, la corrida sigue y termina con `EXITO` | pendiente (depende de la duda) |
