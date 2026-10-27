# Spec 002 — LRU Cache con lista doble e índice (Parte 2)

Estado: implementada
Dueño: Persona 2
Depende de: spec 001 (comportamiento de referencia) y spec 003 (árbol índice)

## Contexto y objetivo
En la Parte 1 cada operación recorre la lista buscando la clave, así que su costo crece con la
capacidad. El enunciado pide una segunda versión, con las mismas primitivas, que busque por clave en un
árbol binario de búsqueda y mantenga el orden de uso en una lista doblemente enlazada. El usuario de la
caché no tiene que notar ninguna diferencia de comportamiento con la Parte 1: solo cambia el costo. Esta
versión además informa la altura del árbol índice.

## Usuarios
- El simulador de la Parte 3, que la compara contra la Parte 1.
- El informe, que usa la altura del índice para explicar el lote ordenado.

## Historias de usuario
- HU-1. Como simulador, quiero las mismas operaciones que en la Parte 1 para cambiar de versión sin
  cambiar mi código.
- HU-2. Como grupo, queremos conocer la altura del índice para explicar el rendimiento con datos
  ordenados.
- HU-3. Como programador, quiero que ningún dato quede sin liberar ni se libere dos veces aunque lo
  referencien dos estructuras.

## Definiciones
- **Uso, más reciente, menos reciente, cantidad:** iguales que en la spec 001.
- **Orden de uso:** la secuencia de elementos del más reciente al menos reciente.
- **Índice:** la estructura que permite encontrar un elemento por su clave sin recorrer el orden de uso.
- **Consistencia:** el índice y el orden de uso tienen exactamente las mismas claves y la misma cantidad
  de elementos, y cada clave del índice lleva al elemento con esa clave.

## Requisitos funcionales
- RF-01 a RF-13: idénticos a los RF-01 a RF-13 de la spec 001, aplicados a esta versión. Ante la misma
  secuencia de operaciones, las dos versiones devuelven los mismos códigos y los mismos valores.
- RF-14: CUANDO se consulta la altura del índice, EL SISTEMA devuelve 0 si la caché está vacía, 1 si
  tiene un elemento y, en general, la cantidad de niveles del árbol índice.
- RF-15: EL SISTEMA mantiene la consistencia entre el índice y el orden de uso después de cada creación,
  agregado, reemplazo, descarte por desborde, obtención, borrado y vaciado.
- RF-16: CUANDO se descarta, borra o vacía un elemento, EL SISTEMA libera su memoria exactamente una
  vez, tanto la del dato como la de su entrada en el índice.
- RF-17: EL SISTEMA busca las claves en el índice: obtener, agregar y borrar nunca recorren el orden de
  uso para encontrar una clave.
- RF-18: CUANDO hay que descartar el elemento menos reciente o mover un elemento al principio del orden
  de uso, EL SISTEMA lo hace sin recorrer el orden de uso.

## Requisitos no funcionales
- RNF-01: Cumple la constitución.
- RNF-02: Ninguna operación pierde memoria ni libera dos veces el mismo bloque (lo verifica el sanitizer).

## Casos límite
- Todos los de la spec 001.
- Mover al principio el elemento que ya es el primero, el último y uno del medio.
- Descartar cuando hay un solo elemento (capacidad 1).
- Reemplazar muchas veces la misma clave: el índice no debe crecer.
- Claves insertadas en orden creciente: el índice degenera y su altura es igual a la cantidad.
- Volver a agregar una clave que fue descartada por desborde.

## Fuera de alcance
- Balancear el índice (candidato a funcionalidad de la defensa).
- Cambiar la capacidad de una caché ya creada.

## Criterios de finalización
- Todos los casos de `casos-prueba.md` en OK, incluidos los de revisión.
- `/revisar 002` y `/validar 002` con `VEREDICTO: APROBADO`.
- Pruebas con el sanitizer sin reportes (si está disponible).

## Dudas abiertas
- (ninguna)
