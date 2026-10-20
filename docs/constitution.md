# Constitución — TP LRU Cache

Principios innegociables. Toda spec, plan, tarea, revisión y validación debe cumplirlos.

1. **Compila limpio en dos configuraciones.** Sin errores ni warnings con los flags de la cátedra
   (`-Wall -Wextra -pedantic`) y con los estrictos del repo (`-std=gnu89 -Wall -Wextra
   -Wdeclaration-after-statement -Wpointer-arith -Wvla -Wlong-long -Werror`). `-std=c89` no se usa
   porque rechaza los comentarios `//` que pide la cátedra.
2. **Estilo de la cátedra.** Manda la skill `c-aed-unlam`: nombres `sNodo`/`tNodo`/`tXxx`/`pXxx`, llaves
   Allman, `if(` sin espacio, declaraciones al inicio del bloque, códigos de retorno simbólicos, nodo
   genérico `void *info` + `tamInfo`, puntero a puntero, todo en español y comentarios con `//`.
3. **La spec manda.** Nada se implementa si no está en la spec aprobada. Si falta una decisión, se para y
   se pregunta al grupo. Un cambio de requisitos va primero a `spec.md`, después a `plan.md` y
   `tasks.md`, y por último al código.
4. **Contratos y capas.** Cada módulo usa solo las primitivas de los `.h` que figuran en su columna "Usa"
   de `docs/arquitectura.md`. Nadie toca nodos de otra estructura. Firmas y retornos coinciden
   exactamente con `docs/primitivas.md`.
5. **Memoria sin fugas ni dobles liberaciones.** Todo `malloc` se chequea. Cada bloque tiene un único
   dueño que lo libera exactamente una vez: la lista es dueña de los datos; el árbol, solo de su índice.
6. **Pruebas como puerta.** Cada RF tiene al menos un caso en `casos-prueba.md`, y cada caso tiene el
   mismo ID en el código de pruebas. Una tarea no se marca hecha con casos en FALLA.
7. **El código lo escriben las personas.** Los agentes revisan specs y código, compilan, ejecutan pruebas
   y registran resultados. Nunca editan archivos `.c` ni `.h` ni escriben la solución: señalan el
   problema y la regla que incumple.
