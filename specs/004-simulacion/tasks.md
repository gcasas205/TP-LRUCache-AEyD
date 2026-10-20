# Tareas — Spec 004

Las implementa la Persona 4, a mano y en orden. En cada tarea: los casos ya están en `pruebas_004.c` (los
escribe un agente y dan FALLA); después el código, y las dos compilaciones de `AGENTS.md` sin warnings.

- [ ] **T1. Congelar `posteo.h`, `usuario.h`, `generador.h`, `metricas.h`, `simulador.h` y `reporte.h`.** Todos los RF
  - Hecho cuando: el grupo los aprobó y compilan con stubs en las dos configuraciones.
- [ ] **T2. `posteo` y `usuario`: los dos modos de lectura, escribir, `cmpIdPosteo`.** RF-08, RF-18
  - Hecho cuando: CP-004-09, 10 y 27 en OK.
- [ ] **T3. `metricas`.** RF-10
  - Hecho cuando: CP-004-11 y 12 en OK.
- [ ] **T4. `generarPosteos` y `generarLote` con validación de parámetros.** RF-01, RF-03, RF-07
  - Hecho cuando: CP-004-01 y 07 en OK.
- [ ] **T5. Estrategias y `aleatorioEntre` (ordenado proporcional, en `double`).** RF-02 a RF-06
  - Hecho cuando: CP-004-02 a 06 y 28 en OK.
- [ ] **T6. `main_generador` con `argv`.** RF-02, RF-07
  - Hecho cuando: CP-004-08 en OK y genera los cuatro archivos de `datos/`.
- [ ] **T7. `simular` con "sin caché", calentamiento y lector por puntero a función.** RF-08 a RF-11, RF-15, RF-17
  - Hecho cuando: CP-004-13, 20, 21, 30 y 31 en OK.
- [ ] **T8. Simulación con la Parte 1 y la Parte 2, en los dos modos (cuando las specs 001 a 003 estén en OK).** RF-11, RF-12, RF-14, RF-18
  - Hecho cuando: CP-004-14 a 19 y 29 en OK.
- [ ] **T9. `reporte` y `main_simulador` con modo; correr todas las corridas en la máquina del informe.** RF-12, RF-13, RF-18, RNF-04
  - Hecho cuando: CP-004-23, 24, 32 y 33 en OK.
- [ ] **T10. Informe, revisión y validación.** RF-16, RNF-01 a RNF-05
  - Hecho cuando: CP-004-22 y 25 en OK, y `/revisar 004` y `/validar 004` dan `VEREDICTO: APROBADO`.
