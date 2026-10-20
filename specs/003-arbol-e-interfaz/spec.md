# Spec 003 — Árbol índice e interfaz común de caché

Estado: borrador
Dueño: Persona 3

## Contexto y objetivo
Dos piezas que conectan las demás specs. El **árbol binario de búsqueda** genérico es el índice que pide
el enunciado para la Parte 2: encuentra un elemento por clave sin recorrer la lista. La **interfaz
común** permite que el simulador de la Parte 3 use "sin caché", la Parte 1 o la Parte 2 sin cambiar su
código, que es lo que hace falta para comparar las tres en las mismas condiciones.

## Usuarios
- La caché de la Parte 2 (spec 002), que usa el árbol como índice.
- El simulador y el reporte (spec 004), que usan la interfaz común.

## Historias de usuario
- HU-1. Como caché de la Parte 2, quiero insertar, buscar y eliminar por clave en un índice genérico.
- HU-2. Como simulador, quiero elegir la versión de caché al crearla y después usarla siempre igual.
- HU-3. Como reporte, quiero saber el nombre de la versión y la altura de su índice, si tiene.

## Definiciones
- **Recorrido en orden:** subárbol izquierdo, nodo, subárbol derecho.
- **Altura:** cantidad de niveles; 0 para el árbol vacío.
- **Variante de caché:** "sin caché", "Lista simple" (spec 001) o "Lista doble + ABB" (spec 002).

## Requisitos funcionales

Árbol:
- RF-01: CUANDO se inserta una clave que no está, EL SISTEMA la agrega de modo que el recorrido en orden
  devuelva todas las claves en orden ascendente según la función de comparación.
- RF-02: SI se inserta una clave que ya está, ENTONCES EL SISTEMA devuelve `DUPLICADO` y no modifica el
  árbol.
- RF-03: CUANDO se busca una clave (con un dato que solo tiene la clave cargada), EL SISTEMA devuelve
  `EXITO` y copia el dato completo; SI no está, ENTONCES devuelve `NO_ENCONTRADO` sin tocar el destino.
- RF-04: CUANDO se elimina una clave que está, sea hoja, nodo con un hijo, nodo con dos hijos o raíz,
  EL SISTEMA la quita, libera su memoria y el recorrido en orden de las restantes sigue ascendente.
- RF-05: SI se elimina una clave que no está, ENTONCES EL SISTEMA devuelve `NO_ENCONTRADO` sin cambios.
- RF-06: CUANDO se consulta la altura o la cantidad de nodos, EL SISTEMA devuelve 0 para el árbol vacío,
  1 para un solo nodo y los valores exactos en general.
- RF-07: CUANDO se insertan 5000 claves en orden creciente, EL SISTEMA las guarda todas, informa altura
  5000 y sigue buscando, eliminando y vaciando sin agotar la pila.
- RF-08: CUANDO se vacía el árbol, EL SISTEMA libera todos sus nodos y sus datos propios, devuelve la
  cantidad de nodos liberados y no libera la memoria a la que apunten esos datos.

Interfaz común:
- RF-09: EL SISTEMA permite crear una caché de cualquiera de las tres variantes y después agregar,
  obtener, borrar y vaciar con las mismas operaciones, sin que quien la usa sepa qué variante es.
- RF-10: MIENTRAS la variante es "sin caché", EL SISTEMA devuelve `EXITO` al agregar sin guardar nada, y
  `NO_ENCONTRADO` al obtener y al borrar.
- RF-11: CUANDO se opera a través de la interfaz con la Parte 1 o la Parte 2, EL SISTEMA devuelve
  exactamente los mismos códigos y valores que la primitiva directa de esa versión.
- RF-12: CUANDO se consulta, EL SISTEMA informa el nombre de la variante y la altura del índice, o -1 si
  la variante no tiene índice.
- RF-13: SI la creación falla (capacidad 0, comparación nula o falta de memoria), ENTONCES EL SISTEMA
  devuelve el mismo código que la versión de abajo y no deja memoria reservada.
- RF-14: CUANDO se vacía la caché a través de la interfaz, EL SISTEMA libera toda su memoria, incluida
  la estructura de la variante.

## Requisitos no funcionales
- RNF-01: Cumple la constitución.
- RNF-02: Agregar una variante nueva (otra política) solo requiere una implementación nueva y cambios en
  la interfaz; el simulador no cambia.

## Casos límite
- Árbol: eliminar la raíz con dos hijos, eliminar el único nodo, eliminar en árbol vacío, insertar
  duplicado, árbol degenerado de 5000 nodos.
- Interfaz: crear con capacidad 0; usar "sin caché" en todas las operaciones; vaciar y volver a crear.

## Fuera de alcance
- Balancear el árbol (candidato a funcionalidad de la defensa).
- Usar la interfaz común en las pruebas internas de las specs 001 y 002 (es opcional).

## Criterios de finalización
- Todos los casos de `casos-prueba.md` en OK, incluidos los de revisión.
- `/revisar 003` y `/validar 003` con `VEREDICTO: APROBADO`.

## Dudas abiertas
- (ninguna)
