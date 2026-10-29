# Validación — Spec 002

Fecha: 29/10/2026 · Commit: 23ff980 · Validador: agente validador

VEREDICTO: APROBADO

## Compilación
| Configuración | Resultado |
| --- | --- |
| Estricta del repo (`-std=gnu89 ... -Werror`) → `bin/pruebas` | OK, sin warnings |
| Cátedra (`-Wall -Wextra -pedantic -Werror`) → `bin/pruebas_cat` | OK, sin warnings |
| Sanitizer (`-fsanitize=address,undefined`) | No disponible: el GCC de MinGW no tiene `libasan`/`libubsan` (`cannot find -lasan`) |

Nota: `gcc` no estaba en el PATH de Git Bash; se agregó `C:\Program Files\CodeBlocks\MinGW\bin` como indica `AGENTS.md`. Se borraron los binarios viejos de `bin/` antes de compilar para no correr uno desactualizado.

## Pruebas
`RESUMEN: 78 OK, 0 FALLA` (incluye los 40 casos `CP-002-*` de prueba: 01 a 23, 26 a 32 y D01 a D10, todos `[OK]`).

Sanitizer: no disponible. CP-002-25 y CP-002-33 se resolvieron por revisión de `cache/lru_doble.c` y `tda/arbol.c`:
- `agregarLruD` libera `bloque` e `indice` en todos los caminos de salida.
- `obtenerLruD` libera `indice` y `temp`.
- `borrarLruD` libera `indice`.
- `vaciarLruD` llama a `vaciarArbol` (libera solo `info` y nodos del árbol) y a `vaciarListaD` (libera nodos y datos).
- `eliminarArbol` en el caso de dos hijos traspasa el puntero `info` del menor y libera el nodo sin liberarlo dos veces.

Revisión:
- CP-002-24: `agregarLruD` chequea cada `malloc`. Si falla `insArbol`, saca el nodo recién insertado con `sacarNodoListaD`. `obtenerLruD` pide el buffer antes de mover el nodo.
- CP-002-34: `obtenerLruD`, `agregarLruD` y `borrarLruD` buscan con `buscarArbol`. El descarte usa `verUltListaD` y `sacarUltListaD`. No recorren la lista doble.

## RF por RF
| RF | Casos | Resultado |
| --- | --- | --- |
| RF-01 a RF-12 | CP-002-01 a 23 | Cumplidos |
| RF-13 | CP-002-24 | Cumplido |
| RF-14 | CP-002-26 a 29 | Cumplido |
| RF-15 | CP-002-01 a 23, 29 a 32 | Cumplido |
| RF-16 | CP-002-25, 33 | Cumplido (por revisión, sin sanitizer) |
| RF-17 | CP-002-34 | Cumplido |
| RF-18 | CP-002-34, D01 a D10 | Cumplido |

## Criterios de finalización
- Todos los casos de `casos-prueba.md` en OK: cumplido.
- Pruebas con el sanitizer sin reportes: no verificable (no disponible, resuelto por revisión).
- `/revisar 002` con APROBADO: no hay registro en el repo; lo decide el grupo.
- Pendientes de las personas: `tasks.md` tiene T1 a T10 sin marcar, y `spec.md` figura como `Estado: borrador`.
- Constitución: principios 1 (dos compilaciones limpias), 5 y 6 verificados. El 7 se respetó: no se tocó código.

## Cambios necesarios
1. `specs/002-lru-doble/tasks.md` — las tareas T1 a T10 siguen sin marcar aunque sus casos están en OK. Las marca la persona.
2. `specs/002-lru-doble/spec.md:3` — el estado dice `borrador`. El grupo debe pasarlo a aprobada/implementada.
3. Observación, sin incumplir ningún caso: en `cache/lru_doble.c`, `agregarLruD` descarta el menos reciente antes de insertar. Si después falla `insArbol` o `insPrinListaD` por falta de memoria, el elemento descartado se pierde (la caché queda consistente, con un elemento menos). La spec no lo prohíbe; el grupo puede decidir si le importa.
