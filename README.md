# TP LRU Cache — AyED UNLaM 2C 2026

TDA LRU Cache genérico en C (estilo cátedra, base C89) en dos versiones, y una simulación que mide si la
caché acelera el feed de una red social. Entrega y defensa: **02/11/2026, 19:00**.

## Cómo trabajamos: Spec-Driven Development con implementación manual

Cada integrante es dueño de una spec. El flujo de cada una:

1. **Spec** (`spec.md`): qué tiene que hacer y por qué, con requisitos en EARS. Se aprueba en grupo.
2. **Clarificación**: `/sdd-clarify NNN` revisa la spec como QA, sin proponer soluciones.
3. **Plan** (`plan.md`): módulos, decisiones con su alternativa descartada, qué RF cubre cada parte.
4. **Tareas** (`tasks.md`): pasos de 20-30 minutos con su "Hecho cuando".
5. **Implementación**: **la escribe la persona, a mano**. Primero las pruebas de la tarea (en FALLA),
   después el código.
6. **Revisión**: `/revisar NNN [archivo]` revisa el código contra la skill de la cátedra, la constitución
   y los contratos. Señala el problema y la regla; no da la solución.
7. **Validación**: `/validar NNN` compila, corre las pruebas, recorre la spec RF por RF, actualiza el
   estado en `casos-prueba.md` y deja el veredicto en `validacion.md`.

Un cambio de requisitos va primero a la spec, después al plan y las tareas, y por último al código.

## Specs y dueños

| Spec | Qué cubre | Dueño |
| --- | --- | --- |
| [`001-lru-simple`](specs/001-lru-simple/spec.md) | Parte 1: lista simple, entrada y LRU sobre lista simple | Persona 1 |
| [`002-lru-doble`](specs/002-lru-doble/spec.md) | Parte 2: lista doble y LRU con índice | Persona 2 |
| [`003-arbol-e-interfaz`](specs/003-arbol-e-interfaz/spec.md) | ABB índice e interfaz común de caché | Persona 3 |
| [`004-simulacion`](specs/004-simulacion/spec.md) | Parte 3: generador, simulador, reporte e informe | Persona 4 |

## Qué hay en el repo

| Ruta | Para qué |
| --- | --- |
| `docs/enunciado.md` | El enunciado de la cátedra: la fuente de las specs |
| `docs/constitution.md` | 7 principios innegociables |
| `docs/arquitectura.md` | Capas, dependencias permitidas, decisiones, reparto y defensa |
| `docs/primitivas.md` | Firma, comportamiento, retorno y stub de cada primitiva |
| `specs/` | Una carpeta por spec |
| `AGENTS.md`, `MEMORY.md` | Instrucciones para los agentes y memoria del proyecto |
| `CLAUDE.md` | Solo importa `AGENTS.md`, para que Claude Code lo cargue siempre |
| `.claude/skills/` | `c-aed-unlam` (estilo de la cátedra) y `sdd` (el flujo). Las leen Claude Code y OpenCode |
| `.claude/agents/`, `.claude/commands/`, `.claude/settings.json` | Agentes, comandos y permisos para Claude Code |
| `.opencode/` | Los mismos agentes y comandos para OpenCode |

El doc compartido "TP LRU Cache — Arquitectura y reparto" sigue siendo la referencia para leer en grupo;
`docs/arquitectura.md` y `docs/primitivas.md` son su copia para los agentes. Si cambia uno, se actualiza
el otro.

## Compilar y probar

Los comandos exactos están en [`AGENTS.md`](AGENTS.md), sección "Comandos".
En CodeBlocks: un workspace con tres proyectos (generador, simulador, pruebas) que comparten `tda/`,
`cache/`, `dominio/` y `sim/`, con `-Wall -Wextra -pedantic` en las opciones del compilador.

## Agentes

Hay dos versiones con las mismas instrucciones: `.claude/` para Claude Code (incluido en Claude Pro y
Max) y `.opencode/` para OpenCode. Cada integrante usa la herramienta que tenga. Si se cambia un agente o
un comando, se actualizan las dos versiones.

**En Claude Code:** abrir la terminal en la raíz del repo y ejecutar `claude`. Los comandos son
`/revisar 001`, `/validar 001`, `/sdd-clarify 001` y `/sdd-status 001`; también se puede pedir "usá el
agente revisor para…". Los agentes usan Sonnet para gastar menos del límite del plan (se cambia en el
campo `model` de `.claude/agents/*.md`). `.claude/settings.json` bloquea editar `.c` y `.h` en todo el
proyecto, también en la conversación principal: es el principio 7 de la constitución. No hay que crear
otro `CLAUDE.md` con contenido propio: las instrucciones viven en `AGENTS.md`.

**En OpenCode:** los mismos comandos, o `@revisor` / `@validador`.

Ninguno de los dos agentes puede editar código:

- **revisor**: lee y compila. Devuelve `VEREDICTO: APROBADO` o `VEREDICTO: CAMBIOS NECESARIOS` con una
  lista `archivo:línea · regla · qué se espera`.
- **validador**: compila en las dos configuraciones, corre las pruebas (y el sanitizer si existe) y solo
  puede escribir `casos-prueba.md` (columna Estado) y `validacion.md` de la spec.
