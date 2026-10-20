# Spec 004 — Generador de lotes, simulación e informe (Parte 3)

Estado: borrador
Dueño: Persona 4
Depende de: spec 003 (interfaz común); usa las specs 001 y 002 a través de ella

## Contexto y objetivo
La red social "Tweener" quiere saber si una LRU Cache acelera la carga del feed. El enunciado pide
generar lotes de prueba reproducibles, simular la lectura de todos los feeds sin caché, con la Parte 1 y
con la Parte 2, medir tiempos, aciertos, fallos y lecturas, y entregar un informe de no más de 3 páginas
que explique los resultados.

Para que la diferencia entre caché y archivo se vea, las corridas tienen que ser largas (muchos
pedidos) y el costo de leer el archivo tiene que ser realista. El archivo de posteos (7,4 MB) queda en la
memoria del sistema operativo después de la primera lectura, así que "leer del disco" cuesta una llamada
al sistema (alrededor de 1 µs) y no un acceso físico. Por eso la simulación permite, además del acceso
directo, un acceso con seek simulado que encarece cada lectura del archivo, y el informe lo declara.

## Usuarios
- El grupo, que corre el generador y el simulador y escribe el informe.
- Los docentes, que leen el informe y pueden pedir regenerar un lote en la defensa.

## Historias de usuario
- HU-1. Como grupo, queremos regenerar exactamente el mismo lote a partir de una semilla.
- HU-2. Como grupo, queremos lotes concentrados, dispersos y ordenados para mostrar situaciones distintas.
- HU-3. Como grupo, queremos una tabla clara y un CSV con todas las corridas para armar el informe.
- HU-4. Como grupo, queremos corridas lo bastante largas para que el reloj las mida con precisión.
- HU-5. Como grupo, queremos comparar con un acceso a disco más caro para mostrar cómo cambian las
  conclusiones según el costo de leer el archivo.

## Definiciones
- **Posteo:** registro `{id: int, texto: char[141]}`; su id es su posición en el archivo.
- **Usuario:** registro `{usuario: char[15], feed: int[20]}`.
- **Lote:** archivo de usuarios. **Pedidos del lote:** usuarios × 20.
- **Acierto / fallo:** el posteo pedido estaba / no estaba en la caché.
- **Lectura:** un acceso al archivo de posteos para traer un registro.
- **Corrida:** una simulación completa de un lote con una variante, una capacidad y un modo de acceso.
- **Modo de acceso:** cómo se lee un registro del archivo. **Directo:** posicionarse en el registro y
  leerlo. **Con seek:** mover primero el puntero del archivo al final, después posicionarse en el
  registro y leerlo.
- **Calentamiento:** lectura secuencial del archivo de posteos completo, antes de medir, que no cuenta
  como lectura.

## Requisitos funcionales

Generador:
- RF-01: CUANDO se ejecuta el generador, EL SISTEMA crea el archivo de 50000 posteos, donde el registro i
  tiene id i y un texto de menos de 141 caracteres terminado en `'\0'`.
- RF-02: CUANDO se ejecuta dos veces con la misma semilla y los mismos parámetros, EL SISTEMA genera
  archivos idénticos byte a byte; con otra semilla, los lotes concentrado y disperso son distintos.
- RF-03: EL SISTEMA genera cada lote con la cantidad de usuarios pedida (al menos 200, sin máximo fijo),
  nombres de hasta 14 caracteres terminados en `'\0'` y 20 ids por feed, todos entre 0 y 49999.
- RF-04: CUANDO genera el lote concentrado, EL SISTEMA toma todos los ids de un grupo de G posteos
  (G es un parámetro).
- RF-05: CUANDO genera el lote disperso, EL SISTEMA reparte los ids en todo el rango: hay ids en cada una
  de las diez décimas partes del rango (incluida 45000-49999) y, con 200 usuarios, al menos el 90 % de
  los ids del lote son distintos (lo esperable por azar es 96 %).
- RF-06: CUANDO genera el lote ordenado, EL SISTEMA asigna al pedido número j del lote (contando usuarios y
  feeds en orden, desde 0) el id ⌊j × 50000 / pedidos del lote⌋: los ids nunca decrecen, el primero es
  0, cubren el rango de forma pareja y, con más de 50000 pedidos, cada id se repite seguido.
- RF-07: SI faltan parámetros o la cantidad de usuarios es menor que 200, ENTONCES EL SISTEMA muestra cómo
  se usa, termina con código 1 y no crea ningún archivo.

Simulación:
- RF-08: CUANDO se simula una corrida, EL SISTEMA obtiene cada posteo de cada feed de cada usuario: de la
  caché si está, y si no, del archivo con el modo de acceso de la corrida (y lo agrega a la caché).
- RF-09: MIENTRAS se mide una corrida, EL SISTEMA no muestra, imprime ni guarda posteos.
- RF-10: CUANDO termina una corrida, EL SISTEMA informa el tiempo total y los promedios por posteo
  (total / posteos procesados) y por usuario (total / usuarios procesados).
- RF-11: CUANDO termina una corrida, EL SISTEMA informa aciertos, fallos y lecturas, que cumplen
  aciertos + fallos = posteos procesados y lecturas = fallos. Sin caché, aciertos = 0.
- RF-12: CUANDO la variante es la Parte 2, EL SISTEMA informa la altura del índice al terminar la corrida;
  en las otras variantes muestra "-".
- RF-13: CUANDO se ejecuta el simulador completo, EL SISTEMA corre, para cada modo de acceso pedido y cada
  lote, una corrida sin caché y corridas con la Parte 1 y la Parte 2 con al menos 4 capacidades. Muestra
  una tabla con una fila por corrida y escribe las mismas filas en un CSV separado por `;`.
- RF-14: EL SISTEMA obtiene los mismos aciertos, fallos y lecturas al repetir una corrida, con la Parte 1
  y con la Parte 2 para igual lote y capacidad (las dos son LRU), y en los dos modos de acceso (el modo
  solo cambia el tiempo).
- RF-15: SI no encuentra el archivo de posteos o un lote, ENTONCES EL SISTEMA informa cuál falta y termina
  con código 1, sin dejar archivos abiertos.
- RF-17: CUANDO empieza una corrida, EL SISTEMA hace el calentamiento antes de empezar a medir, para que
  todas las corridas encuentren el archivo de posteos en la memoria del sistema operativo. El
  calentamiento no cuenta en las lecturas ni en el tiempo.
- RF-18: EL SISTEMA permite elegir el modo de acceso (directo, con seek o ambos) al ejecutar el
  simulador; el modo solo afecta las lecturas del archivo e identifica cada fila de la tabla y del CSV.

Informe:
- RF-16: EL SISTEMA (el grupo) entrega un informe de no más de 3 páginas con tablas o gráficos de las
  corridas, que responde las tres preguntas del enunciado con números de esas corridas, anota la semilla
  y los parámetros, y declara que el modo con seek es un costo de acceso simulado (con el costo por
  lectura medido en cada modo) y que el archivo de posteos se lee desde la memoria del sistema operativo.

## Requisitos no funcionales
- RNF-01: Cumple la constitución.
- RNF-02: El simulador solo conoce la interfaz común: no incluye `lru_simple.h` ni `lru_doble.h`.
- RNF-03: Los tiempos por posteo y por usuario se calculan como promedio del total, nunca midiendo una
  sola operación (el enunciado lo exige por la resolución del reloj).
- RNF-04: En la máquina donde se miden los resultados del informe, cada corrida sin caché en modo directo
  dura al menos 1 segundo. Si no, se aumenta la cantidad de usuarios del lote.
- RNF-05: Agregar otro modo de acceso solo requiere una función de lectura nueva con la misma firma; el
  simulador no cambia.

## Casos límite
- Ids repetidos dentro de un mismo feed (posibles en el concentrado): cuentan como dos pedidos.
- Capacidad mayor que la cantidad de ids distintos del lote.
- Lote ordenado con muchos usuarios: cada id se repite unas 20 veces seguidas (casi todo aciertos, pero
  el índice de la Parte 2 queda como una cadena del largo de la capacidad).
- Lotes de 50000 usuarios o más: el cálculo de RF-06 no puede hacerse en `long`, que en MinGW tiene 32
  bits y desborda.
- Archivo de posteos más corto que el id pedido.
- Parte 1 con capacidades grandes en el lote disperso: corridas de decenas de segundos (es un resultado
  esperado, no un error).

## Fuera de alcance
- Medir el tiempo de cada posteo individual.
- Mostrar el contenido de los posteos.
- Generar los gráficos desde el programa (se hacen con el CSV en una planilla).
- Vaciar la caché de archivos del sistema operativo entre corridas (no es portable).

## Criterios de finalización
- Todos los casos de `casos-prueba.md` en OK.
- Tabla y CSV de todas las corridas, en los dos modos, generados con la semilla y los parámetros
  anotados en el informe.
- Informe revisado por el grupo contra RF-16.
- `/revisar 004` y `/validar 004` con `VEREDICTO: APROBADO`.

## Dudas abiertas
- [NECESITA ACLARACIÓN] ¿Qué hace el simulador si un feed trae un id fuera de rango? Propuesta: lo cuenta
  como fallo, no lo agrega a la caché y sigue. El generador nunca lo produce.
- [NECESITA ACLARACIÓN] Valores definitivos: capacidades (propuesta 10, 100, 1000, 5000), G del
  concentrado (propuesta 300) y cantidad de usuarios (propuesta 50000, o sea un millón de pedidos;
  ajustar hasta cumplir RNF-04 en la máquina del informe).
