# Tareas — Spec 001

Las implementa la Persona 1, a mano y en orden. En cada tarea: los casos ya están en `pruebas_001.c` (los
escribe un agente y dan FALLA); después el código, y las dos compilaciones de `AGENTS.md` sin warnings.

- [x] **T1. Congelar `comun.h`, `lista.h`, `entrada.h` y `lru_simple.h` según `docs/primitivas.md`.** Todos los RF
  - Hecho cuando: el grupo los aprobó y compilan con stubs en las dos configuraciones.
- [x] **T2. `main_pruebas.c` (llama a `pruebas001` y a `imprimirResumen`; `verificar` y `pruebas_001.c` los entrega el agente).** —
  - Hecho cuando: `bin/pruebas` imprime `RESUMEN:` y sale con 1 mientras haya casos en FALLA.
- [x] **T3. Lista: crear, vacía, llena, insertar al principio, recorrer, vaciar.** —
  - Hecho cuando: CP-001-L01, L02, L13 y L15 en OK.
- [x] **T4. Lista: buscar y llevar al principio por dato parcial.** —
  - Hecho cuando: CP-001-L03 a L08 en OK.
- [x] **T5. Lista: ver primero, sacar por clave, sacar último.** —
  - Hecho cuando: CP-001-L09 a L12 en OK.
- [x] **T6. Módulo `entrada`.** RF-11, RF-12
  - Hecho cuando: CP-001-L14 en OK.
- [x] **T7. `crearLruS`, `agregarLruS` (nueva, desborde) y `obtenerLruS`.** RF-01 a RF-04, RF-06, RF-07, RF-12
  - Hecho cuando: CP-001-01 a 06, 11, 12, 21, 22 y 23 en OK.
- [x] **T8. Reemplazo de clave existente.** RF-05, RF-11
  - Hecho cuando: CP-001-07 a 10, 19 y 20 en OK.
- [x] **T9. `borrarLruS` y `vaciarLruS`.** RF-08, RF-09, RF-10
  - Hecho cuando: CP-001-13 a 18 en OK.
- [x] **T10. Revisión y validación.** RF-13, RNF-01, RNF-02
  - Hecho cuando: `/revisar 001` y `/validar 001` dan `VEREDICTO: APROBADO` (CP-001-24, 25 y 26 en OK).
