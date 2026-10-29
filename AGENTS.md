# AGENTS.md — TP LRU Cache (AyED, UNLaM)

TP grupal en C: TDA LRU Cache genérico con lista simple (Parte 1), lista doble + ABB índice (Parte 2) y
simulación de la red social "Tweener" (Parte 3). El programa lo escriben las personas; los agentes
escriben las pruebas (`pruebas/`), revisan specs y código y validan las specs. Nunca escriben la solución.

## Antes de cualquier tarea
- Leé `docs/constitution.md`, `MEMORY.md` y la spec activa (`specs/NNN-*/`).
- Para código C seguí la skill `c-aed-unlam`; para el flujo, la skill `sdd` (las dos en `.claude/skills/`,
  que leen Claude Code y OpenCode).
- Agentes `revisor` y `validador`: `.claude/agents/` (Claude Code) y `.opencode/agents/` (OpenCode), con
  las mismas instrucciones. Si cambia uno, se actualiza el otro.

## Estructura
- `docs/`: enunciado, constitución, arquitectura (capas y columna "Usa"), primitivas (contratos exactos).
- `specs/NNN-nombre/`: `spec.md`, `plan.md`, `tasks.md`, `casos-prueba.md` y `validacion.md`.
- `tda/`, `cache/`, `dominio/`, `sim/`: módulos `.h`/`.c`. `pruebas/`: un archivo por spec. `apps/`: los mains.
- Raíz: `TP-LRUCache.workspace` y los proyectos `pruebas.cbp`, `generador.cbp` y `simulador.cbp` de
  CodeBlocks (flags de la cátedra). Un `.c` nuevo se agrega a mano a los proyectos que lo usan.

## Comandos (desde la raíz; en Windows, desde Git Bash o MSYS2)
Literales, sin variables de shell, para que coincidan con los permisos de `.claude/settings.json`.
```
mkdir -p bin
gcc -std=gnu89 -Wall -Wextra -Wdeclaration-after-statement -Wpointer-arith -Wvla -Wlong-long -Werror -Itda -Icache -Idominio -Isim -Ipruebas tda/lista.c tda/lista_doble.c tda/arbol.c cache/entrada.c cache/lru_simple.c cache/lru_doble.c pruebas/verificar.c pruebas/pruebas_001.c pruebas/pruebas_002.c apps/main_pruebas.c -o bin/pruebas
./bin/pruebas
gcc -Wall -Wextra -pedantic -Werror -Itda -Icache -Idominio -Isim -Ipruebas tda/lista.c tda/lista_doble.c tda/arbol.c cache/entrada.c cache/lru_simple.c cache/lru_doble.c pruebas/verificar.c pruebas/pruebas_001.c pruebas/pruebas_002.c apps/main_pruebas.c -o bin/pruebas_cat
gcc -std=gnu89 -Wall -Wextra -Wdeclaration-after-statement -Wpointer-arith -Wvla -Wlong-long -Werror -g -fsanitize=address,undefined -Itda -Icache -Idominio -Isim -Ipruebas tda/lista.c tda/lista_doble.c tda/arbol.c cache/entrada.c cache/lru_simple.c cache/lru_doble.c pruebas/verificar.c pruebas/pruebas_001.c pruebas/pruebas_002.c apps/main_pruebas.c -o bin/pruebas_asan
./bin/pruebas_asan
gcc -std=gnu89 -Wall -Wextra -Wdeclaration-after-statement -Wpointer-arith -Wvla -Wlong-long -Werror -Itda -Icache -Idominio -Isim -Ipruebas tda/*.c cache/*.c dominio/*.c sim/*.c apps/main_generador.c -o bin/generador
gcc -std=gnu89 -Wall -Wextra -Wdeclaration-after-statement -Wpointer-arith -Wvla -Wlong-long -Werror -Itda -Icache -Idominio -Isim -Ipruebas tda/*.c cache/*.c dominio/*.c sim/*.c apps/main_simulador.c -o bin/simulador
```
La primera línea `gcc` es la configuración estricta del repo; la segunda, la de la cátedra. Las tres de `pruebas`
listan los `.c` uno por uno, sin comodines: un `.c` vacío rompe la de la cátedra (`-pedantic -Werror`,
"empty translation unit"). Cada spec agrega sus `.c` (y su `pruebas_NNN.c`) a las tres líneas cuando
tienen código. En Windows, `gcc` es el de CodeBlocks (`C:\Program Files\CodeBlocks\MinGW\bin`): si Git
Bash no lo encuentra, agregar esa carpeta al PATH. El sanitizer solo existe en GCC de Linux (no en
MinGW); si no está, se informa y se sigue. Si cambian estos comandos, hay que reiniciar la sesión de
Claude Code u OpenCode para que los agentes los tomen.

## Formato de las pruebas
- Cada caso imprime `[OK] CP-NNN-xx <descripción>` o `[FALLA] CP-NNN-xx <descripción> (esperado ..., obtenido ...)`.
- Al final, `RESUMEN: <n> OK, <m> FALLA`. `bin/pruebas` sale con 1 si hubo alguna FALLA.
- El ID del caso es el mismo que en `specs/NNN-*/casos-prueba.md`.

## Límites
- ✅ Siempre: compilar en las dos configuraciones antes de opinar sobre código, y citar en cada
  observación la regla que se incumple (principio, sección de la skill o primitiva).
- ⚠ Preguntá antes: cualquier cambio en `spec.md`, `plan.md` o `docs/` (lo aprueba el grupo).
- ✅ Permitido: escribir y corregir `pruebas/*.c` y `pruebas/verificar.h`, siempre fieles a
  `casos-prueba.md` (mismo ID y mismos valores) y a `docs/primitivas.md`.
- 🚫 Nunca: editar `.c` o `.h` de `tda/`, `cache/`, `dominio/`, `sim/` o `apps/`, escribir la solución,
  marcar casillas de `tasks.md`.

## Memoria
- `MEMORY.md` guarda el estado y las decisiones que afectan a todo el proyecto. Lo mantienen las
  personas; un agente propone cambios, no lo edita.
