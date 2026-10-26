# MEMORY.md — TP LRU Cache

Estado del proyecto y decisiones que afectan a todo el repo. Máximo ~50 líneas: resumir o borrar lo que
ya no aporte. Las decisiones de un solo módulo van en el `plan.md` de su spec.

## Estado actual (25/10/2026)
- **Spec 001 implementada y validada** (`/revisar` y `/validar` APROBADO, 41 casos en OK). Falta que el
  grupo apruebe la spec y los dos cambios de contrato de hoy (`listaLlena`/`LISTA_*` y la precarga de
  `obtenerLruS`), pasarlos al doc compartido y marcar `tasks.md`.
- Specs 002 a 004 en `Estado: borrador`, sin código: sus `.c`/`.h` existen vacíos. Primer paso de cada
  una: su T1 (congelar los `.h` según `docs/primitivas.md`, con stubs).
- Code::Blocks: `TP-LRUCache.workspace` y `pruebas.cbp`, `generador.cbp`, `simulador.cbp` en la raíz.

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
- **Estados de lista en `comun.h` (25/10/2026):** `listaVacia`, `listaLlena` (primitiva de la cátedra)
  y `listaVaciaD` devuelven `LISTA_VACIA` 8 / `LISTA_LLENA` 9 / `LISTA_DISPONIBLE` 0, no 1/0 (pedido de
  los docentes). Son solo respuestas de esas preguntas: las demás primitivas siguen devolviendo `VACIA`,
  `ERROR_MALLOC`, etc. `insPrinLista` consulta `listaLlena` (dos `malloc` extra por inserción: tenerlo en
  cuenta al medir en la spec 004).
- **Compilación de pruebas con los `.c` listados uno por uno (25/10/2026):** un `.c` vacío rompe la
  configuración de la cátedra. Cada spec suma sus `.c` a las tres líneas de `pruebas` de `AGENTS.md`.
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
- Code::Blocks genera las firmas como `tLista* pLista`; la cátedra pide `tLista *pLista` (lo marca
  `/revisar`). Los casts sí van pegados: `(tNodo*)`.
- En `listaLlena` el truco "liberar y preguntar si quedó `NULL`" vale solo con dos variables (nodo y
  bloque): `nodo->info` después del `free` es leer memoria liberada.
- MinGW no trae sanitizer y su `gcc` no está en el PATH de Git Bash. Después de cambiar `AGENTS.md`, salir
  y volver a abrir Claude Code: la sesión vieja sigue con los comandos anteriores.
