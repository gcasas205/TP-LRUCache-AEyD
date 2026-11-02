# Validación — Spec 002

Fecha: 27/10/2026 · Commit: 512687f (con cambios sin commitear: `apps/main_pruebas.c` y `tda/arbol.c`, el árbol real) · Validador: agente validador

VEREDICTO: APROBADO

## Compilación
gcc de CodeBlocks (GCC 14.2.0 MinGW-w64), agregado al PATH de Git Bash. Se compiló con `tda/lista.c tda/lista_doble.c tda/arbol.c cache/entrada.c cache/lru_simple.c cache/lru_doble.c pruebas/verificar.c pruebas/pruebas_001.c pruebas/pruebas_002.c apps/main_pruebas.c`.

| Configuración | Resultado |
| --- | --- |
| Estricta del repo (`-std=gnu89 ... -Werror`) -> `bin/pruebas` | OK, sin errores ni warnings |
| Cátedra (`-Wall -Wextra -pedantic -Werror`) -> `bin/pruebas_cat` | OK, sin errores ni warnings |

## Pruebas
`./bin/pruebas` (salida 0): CP-002-D01 a D10, CP-002-01 a 23 y CP-002-26 a 32 en `[OK]` (con el árbol real, no el stub).
`RESUMEN: 78 OK, 0 FALLA`

Sanitizer: no disponible en MinGW (`cannot find -lasan`). Actualización 27/10 (19:30): se corrió aparte en Linux (GCC 11.4, `-fsanitize=address,undefined`, mismos `.c` que la línea de `pruebas_asan` de `AGENTS.md`) con el código a commitear: `RESUMEN: 78 OK, 0 FALLA`, salida 0, sin reportes. CP-002-25 queda verificado por revisión y con herramienta.

## Casos de revisión
- CP-002-24 OK: `agregarLruD` libera `bloque` y `busqueda` en cada error de `malloc`/`crearEntrada`/`insPrinListaD`; si falla `crearEntrada` del índice o `insArbol`, hace `sacarNodoListaD` del nodo recién insertado. `obtenerLruD` chequea sus `malloc` y libera `busqueda`.
- CP-002-25 OK (por revisión): bloque temporal, `busqueda` e `indice` se liberan en todos los caminos; el árbol libera solo su `info` (`eliminarArbol`, `vaciarArbol`) y la lista libera `info` y nodo una vez (`sacarNodoListaD`, `vaciarListaD`). Sin fugas ni dobles liberaciones visibles.
- CP-002-33 OK: el índice guarda una copia `[clave][posición]` que libera solo el árbol; los datos los libera solo la lista; el descarte, `borrarLruD` y `vaciarLruD` eliminan de cada estructura lo suyo una sola vez.
- CP-002-34 OK: `obtenerLruD`, `agregarLruD` y `borrarLruD` buscan con `buscarArbol`; el descarte usa `verUltListaD`/`sacarUltListaD` (`ult`); mover usa `moverAlPrinListaD`. Solo `verificarLruD` recorre la lista.

## RF por RF
| RF | Casos | Resultado |
| --- | --- | --- |
| RF-01 a RF-12 | CP-002-01 a 23 | cumplido |
| RF-13 | CP-002-24 | cumplido |
| RF-14 | CP-002-26 a 29 | cumplido |
| RF-15 | CP-002-01 a 23, 29 a 32 | cumplido |
| RF-16 | CP-002-25, 33 | cumplido (revisión + sanitizer en Linux) |
| RF-17 | CP-002-34 | cumplido |
| RF-18 | CP-002-D04 a D06, 34 | cumplido |
| RNF-01 | compilación en dos configuraciones | cumplido |
| RNF-02 | CP-002-25 | cumplido (revisión + sanitizer en Linux) |

## Criterios de finalización
- Todos los casos de `casos-prueba.md` en OK (22 filas, incluidas las de revisión): cumplido.
- `/revisar 002` con `VEREDICTO: APROBADO`: cumplido (27/10, después de sacar el `if` de `sacarNodoListaD`).
- Pruebas con el sanitizer sin reportes: cumplido (27/10, GCC de Linux).

## Cambios necesarios
1. (Sugerencia, no bloquea) Correr `bin/pruebas_asan` en Linux antes de la entrega, para confirmar RF-16 y RNF-02 con herramienta.
2. (Sugerencia) `tasks.md` tiene T9 y T10 sin marcar y `spec.md` figura como "borrador"; lo decide y marca el grupo. `tda/arbol.c` (spec 003) está sin commitear.
