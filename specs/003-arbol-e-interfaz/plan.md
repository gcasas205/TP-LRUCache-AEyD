# Plan — Spec 003: Árbol índice e interfaz común

Parte de la spec 003 en borrador. Firmas y retornos exactos: `docs/primitivas.md` (sección Spec 003).

## Archivos y responsabilidades

| Archivo | Responsabilidad | Usa |
| --- | --- | --- |
| `tda/arbol.h` / `.c` | ABB genérico: insertar, buscar por dato parcial, eliminar, altura, cantidad, recorrido en orden, vaciar | `comun.h` |
| `cache/cache.h` / `.c` | `tCache`, creación de las tres variantes, despacho por punteros a función y adaptadores `static` | `lru_simple`, `lru_doble` |
| `pruebas/pruebas_003.c` | `int pruebas003(void)`: casos de `casos-prueba.md`; devuelve la cantidad de fallas | `arbol`, `cache` |
| `apps/main_pruebas.c` | Llama a `pruebas001()` … `pruebas004()`, imprime el total y devuelve 1 si hubo fallas | `pruebas/` |

Además, la Persona 3 entrega temprano el **stub de `arbol`** (vector estático de 1000 elementos con
búsqueda lineal y las mismas firmas) para que la Persona 2 no se trabe.

## Cómo resuelve cada operación (sin código)

- **Insertar y buscar:** bajan con puntero a puntero comparando con `cmp`; insertar engancha en el `NULL`
  donde terminó la bajada. Ambas comparten la función `static` `buscarNodoArbol`, que devuelve la
  dirección del puntero al nodo (o al `NULL` donde iría).
- **Eliminar:** con `buscarNodoArbol` obtiene el puntero al nodo. Hoja: libera y pone `NULL`. Un hijo:
  el puntero pasa a apuntar al hijo. Dos hijos: copia el `info` del menor del subárbol derecho
  (`menorNodoArbol`) en el nodo y elimina ese menor, que tiene a lo sumo un hijo.
- **Altura, cantidad, recorrido, vaciar:** recursivas, caso base árbol vacío. Vaciar en pos-orden.
- **Interfaz:** cada `crearCacheX` pide la estructura de la variante, la crea y carga sus cinco
  adaptadores (incluido `altura`) y su nombre. Cada operación de la interfaz solo llama al puntero correspondiente. "Sin
  caché" tiene adaptadores fijos que no guardan nada.

## Decisiones (y alternativa descartada)

| Decisión | Por qué | Alternativa descartada |
| --- | --- | --- |
| Insertar y buscar iterativos | Se llaman millones de veces en la simulación y el árbol puede degenerar (RF-07) | Recursivos: más simples de escribir, pero con 5000 niveles cada llamada apila 5000 marcos |
| Altura, cantidad y vaciar recursivos | Son la forma de la cátedra y se llaman pocas veces; 5000 niveles entran en la pila | Iterativos con una pila propia: más código sin beneficio |
| Dos hijos: menor del subárbol derecho | Regla fija, así los casos de prueba tienen un resultado único | Mayor del izquierdo: igual de válido, pero hay que elegir uno |
| Despacho con punteros a función en `tCache` | El simulador no tiene ningún `if` por variante (RF-09, RNF-02) | Un `switch` sobre un campo "tipo" en cada operación: cada variante nueva toca todas las funciones |
| "Sin caché" como una variante más | El simulador mide la línea base con el mismo código | Un `if(usarCache)` en el simulador |

## Estrategia de pruebas

- Árbol primero (01 a 16), con el árbol T7 como caso base y el degenerado de 5000 para RF-07.
- La interfaz (17 a 25) reusa secuencias de la spec 001, así se ve que la interfaz no cambia resultados.
- CP-003-24 deja escrita una función de prueba sobre `tCache*` que las specs 001 y 002 pueden reusar.

## Trazabilidad

| RF | Dónde se resuelve | Casos |
| --- | --- | --- |
| RF-01, RF-02 | `insArbol` + `buscarNodoArbol` | 02, 03 |
| RF-03 | `buscarArbol` | 04, 05 |
| RF-04, RF-05 | `eliminarArbol` + `menorNodoArbol` | 06 a 11 |
| RF-06 | `alturaArbol`, `cantNodosArbol` | 01, 02, 10, 12 |
| RF-07 | Insertar y buscar iterativos | 13 |
| RF-08 | `vaciarArbol` | 14, 15 |
| RF-09 | `tCache` + adaptadores | 18, 19, 24, 25 |
| RF-10 | `crearSinCache` | 17 |
| RF-11 | Adaptadores que devuelven lo de abajo | 18, 19 |
| RF-12 | `nombreCache`, `alturaIndiceCache` | 17, 20, 21 |
| RF-13 | Limpieza en `crearCacheX` | 22 |
| RF-14 | `vaciarCache` | 23 |
