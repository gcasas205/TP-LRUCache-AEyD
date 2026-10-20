# Tareas — Spec 003

Las implementa la Persona 3, a mano y en orden. En cada tarea: los casos ya están en `pruebas_003.c` (los
escribe un agente y dan FALLA); después el código, y las dos compilaciones de `AGENTS.md` sin warnings.

- [ ] **T1. Congelar `arbol.h` y `cache.h` según `docs/primitivas.md`.** Todos los RF
  - Hecho cuando: el grupo los aprobó y compilan con stubs en las dos configuraciones.
- [ ] **T2. Stub de `arbol` para la Persona 2 (vector de 1000 + búsqueda lineal).** —
  - Hecho cuando: está en el repo y `lru_doble` enlaza contra él.
- [ ] **T3. Crear, insertar (iterativo), altura, cantidad, recorrido en orden.** RF-01, RF-02, RF-06
  - Hecho cuando: CP-003-01, 02, 03 y 12 en OK.
- [ ] **T4. Buscar por dato parcial (iterativo).** RF-03
  - Hecho cuando: CP-003-04 y 05 en OK.
- [ ] **T5. Eliminar: hoja y un hijo.** RF-04, RF-05
  - Hecho cuando: CP-003-06, 07, 10 y 11 en OK.
- [ ] **T6. Eliminar: dos hijos y raíz.** RF-04
  - Hecho cuando: CP-003-08 y 09 en OK.
- [ ] **T7. Vaciar y árbol degenerado.** RF-07, RF-08
  - Hecho cuando: CP-003-13, 14 y 15 en OK.
- [ ] **T8. Interfaz: `crearSinCache`, `crearCacheSimple`, operaciones y adaptadores de la Parte 1.** RF-09 a RF-12
  - Hecho cuando: CP-003-17, 20 y la parte "simple" de 18, 19 y 21 en OK.
- [ ] **T9. Interfaz: `crearCacheDoble`, errores de creación y `vaciarCache`.** RF-11 a RF-14
  - Hecho cuando: CP-003-18 a 24 completos en OK (necesita la spec 002 con el árbol real).
- [ ] **T10. `main_pruebas.c` final, revisión y validación.** RNF-01, RNF-02
  - Hecho cuando: `bin/pruebas` corre las cuatro specs, y `/revisar 003` y `/validar 003` dan
    `VEREDICTO: APROBADO` (CP-003-16 y 25 en OK).
