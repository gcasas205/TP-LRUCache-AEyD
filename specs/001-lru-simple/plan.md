# Plan — Spec 001: LRU Cache sobre lista simple

Parte de la spec 001 en borrador; se ajusta cuando se apruebe. Las firmas y retornos exactos están en
`docs/primitivas.md` (sección Spec 001); acá van las responsabilidades, las decisiones y la trazabilidad.

## Archivos y responsabilidades

| Archivo | Responsabilidad | Usa |
| --- | --- | --- |
| `tda/comun.h` | Códigos de retorno, `MIN`, `tCmp`, `tAccion`. Lo incluyen todos los módulos | — |
| `tda/lista.h` / `.c` | Lista simple genérica: insertar al principio, buscar por dato parcial, llevar al principio, ver el primero, sacar por clave, sacar el último, recorrer, vaciar | `comun.h` |
| `cache/entrada.h` / `.c` | Armar el bloque `[clave][valor]` y copiar el valor de un bloque | `comun.h` |
| `cache/lru_simple.h` / `.c` | `tLruS` y sus cinco primitivas | `lista`, `entrada` |
| `pruebas/verificar.h` / `.c` | `verificarCond`, `verificarInt`, `verificarTexto` imprimen `[OK]` / `[FALLA] (esperado ..., obtenido ...)` y cuentan; `imprimirResumen` imprime `RESUMEN:` y devuelve las fallas (la usan las 4 specs) | — |
| `pruebas/pruebas_001.c` | `int pruebas001(void)`: todos los casos de `casos-prueba.md`; devuelve la cantidad de fallas | `lru_simple`, `lista`, `entrada` |

## Cómo resuelve cada operación (sin código)

- **agregar:** arma la entrada con `crearEntrada`. Si `sacarPorClaveLista` encuentra la clave, la saca
  (el reemplazo es sacar + insertar al principio). Si no estaba, inserta al principio, suma 1 y, si
  superó la capacidad, saca el último. Libera el bloque temporal al final (la lista guardó su copia).
- **obtener:** `llevarAlPrinLista`; si encontró, `verPrimeroLista` a un buffer temporal del tamaño de la
  entrada y `copiarValorEntrada` al buffer del llamador. Un solo recorrido hace la búsqueda y el
  reordenamiento.
- **borrar:** `sacarPorClaveLista` con `info = NULL`; si encontró, resta 1.
- **vaciar:** `vaciarLista` y `cantidad = 0`; capacidad y `cmp` se conservan.

## Decisiones (y alternativa descartada)

| Decisión | Por qué | Alternativa descartada |
| --- | --- | --- |
| Entrada `[clave][valor]` en un solo `info` | El `cmp` del usuario compara directo contra el principio del bloque: búsqueda por dato parcial sin comparador intermedio | Nodo con dos punteros (`clave` y `valor`): obliga a cambiar el nodo genérico de la cátedra |
| La caché lleva `cantidad` | Saber si está llena sin recorrer la lista | Contar nodos en cada `agregar`: recorrido extra en cada operación |
| Reemplazo = sacar + insertar al principio | Reusa dos primitivas probadas y el valor puede cambiar de tamaño | Modificar el `info` en el lugar y después mover: necesita una primitiva más en la lista |
| `llevarAlPrinLista` re-enlaza el nodo | No hay `malloc` ni `free` al obtener, que es la operación más frecuente | Sacar y volver a insertar: dos reservas por cada acierto |
| Pruebas de caja negra para la caché | Los mismos casos sirven después para la Parte 2 (spec 002) y para la interfaz (spec 003) | Mirar los nodos desde las pruebas: rompe el principio 4 |

## Estrategia de pruebas

- Primero las primitivas de la lista (L01 a L14), después la caché (01 a 23).
- Un agente escribe todos los casos en `pruebas_001.c` **antes** del código; la persona escribe el código que los hace pasar.
- Los valores de prueba son distintos entre sí ("A", "B", "Z", longitudes diferentes) para que un dato
  perdido o cruzado se note.
- Los casos 24 y 25 son de revisión: los verifica `/revisar 001` y el sanitizer en `/validar 001`.

## Trazabilidad

| RF | Dónde se resuelve | Casos |
| --- | --- | --- |
| RF-01, RF-02 | `crearLruS` | 01, 02, 03 |
| RF-03 | `agregarLruS` (clave nueva) | 04 |
| RF-04 | `agregarLruS` + `sacarUltLista` | 05, 06, 22, 23 |
| RF-05 | `agregarLruS` + `sacarPorClaveLista` | 07, 08, 09, 10 |
| RF-06 | `obtenerLruS` + `llevarAlPrinLista` | 04, 06 |
| RF-07 | `obtenerLruS` (camino `NO_ENCONTRADO`) | 11, 12 |
| RF-08, RF-09 | `borrarLruS` | 13 a 17 |
| RF-10 | `vaciarLruS` + `vaciarLista` | 18, 25 |
| RF-11 | `entrada` + genericidad de la lista | 19, 20 |
| RF-12 | `MIN` en `copiarValorEntrada` | 21 |
| RF-13 | Chequeos de `malloc` en lista, entrada y caché | 24 |
