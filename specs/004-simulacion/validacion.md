# Validación — Spec 004

Fecha: 27/10/2026 · Commit: 94b6c82 (con cambios sin commitear en el árbol de trabajo) · Validador: agente validador

VEREDICTO: APROBADO

## Compilación
| Configuración | Resultado |
| --- | --- |
| Estricta del repo (`-std=gnu89 ... -Werror`) `bin/pruebas` | OK, sin warnings |
| Cátedra (`-Wall -Wextra -pedantic -Werror`) `bin/pruebas_cat` | OK, sin warnings |
| `bin/generador` y `bin/simulador` (estricta) | OK, sin warnings |

Nota: `gcc` no estaba en el PATH de Git Bash; se agregó `C:\Program Files\CodeBlocks\MinGW\bin` (GCC 14.2.0 MinGW).

## Pruebas
`RESUMEN: 128 OK, 0 FALLA` (las 25 líneas CP-004-xx de tipo prueba aparecen en `[OK]`).

Sanitizer: no disponible en esta máquina (MinGW: `cannot find -lasan` / `-lubsan`), así que no se corrió
`bin/pruebas_asan`. La spec no tiene casos de memoria propios. La corrida con sanitizer en Linux (GCC 11.4, 128 OK,
sin reportes) figura en la validación anterior de esta spec; esta vez no se pudo reproducir.

Comandos (esta corrida):
- CP-004-08: `./bin/generador` sin argumentos, con 150, 1000001, 3000000000 y `abc`: código 1 y mensaje de uso en los
  cinco; la carpeta `datos/` quedó idéntica (mismos archivos y fechas).
- CP-004-23: `./bin/simulador directo` (en una copia de `datos/` fuera del repo, para no pisar el CSV del informe):
  27 filas en pantalla, CSV con encabezado + 27 líneas de 11 campos, todas "Directo".
- CP-004-32: `./bin/simulador ambos`: CSV con 54 filas (27 Directo + 27 Con seek), 11 campos en todas; los
  contadores (aciertos, fallos, lecturas, altura) son idénticos a los de `informe/resultados.csv`.
- CP-004-24 (sobre `informe/resultados.csv`, 54 filas): con caché, aciertos + fallos = 1.600.000 y lecturas = fallos;
  sin caché aciertos 0 y lecturas 1.600.000; altura distinta de "-" solo en Lista doble + ABB; mismos contadores en
  los dos modos y entre Lista simple y Lista doble para igual lote y capacidad.
- CP-004-33: sin caché en modo directo: Concentrado 4,239 s, Disperso 6,577 s, Ordenado 1,132 s (todos ≥ 1 s).

Revisión:
- CP-004-21, 30, 31: en `sim/simulador.c` no hay `printf` ni escrituras entre los dos `clock()`; si falla abrir el lote
  se hace `fclose` del archivo de posteos; `calentarArchivo` lee el archivo completo con `fread` antes del primer
  `clock()` y no toca las métricas; el modo llega como `tLectorPosteo` y `simular` no tiene `if` sobre él.
- CP-004-22: `sim/` incluye `cache.h` (vía `simulador.h` y `reporte.h`), nunca `lru_simple.h` ni `lru_doble.h`.
- CP-004-25: `informe/Informe_TP_LRU_Cache.pdf` (texto leído con `pdftotext`): 3 páginas ("Page 1/2/3 of 3"); tabla de
  aceleración por lote, variante y capacidad, y dos gráficos referidos al CSV; responde las tres preguntas con números
  que coinciden con el CSV (por ejemplo ordenado, Parte 2, capacidad 5000: 41,436 s contra 1,132 s, 37 veces; disperso
  capacidad 100: 8,959 s contra 7,549 s, 19 %; altura 34 en el disperso); anota semilla 2026, 80.000 usuarios, G = 300
  y capacidades; declara el seek como costo simulado con el costo por lectura de cada modo (2,6 a 3,3 µs, 4,1 a 5,0 µs
  y 0,7 a 3,3 µs) y que las lecturas salen de la memoria del sistema operativo.

## RF por RF
| RF | Casos | Resultado |
| --- | --- | --- |
| RF-01 | 01 | Cumplido |
| RF-02 | 02 | Cumplido |
| RF-03 | 03 | Cumplido |
| RF-04 | 04 | Cumplido |
| RF-05 | 05 | Cumplido |
| RF-06 | 06, 28 | Cumplido |
| RF-07 | 07, 08 | Cumplido |
| RF-08 | 09, 10, 13 | Cumplido |
| RF-09 | 21 | Cumplido |
| RF-10 | 11, 12, 19 | Cumplido |
| RF-11 | 13, 14, 15, 24 | Cumplido |
| RF-12 | 17, 24 | Cumplido |
| RF-13 | 23, 32 | Cumplido |
| RF-14 | 16, 18, 24, 29 | Cumplido |
| RF-15 | 20, 21 | Cumplido |
| RF-16 | 25 | Cumplido |
| RF-17 | 13, 30 | Cumplido |
| RF-18 | 23, 27, 29, 31, 32 | Cumplido |
| RF-19 | 26 | Cumplido |
| RNF-02 | 22 | Cumplido |
| RNF-04 | 33 | Cumplido |
| RNF-05 | 31 | Cumplido |

## Criterios de finalización
- Todos los casos en OK: sí (33 de 33).
- Tabla y CSV de todas las corridas en los dos modos: sí (`informe/resultados.csv`, 54 filas, semilla 2026, 80000
  usuarios, G = 300). El CSV y el PDF del informe están sin versionar.
- Informe revisado contra RF-16: el agente lo comprobó (CP-004-25); la revisión del grupo no queda registrada acá.
- `/revisar 004`: no consta en esta validación; `/validar 004`: APROBADO.
- Constitución: principios 1, 4 (RNF-02) y 6 verificados. El 5 (memoria) sin sanitizer en esta máquina; el reporte
  limpio de Linux es el de la validación anterior.
- `spec.md` en "Estado: implementada" y T1 a T10 marcadas por la persona.

## Cambios necesarios
Ninguno. Pendiente de confirmar por el grupo: correr `pruebas_asan` en Linux sobre el commit final y commitear
`informe/` y `pruebas/pruebas_004.c`.
