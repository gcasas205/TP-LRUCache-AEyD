# Plan — Spec 002: LRU Cache con lista doble e índice

Parte de la spec 002 en borrador. Firmas y retornos exactos: `docs/primitivas.md` (sección Spec 002). La
tabla completa de qué hace cada primitiva sobre cada estructura está en `docs/arquitectura.md`, sección
"Parte 2: consistencia y memoria".

## Archivos y responsabilidades

| Archivo | Responsabilidad | Usa |
| --- | --- | --- |
| `tda/lista_doble.h` / `.c` | Lista doble con `pri` y `ult`; expone la posición de un nodo como `void*` opaco | `comun.h` |
| `cache/lru_doble.h` / `.c` | `tLruD`, sus cinco primitivas, `alturaIndiceLruD` y `verificarLruD` | `lista_doble`, `arbol`, `entrada` |
| `pruebas/pruebas_002.c` | `int pruebas002(void)`: casos de `casos-prueba.md`; devuelve la cantidad de fallas | `lru_doble`, `lista_doble` |

Mientras el árbol real (spec 003) no exista, `lru_doble` compila contra `arbol.h` con el stub de vector
y búsqueda lineal descrito en `docs/primitivas.md`.

## Cómo resuelve cada operación (sin código)

- **obtener:** busca `[clave]` en el árbol; si está, saca la posición del `info` del árbol, mueve ese
  nodo al principio de la lista y copia el valor desde el nodo de la lista.
- **agregar, clave existente:** actualiza el `info` del nodo de la lista (puede cambiar de tamaño) y lo
  mueve al principio. El árbol no cambia: la posición del nodo es la misma.
- **agregar, clave nueva:** si está llena, lee la entrada del último, elimina esa clave del árbol y saca
  el último de la lista. Después inserta al principio de la lista (obtiene la posición) e inserta
  `[clave][posición]` en el árbol. Si el árbol falla, saca el nodo recién insertado.
- **borrar:** busca en el árbol para tener la posición, elimina la clave del árbol y saca ese nodo de la
  lista.
- **vaciar:** vacía el árbol (libera solo sus entradas) y la lista (libera nodos y datos).

## Decisiones (y alternativa descartada)

| Decisión | Por qué | Alternativa descartada |
| --- | --- | --- |
| Lista doble con `pri` y `ult` | Mover al principio y descartar el último sin recorrer (RF-18) | Puntero flotante del apunte: no da acceso directo a los dos extremos |
| La posición sale como `void*` opaco | El árbol guarda bytes sin saber qué son; solo `lru_doble` conoce el significado | Que el árbol incluya `lista_doble.h`: acopla dos TDA base entre sí |
| El árbol copia la clave en su `info` | Cada estructura libera solo lo suyo (RF-16) | Compartir el puntero a la clave del nodo de la lista: obliga a liberar en un orden exacto |
| Reemplazo en el lugar con `actualizarInfoListaD` | La posición no cambia y el árbol no se toca | Sacar e insertar de nuevo (como la Parte 1): habría que actualizar la posición en el árbol |
| `verificarLruD` en la API | Las pruebas comprueban la consistencia (RF-15) sin mirar nodos | Revisar la consistencia solo leyendo el código |

## Estrategia de pruebas

- Primero la lista doble (D01 a D10), después los propios de la Parte 2 (26 a 32) y por último los 23
  casos compartidos con la Parte 1.
- `verificarLruD` después de cada operación en todos los casos de caché.
- El sanitizer es la prueba principal de RF-16: una doble liberación entre lista y árbol no siempre
  rompe el programa, pero el sanitizer la detecta.

## Trazabilidad

| RF | Dónde se resuelve | Casos |
| --- | --- | --- |
| RF-01 a RF-13 | Primitivas `LruD` | 01 a 25 |
| RF-14 | `alturaIndiceLruD` → `alturaArbol` | 26 a 29 |
| RF-15 | Orden de las operaciones de la tabla de consistencia | 01 a 23 (con `verificarLruD`), 29 a 32 |
| RF-16 | Dueños de la memoria: lista los datos, árbol su índice | 25, 33 |
| RF-17 | Búsqueda con `buscarArbol` | 34 |
| RF-18 | `moverAlPrinListaD`, `sacarUltListaD` con `pri` y `ult` | D04 a D06, 34 |
