---
description: SDD · Dónde está una spec y cuál es el siguiente paso (uso - /sdd-status 001)
agent: plan
---

Leé `specs/$1-*/` (`spec.md`, `plan.md`, `tasks.md`, `casos-prueba.md` y `validacion.md` si existe) y
`MEMORY.md`. Usá la skill `sdd`. Decime en pocas líneas:

1. Estado de la spec (borrador, aprobada, implementada) y dudas abiertas.
2. Tareas hechas y pendientes (x de y).
3. Casos de prueba en OK, FALLA, sin probar y pendientes.
4. El siguiente paso exacto y, si corresponde, el comando (`/revisar`, `/validar`, `/sdd-clarify`).

No modifiques ningún archivo.
