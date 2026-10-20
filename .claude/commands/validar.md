---
description: SDD · Valida una spec RF por RF y registra los resultados
argument-hint: <NNN>
disable-model-invocation: true
context: fork
agent: validador
background: false
---

Validá la spec $0 (carpeta `specs/$0-*/`) siguiendo tus pasos: compilar en las dos configuraciones,
correr las pruebas y el sanitizer si existe, resolver los casos de comando y de revisión, actualizar la
columna Estado de `casos-prueba.md` y reescribir `validacion.md`.

No arregles nada ni toques código. Terminá con el veredicto.
