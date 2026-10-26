# Validación — Spec 001

Fecha: 25/10/2026 · Commit: d83fa7e (con cambios sin commitear en el árbol de trabajo) · Validador: agente validador

VEREDICTO: APROBADO

## Compilación
gcc de CodeBlocks (GCC 14.2.0 MinGW-w64), agregado al PATH de Git Bash.

| Configuración | Resultado |
| --- | --- |
| Estricta del repo (`-std=gnu89 ... -Werror`) -> `bin/pruebas` | OK, sin errores ni warnings |
| Cátedra (`-Wall -Wextra -pedantic -Werror`) -> `bin/pruebas_cat` | OK, sin errores ni warnings |

## Pruebas
`./bin/pruebas`: todos los CP-001-L01 a L15 y CP-001-01 a 23 en `[OK]`.
`RESUMEN: 38 OK, 0 FALLA`

Sanitizer: no disponible (el GCC de MinGW no trae libasan/libubsan: `cannot find -lasan`). CP-001-25 se resolvió por revisión.

## Casos de revisión
- CP-001-24 OK: `crearEntrada` y `insPrinLista` chequean cada `malloc`; `insPrinLista` libera el nodo si falla `info`; `agregarLruS` libera el bloque temporal y devuelve `ERROR_MALLOC`; `obtenerLruS` chequea el `malloc` del temporal. La lista nunca queda con un nodo sin `info`.
- CP-001-25 OK (por revisión): `agregarLruS` libera el bloque en todos los caminos, `obtenerLruS` libera el temporal, `sacarPorClaveLista`, `sacarUltLista` y `vaciarLista` liberan `info` y nodo una sola vez. Sin fugas ni dobles liberaciones visibles.
- CP-001-26 OK: todas las funciones de `tda/lista.*`, `cache/entrada.*` y `cache/lru_simple.*` están en camelCase; sin snake_case.

## RF por RF
| RF | Casos | Resultado |
| --- | --- | --- |
| RF-01 | CP-001-01 | cumplido |
| RF-02 | CP-001-02, 03 | cumplido |
| RF-03 | CP-001-04 | cumplido |
| RF-04 | CP-001-05, 06, 22, 23 | cumplido |
| RF-05 | CP-001-07 a 10 | cumplido |
| RF-06 | CP-001-04, 06 | cumplido |
| RF-07 | CP-001-11, 12 | cumplido |
| RF-08 | CP-001-13 a 15 | cumplido |
| RF-09 | CP-001-16, 17 | cumplido |
| RF-10 | CP-001-18, 25 | cumplido |
| RF-11 | CP-001-19, 20 | cumplido |
| RF-12 | CP-001-21 | cumplido |
| RF-13 | CP-001-24 | cumplido |
| RNF-01 | CP-001-26 | cumplido |
| RNF-02 | CP-001-25 | cumplido (por revisión, sin sanitizer) |

## Criterios de finalización
- Todos los casos en OK (26 de caja negra y revisión, 15 de primitivas): SÍ.
- `/validar 001` APROBADO: SÍ. `/revisar 001`: no evaluado por este agente.
- Pruebas sin reportes del sanitizer: no verificable (no disponible); resuelto por revisión.
- Constitución: principio 1 cumple (dos configuraciones limpias); principios 4 y 5 sin hallazgos en la revisión.
- Pendientes de las personas (no bloquean este veredicto): `spec.md` sigue en `Estado: borrador` y las tareas de `tasks.md` están sin marcar.

## Cambios necesarios
Ninguno. Avisos:
1. El sanitizer no existe en este GCC (MinGW); conviene correr `bin/pruebas_asan` en Linux antes de la entrega.
2. `AGENTS.md`: en Git Bash de esta máquina `gcc` requiere agregar `C:\Program Files\CodeBlocks\MinGW\bin` al PATH.
