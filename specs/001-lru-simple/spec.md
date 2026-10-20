# Spec 001 — LRU Cache sobre lista simple (Parte 1)

Estado: borrador
Dueño: Persona 1

## Contexto y objetivo
El enunciado pide un TDA LRU Cache genérico: guarda pares clave-valor de cualquier tipo y tamaño hasta
una capacidad máxima y, cuando se llena, descarta el elemento usado hace más tiempo. Esta spec cubre la
primera versión, donde el orden de uso vive en una lista simplemente enlazada. Su comportamiento
observable es la referencia que la Parte 2 (spec 002) tiene que repetir exactamente.

## Usuarios
- El simulador de la Parte 3, que guarda posteos por id.
- Cualquier programa que necesite una caché genérica (en la defensa pueden pedir otro uso).

## Historias de usuario
- HU-1. Como simulador, quiero guardar un posteo bajo su id para no volver a leerlo del archivo.
- HU-2. Como simulador, quiero pedir un posteo por id y saber si estaba, para contar aciertos y fallos.
- HU-3. Como programador, quiero usar la misma caché con claves y valores de cualquier tipo.
- HU-4. Como programador, quiero vaciar la caché y recuperar toda la memoria.

## Definiciones
- **Uso de un elemento:** agregarlo (nuevo o reemplazado) u obtenerlo con éxito. Una búsqueda fallida y
  un borrado no son usos.
- **Más reciente / menos reciente:** el elemento con el uso más nuevo / más viejo entre los guardados.
- **Cantidad:** número de elementos guardados en un momento dado. Nunca supera la capacidad.

## Requisitos funcionales
- RF-01: CUANDO se crea una caché con capacidad N > 0 y una función de comparación, EL SISTEMA la deja
  sin elementos y con capacidad N.
- RF-02: SI la capacidad es 0 o la función de comparación es nula, ENTONCES EL SISTEMA rechaza la
  creación con `ERROR_PARAMETRO`.
- RF-03: CUANDO se agrega una clave que no está y la cantidad es menor que la capacidad, EL SISTEMA la
  guarda como el elemento más reciente y la cantidad aumenta en 1.
- RF-04: CUANDO se agrega una clave que no está y la cantidad es igual a la capacidad, EL SISTEMA
  descarta el elemento menos reciente, guarda el nuevo como el más reciente y la cantidad no cambia.
- RF-05: CUANDO se agrega una clave que ya está, EL SISTEMA reemplaza su valor (aunque tenga otro
  tamaño), la deja como el elemento más reciente y no descarta ningún otro elemento.
- RF-06: CUANDO se obtiene una clave que está, EL SISTEMA devuelve `EXITO`, copia su valor en el buffer
  del llamador y la deja como el elemento más reciente.
- RF-07: SI se obtiene una clave que no está, ENTONCES EL SISTEMA devuelve `NO_ENCONTRADO` y no modifica
  ni el buffer del llamador ni el orden de uso.
- RF-08: CUANDO se borra una clave que está, EL SISTEMA la elimina, libera su memoria, la cantidad baja
  en 1 y el orden de uso de los demás no cambia.
- RF-09: SI se borra una clave que no está, ENTONCES EL SISTEMA devuelve `NO_ENCONTRADO` sin cambios.
- RF-10: CUANDO se vacía la caché, EL SISTEMA libera toda la memoria de los elementos y la deja sin
  elementos, utilizable de nuevo con la misma capacidad y función de comparación.
- RF-11: EL SISTEMA acepta claves y valores de cualquier tipo y tamaño, comparando claves solo con la
  función recibida al crearla.
- RF-12: SI el buffer del llamador es más chico que el valor guardado, ENTONCES EL SISTEMA copia solo
  los bytes que entran y no escribe fuera del buffer.
- RF-13: SI falla una reserva de memoria, ENTONCES EL SISTEMA devuelve `ERROR_MALLOC`, no deja ningún
  elemento a medio guardar y la caché sigue siendo utilizable. Si fallaba un reemplazo (RF-05), la clave
  puede quedar fuera de la caché, nunca con un valor corrupto.

## Requisitos no funcionales
- RNF-01: Cumple la constitución (compila limpio en las dos configuraciones, estilo de la cátedra).
- RNF-02: Ninguna operación pierde memoria ni libera dos veces el mismo bloque.

## Casos límite
- Capacidad 1: cada clave nueva descarta a la anterior.
- Agregar una clave existente con la caché llena: no descarta a nadie (RF-05).
- Obtener o borrar en una caché vacía.
- Borrar el único elemento, el más reciente y el menos reciente.
- Reemplazar un valor por otro más largo y por otro más corto.
- Muchas más inserciones que la capacidad (1000 claves con capacidad 100).

## Fuera de alcance
- Validar punteros nulos en clave, valor o buffer: es responsabilidad del llamador.
- Claves con tamaño variable dentro de una misma caché (todas las claves de una caché miden lo mismo).
- Recorrer o mostrar la caché (candidato a funcionalidad de la defensa).

## Criterios de finalización
- Todos los casos de `casos-prueba.md` en OK, incluidos los de revisión.
- `/revisar 001` y `/validar 001` con `VEREDICTO: APROBADO`.
- Pruebas sin reportes del sanitizer (si está disponible).

## Dudas abiertas
- ~~¿El docente acepta los nombres `crearLruS`, `agregarLruS`, etc., en lugar de `crear_lrucache`?~~
  **Resuelta (20/10/2026):** el docente acepta camelCase si todas las primitivas usan el mismo estilo.
