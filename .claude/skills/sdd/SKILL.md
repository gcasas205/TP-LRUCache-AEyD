---
name: sdd
description: Úsala siempre que trabajes con Spec-Driven Development en el TP LRU Cache (docs/constitution.md o cualquier archivo dentro de specs/) - revisar o cambiar specs, planes, tareas y casos de prueba, o revisar y validar el código de una spec. En este proyecto el código lo escriben las personas.
---

# Spec-Driven Development — TP LRU Cache

## Flujo
Constitución → Spec → Clarificación → Plan → Tareas → Casos de prueba → Implementación (persona) →
Revisión → Validación → Cambio.

- La spec manda: si algo no está en la spec, no se implementa. Si falta una decisión, se para y se
  pregunta al grupo.
- Un cambio de requisitos va primero a `spec.md`, después a `plan.md`, `tasks.md` y `casos-prueba.md`, y
  por último al código.
- Cada spec vive en `specs/NNN-nombre/` con `spec.md`, `plan.md`, `tasks.md`, `casos-prueba.md` y, cuando
  se valida, `validacion.md`.
- **Los agentes nunca escriben ni editan `.c` o `.h`** (constitución, principio 7). Señalan el problema y
  la regla; la persona lo arregla.

## Spec (`spec.md`)
```
# Spec NNN — <Nombre>

Estado: borrador | aprobada | implementada
Dueño: Persona N

## Contexto y objetivo
## Usuarios
## Historias de usuario
- HU-1. Como <rol>, quiero <acción> para <beneficio>.
## Definiciones
## Requisitos funcionales
## Requisitos no funcionales
## Casos límite
## Fuera de alcance
## Criterios de finalización
## Dudas abiertas
- [NECESITA ACLARACIÓN] <duda>
```
La spec describe el QUÉ y el POR QUÉ, sin archivos ni estructuras. Su fuente es `docs/enunciado.md`: si
la contradice, manda el enunciado.

### Requisitos en EARS (en español)
- RF-xx: CUANDO <evento>, EL SISTEMA <respuesta>.
- RF-xx: SI <condición no deseada>, ENTONCES EL SISTEMA <respuesta>.
- RF-xx: MIENTRAS <estado>, EL SISTEMA <respuesta>.
- RF-xx: EL SISTEMA <comportamiento permanente>.

Cada RF debe ser verificable con un caso de prueba concreto: nada de "rápido" o "correcto" sin un
criterio medible.

## Plan (`plan.md`)
Archivos y responsabilidades (con su columna "Usa" de `docs/arquitectura.md`) · cómo resuelve cada
operación, sin código · decisiones con su alternativa descartada · estrategia de pruebas · tabla de
trazabilidad RF → dónde se resuelve → casos. Las firmas exactas viven en `docs/primitivas.md`.

## Tareas (`tasks.md`)
```
- [ ] **Tn. <Descripción>.** RF-x, RF-y
  - Hecho cuando: CP-NNN-aa a bb en OK.
```
20-30 minutos por tarea, en orden de dependencia, máximo 10. Las marca la persona, nunca un agente.

## Casos de prueba (`casos-prueba.md`)
- ID `CP-NNN-xx` (caja negra) o `CP-NNN-Lxx` / `Dxx` (primitivas internas). El mismo ID aparece en
  `pruebas/pruebas_NNN.c`.
- Columnas: ID · Cubre (RF) · Método · Preparación · Acción · Esperado · Estado.
- Métodos: **prueba** (lo verifica el programa de pruebas), **comando** (lo corre el validador en la
  terminal), **revisión** (leyendo el código, el informe o con el sanitizer).
- Estado: `pendiente`, `OK dd/mm`, `FALLA dd/mm`, `sin probar dd/mm` (el ID no apareció en la salida) o
  `bloqueado por spec NNN`.
- Valores distintos entre sí, para que un dato perdido o cruzado se note. Todo RF tiene al menos un caso.

Salida del programa de pruebas, una línea por caso:
```
[OK] CP-001-05 desborde descarta el menos reciente
[FALLA] CP-001-07 reemplazo deja la clave primera (esperado "Z", obtenido "A")
RESUMEN: 41 OK, 1 FALLA
```

## Implementación (la hace la persona)
Una tarea por vez: primero los casos de la tarea en `pruebas_NNN.c` (en FALLA), después el código hasta
que pasen, las dos compilaciones de `AGENTS.md` sin warnings, y recién ahí se marca la tarea.

## Revisión (`/revisar NNN [archivo]`, agente `revisor`)
Lee y compila; no modifica nada. Devuelve `VEREDICTO: APROBADO` o `VEREDICTO: CAMBIOS NECESARIOS` y una
lista `archivo:línea — regla — qué pasa — qué se espera`, sin escribir el código corregido.

## Validación (`/validar NNN`, agente `validador`)
Compila en las dos configuraciones, corre las pruebas (y el sanitizer si existe), resuelve los casos de
comando y de revisión, actualiza la columna Estado de `casos-prueba.md` y reescribe `validacion.md`:

```
# Validación — Spec NNN

Fecha: dd/mm/aaaa · Commit: <hash corto> · Validador: agente validador

VEREDICTO: APROBADO | CAMBIOS NECESARIOS

## Compilación
| Configuración | Resultado |
## Pruebas
<línea RESUMEN y sanitizer>
## RF por RF
| RF | Casos | Resultado |
## Criterios de finalización
## Cambios necesarios
1. archivo:línea — qué incumple (RF, caso o principio) — qué se espera
```
Un RF está cumplido solo si todos sus casos están en OK. El historial de validaciones queda en git.

## Cambio
Sin tocar código: RF nuevo o modificado en EARS en `spec.md`, sus casos límite y casos de prueba, y qué
partes de `plan.md` y `tasks.md` cambian. Se muestra el diff y el grupo lo aprueba.

## Estado
Sin modificar archivos: fase de la spec, tareas hechas y pendientes (x de y), casos en OK / FALLA /
pendientes, y el siguiente paso exacto.
