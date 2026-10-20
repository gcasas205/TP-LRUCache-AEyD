# Tareas — Spec 002

Las implementa la Persona 2, a mano y en orden. En cada tarea: los casos ya están en `pruebas_002.c` (los
escribe un agente y dan FALLA); después el código, y las dos compilaciones de `AGENTS.md` sin warnings.

- [ ] **T1. Congelar `lista_doble.h` y `lru_doble.h` según `docs/primitivas.md`.** Todos los RF
  - Hecho cuando: el grupo los aprobó y compilan con stubs en las dos configuraciones.
- [ ] **T2. Lista doble: crear, vacía, insertar al principio, ver y sacar el último, recorrer, vaciar.** RF-18
  - Hecho cuando: CP-002-D01, D02, D03 y D10 en OK.
- [ ] **T3. Lista doble: mover al principio y sacar un nodo dado.** RF-18
  - Hecho cuando: CP-002-D04 a D08 en OK.
- [ ] **T4. Lista doble: ver y actualizar el `info` de una posición.** RF-05
  - Hecho cuando: CP-002-D09 en OK.
- [ ] **T5. Stub de `arbol` (vector + búsqueda lineal) si la spec 003 no está lista.** —
  - Hecho cuando: `lru_doble` compila y enlaza contra él.
- [ ] **T6. `crearLruD`, `agregarLruD` (clave nueva y desborde), `obtenerLruD`, `alturaIndiceLruD`, `verificarLruD`.** RF-01 a RF-04, RF-06, RF-07, RF-14, RF-15
  - Hecho cuando: CP-002-01 a 06, 11, 12, 22 y 23 en OK (la altura se prueba en T9: el stub no la calcula).
- [ ] **T7. Reemplazo de clave existente.** RF-05, RF-15
  - Hecho cuando: CP-002-07 a 10, 30 y 31 en OK.
- [ ] **T8. `borrarLruD` y `vaciarLruD`.** RF-08 a RF-10, RF-16
  - Hecho cuando: CP-002-13 a 18 y 32 en OK.
- [ ] **T9. Cambiar el stub por el árbol real de la spec 003 y correr todo.** RF-11, RF-12, RF-14
  - Hecho cuando: todos los casos de prueba de 002 en OK con el árbol real, incluidos 19 a 21 y 26 a 29.
- [ ] **T10. Revisión y validación.** RF-13, RF-16, RF-17, RNF-01, RNF-02
  - Hecho cuando: `/revisar 002` y `/validar 002` dan `VEREDICTO: APROBADO` (CP-002-24, 25, 33 y 34 en OK).
