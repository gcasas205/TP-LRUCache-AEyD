# Validación — Spec 003

Fecha: 27/10/2026 · Commit: bd1c656 (con cambios sin commitear en el árbol de trabajo) · Validador: agente validador

VEREDICTO: APROBADO

## Compilación
| Configuración | Resultado |
| --- | --- |
| Estricta del repo (`-std=gnu89 ... -Werror`, `bin/pruebas`) | Sin errores ni warnings |
| Cátedra (`-Wall -Wextra -pedantic -Werror`, `bin/pruebas_cat`) | Sin errores ni warnings |

gcc de CodeBlocks (MinGW-W64 14.2.0) agregado al PATH.

## Pruebas
`RESUMEN: 104 OK, 0 FALLA` (las cuatro specs; los 25 casos de la spec 003 aparecen en OK, con CP-003-18, 19 y 23 una vez por variante).

Sanitizer: no disponible (MinGW no tiene `-lasan` ni `-lubsan`; falló el enlace). Los casos de memoria CP-003-15, 22 y 23 se resolvieron por revisión:
- CP-003-15: `vaciarArbol` libera solo `info` y el nodo, no lo apuntado por los datos.
- CP-003-22: `crearLruS`/`crearLruD` devuelven `ERROR_PARAMETRO` antes de reservar nada y `crearCacheSimple`/`crearCacheDoble` liberan la estructura.
- CP-003-23: `vaciarCache` vacía la variante (lista e índice), libera `impl` y deja la caché como "sin caché".

## RF por RF
| RF | Casos | Resultado |
| --- | --- | --- |
| RF-01 | 02, 16 | Cumplido |
| RF-02 | 03, 16 | Cumplido |
| RF-03 | 04, 05, 16 | Cumplido |
| RF-04 | 06 a 10, 16 | Cumplido |
| RF-05 | 11, 16 | Cumplido |
| RF-06 | 01, 02, 10, 12, 16 | Cumplido |
| RF-07 | 13, 16 | Cumplido |
| RF-08 | 14, 15, 16 | Cumplido |
| RF-09 | 18, 19, 24, 25 | Cumplido |
| RF-10 | 17 | Cumplido |
| RF-11 | 18, 19 | Cumplido |
| RF-12 | 17, 20, 21 | Cumplido |
| RF-13 | 22 | Cumplido |
| RF-14 | 23 | Cumplido |

## Criterios de finalización
- Todos los casos en OK, incluidos los de revisión: cumplido (25 de 25).
  - CP-003-16: `arbol.c` inserta y busca de forma iterativa con puntero a puntero (`buscarNodoArbol`); altura, cantidad, recorrido y vaciado son recursivos con caso base primero; cada `malloc` está chequeado.
  - CP-003-25: solo `cache/cache.c` incluye `lru_simple.h` y `lru_doble.h` (los demás son pruebas y los propios módulos). Los archivos de `sim/` están vacíos todavía, así que no incluyen nada; conviene volver a mirar cuando se escriba la spec 004.
- `/revisar 003`: no es parte de esta validación; el grupo debe confirmarlo.
- Constitución: compila limpio en las dos configuraciones (1); sin fugas por revisión (5, sin sanitizer).

## Cambios necesarios
Ninguno bloqueante.
1. Observación: sin sanitizer en esta máquina, la ausencia de fugas se resolvió por revisión; correr `bin/pruebas_asan` en Linux si se puede.
2. Observación: `cache/cache.c:152` — `crearSinCache` usa `alturaAdapS` (devuelve -1) como adaptador de altura; funciona pero el nombre confunde.
3. Observación: `tda/arbol.c:116` — línea en blanco antes de la llave de cierre en `alturaArbol` (estilo).
