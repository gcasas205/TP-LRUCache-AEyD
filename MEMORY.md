# MEMORY.md — TP LRU Cache

Estado del proyecto y decisiones que afectan a todo el repo. Máximo ~50 líneas: resumir o borrar lo que
ya no aporte. Las decisiones de un solo módulo van en el `plan.md` de su spec.

## Estado actual (20/10/2026)
- Specs 001 a 004 en `Estado: borrador`, con plan, tareas y casos de prueba. Falta aprobarlas en grupo.
- Spec 004 ampliada: muchos datos, calentamiento, modo de acceso con seek simulado (pedido de la cátedra).
- Sin código todavía. Primer paso: congelar los `.h` según `docs/primitivas.md`.

## Decisiones (y por qué)
- **Primitivas en camelCase con sufijo (`crearLruS`, `agregarCache`):** el docente lo aceptó (20/10/2026)
  con la condición de no mezclar con snake_case. Los nombres de módulos y archivos (`lru_simple.c`) no
  cuentan como primitivas.
- **Flags `-std=gnu89` + chequeos explícitos, no `-std=c89`:** `-std=c89` da error con los comentarios
  `//` que pide la cátedra. La combinación de la constitución acepta `//` y sigue rechazando
  declaraciones mezcladas, `for(int ...)`, VLA, `long long` y aritmética sobre `void*`.
- **Un solo ejecutable con interfaz común (`tCache`):** las dos cachés se miden con el mismo simulador.
  Por eso las primitivas llevan sufijo `LruS` / `LruD`.
- **Entrada `[clave][valor]` con la clave primero:** el `cmp` del usuario sirve directo como búsqueda por
  dato parcial en la lista y en el árbol.
- **La lista es dueña de los datos; el árbol, solo de su índice** (copia de la clave + posición). Cada
  `free` tiene un único responsable.
- **Lista doble con `pri` y `ult`** en vez del puntero flotante del apunte: la caché necesita los dos
  extremos sin recorrer.
- **Pruebas en `pruebas/`, un archivo por spec:** cada integrante escribe las suyas sin conflictos.
  `main_pruebas` devuelve 1 si hubo fallas, para que el validador lo detecte.
- **Muchos datos (propuesta 50000 usuarios = un millón de pedidos):** con 200 usuarios una corrida dura
  milisegundos y `clock()` de Windows mide de a 1 ms. Regla: la corrida sin caché dura ≥ 1 s.
- **Dos modos de acceso (directo y con seek simulado), como `tLectorPosteo`:** la cátedra recomendó mover
  el puntero al final antes de leer; duplica el costo por lectura. Se reportan los dos y el informe lo
  declara. **Calentamiento** del archivo antes de medir cada corrida.
- **Lote ordenado proporcional** (`⌊j × 50000 / pedidos⌋`): el salto aleatorio no funciona con muchos
  usuarios.
- **Dos herramientas, una fuente:** skills en `.claude/skills/` (las leen Claude Code y OpenCode);
  agentes y comandos duplicados en `.claude/` y `.opencode/`; `CLAUDE.md` solo importa `AGENTS.md`.
- **Agentes solo revisan y validan:** el TP es para aprender; el código lo escribe cada uno.
- **`listaLlena` en el contrato de la lista (25/10/2026):** es primitiva de la cátedra.
  `listaVacia`, `listaLlena` y `listaVaciaD` devuelven `LISTA_VACIA` / `LISTA_LLENA` / `LISTA_DISPONIBLE`
  (8 / 9 / 0, en `comun.h`), no 1/0: pedido de los docentes. `insPrinLista` la consulta antes de reservar (decisión del grupo: cuesta dos `malloc`
  extra por inserción; tenerlo en cuenta al medir en la spec 004). Caso CP-001-L15.
- **Códigos de retorno en `tda/comun.h`:** la cátedra suele llamarlo `retornos.h`; lo que se corrige es
  usar los nombres simbólicos.

## Dudas abiertas del grupo
- [ ] Qué hace el simulador con un id fuera de rango en un feed (spec 004).
- [ ] Capacidades definitivas, G del concentrado y cantidad de usuarios medida en la máquina del informe
  (spec 004).

## Aprendizajes y errores a evitar
- En MinGW `RAND_MAX` es 32767: un solo `rand()` nunca genera ids mayores; combinar dos.
- En MinGW `long` tiene 32 bits y C89 no tiene `long long`: cuentas grandes (como `j × 50000`) en `double`.
- El archivo de posteos (7,4 MB) queda en la memoria del sistema operativo: "leer del disco" es una
  llamada al sistema (~1 µs), no un acceso físico. Decirlo en el informe.
- El valor de una entrada se copia con `memcpy` desde `(char*)info + tamClave`; nunca castear ese
  puntero a un struct (alineación).
