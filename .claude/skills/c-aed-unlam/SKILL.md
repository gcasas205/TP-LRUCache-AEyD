---
name: c-aed-unlam
description: Escribir, revisar, corregir y explicar código en C para la materia Algoritmos y Estructuras de Datos de UNLaM (Ingeniería Informática), siguiendo el estilo de la cátedra (apunte de Tiago Pujia 2025) sobre base ANSI C89. Cubre TDA genéricos con void* (Pila, Cola, Lista simple, circular y doblemente enlazada), árboles binarios, recursividad, memoria dinámica y archivos binarios y de texto. Úsalo SIEMPRE que el usuario comparta código en C para revisar o corregir, pida resolver un ejercicio o parcial, pida implementar o usar un TDA o estructura de datos, o mencione K&R, C89/ANSI C, "parcial de C", cátedra, UNLaM, recursividad, punteros, malloc, TDA, pila, cola, lista, lista doble, circular, árbol o archivos. Si el contexto es C académico, este es el estilo por defecto aunque no lo pidan explícitamente.
---

# C para AyED (UNLaM) — estilo de la cátedra sobre base C89

Este skill hace que el código en C se ajuste al estilo con el que **se toman los parciales**
en Algoritmos y Estructuras de Datos de UNLaM (Ing. Informática), tal como está en el apunte
de la cátedra (Tiago Pujia, 2025). La base es ANSI C89, pero **donde el estilo de la cátedra
difiere del K&R "de libro", manda la cátedra**.

## Idioma y comentarios

- **Todo en español**: comentarios y nombres. Comentarios con `//` (la cátedra los usa).
- Base **C89**: declaraciones al inicio del bloque, sin construcciones de C99+ (VLAs,
  `for (int i...)`, `<stdbool.h>`, `<stdint.h>`, `long long`, inicializadores designados).

## Compilación (cátedra)

La cátedra usa **CodeBlocks con MinGW** y compila con **warnings estrictos** (`-Wall -Wextra
-pedantic`). En el parcial, el ejercicio **tiene que compilar sin errores ni warnings y andar**
para que lo evalúen. Por eso se penaliza lo que MinGW deja pasar pero no es estándar (aritmética
sobre `void*`, declarar variables fuera del inicio del bloque): rompe la portabilidad a otros
compiladores (p. ej. GCC en Linux). Escribir siempre apegado al estándar.

## Estilo de la cátedra (formato)

- **Llaves estilo Allman**: la llave de apertura va **sola en la línea siguiente**, tanto en
  funciones como en `if`, `while`, `for`. La de cierre, sola en su línea.
- `if`, `while`, `for` **sin espacio** antes del paréntesis: `if(cond)`, `while(*pLista)`.
- Indentación de 4 espacios, consistente.
- Punteros con el asterisco pegado a la variable en declaraciones: `void *info`, `tNodo *nue`.
  En casts se ve pegado al tipo: `(tNodo*)malloc(...)`.

```c
int insPila(tPila *pPila, void *info, size_t tamInfo)
{
    tNodo *nue;

    if(!(nue = (tNodo*)malloc(sizeof(tNodo))))
        return ERROR_MALLOC;
    ...
}
```

## Convenciones de nombres (¡esto es lo que mira el corrector!)

- **Estructura de nodo**: `struct sNodo` con el `typedef` a `tNodo`; el tipo del TDA es un
  alias del puntero: `typedef tNodo* tPila;` (o `tLista`, `tCola`, `tArbol`).
- **Prefijos**: `s` para el tag del struct (`sNodo`), `t` para tipos (`tNodo`, `tPila`),
  `p` para punteros-parámetro (`pPila`, `pLista`, `pCola`). `nue` = nodo nuevo, `aux` = auxiliar.
- **Funciones en camelCase con sufijo del TDA**: `crearPila`, `insPila`, `outPila`,
  `verTopePila`, `vaciarPila`, `pilaVacia`, `pilaLlena`; `insCola`/`outCola`/`verFrenteCola`;
  la familia de lista `insPrinLista`/`insFinLista`/`insPosLista`/`insOrdLista`, etc.
- **Retornos por códigos simbólicos** (`#define`): `EXITO`, `ERROR_MALLOC`, `PILA_VACIA`,
  `PILA_LLENA`, `COLA_VACIA`, etc. El éxito es `EXITO`; los bucles hacen
  `while(outPila(&pila, &x, sizeof(x)) == EXITO)`.

Ver `references/convenciones.md` para la tabla de códigos, el nodo genérico y las macros.

## Nodo genérico con void* (núcleo del estilo)

Los TDA de la cátedra son **genéricos**: el nodo guarda un puntero `void *info` y el tamaño
`size_t tamInfo`, reservando la info con `malloc` y copiándola con `memcpy`. Así el mismo TDA
sirve para `int`, `float`, cadenas o structs.

```c
typedef struct sNodo
{
    void *info;         // puntero genérico; la info se guarda con malloc
    size_t tamInfo;     // tamaño en bytes de info
    struct sNodo *sig;  // puntero al siguiente nodo
} tNodo;

typedef tNodo* tPila;   // (o tLista, tCola)
```

Patrón de inserción: reservar el nodo, reservar `info` (si falla, liberar el nodo y devolver
`ERROR_MALLOC`), `memcpy(nue->info, info, tamInfo)`, guardar `tamInfo`, y enganchar.

## Idiom estrella: desplazamiento con puntero a puntero

Las funciones reciben la **dirección del puntero** al primer nodo (`tLista *pLista`) y avanzan
con `pLista = &(*pLista)->sig;`. Así insertar/sacar al principio, al medio o al final se
resuelven con el mismo código, sin casos especiales para la cabeza.

```c
while(*pLista)                      // mientras haya nodo
    pLista = &(*pLista)->sig;       // avanzar al campo sig del nodo actual
```

Interpretación: `*pLista` = nodo actual; `(*pLista)->sig` = a dónde apunta; `&(*pLista)->sig`
= la dirección del campo `sig` dentro del nodo actual. Dominar esto es central en la materia.

## Recursividad

Es un eje de la materia. Escribir con **caso base primero** (condición de corte) y luego el
**caso recursivo** acercándose al corte. Se usa mucho para recorrer listas y árboles.

```c
int cantNodos(tArbol *arbol)
{
    if(!*arbol)                     // caso base: árbol/subárbol vacío
        return 0;
    return 1 + cantNodos(&(*arbol)->izq) + cantNodos(&(*arbol)->der);
}
```

## Router de temas (leer la referencia antes de escribir código de ese tema)

- **Convenciones, códigos de retorno, nodo genérico, macros** → `references/convenciones.md`.
- **Genéricos sobre vectores** (`void*`, aritmética con `char*`, comparadores tipo qsort,
  puntero a función con/sin typedef, intercambio genérico, ordenar por selección, qsort) →
  `references/genericos.md`.
- **Memoria dinámica** (malloc/free/memcpy, matrices, fugas) → `references/memoria-dinamica.md`.
- **Pila** (dinámica, estática y circular) → `references/pila.md`.
- **Cola** (dinámica con pri/ult, y circular) → `references/cola.md`.
- **Lista simple** (principio/final/posición, recorrer/reduce/filter/búsqueda, ordenar,
  invertir, duplicados, insertar ordenado, sacar por clave, dato parcial, corte de control) →
  `references/lista.md`.
- **Lista doblemente enlazada** (incluye variante con puntero flotante y recorrido tipo
  iterador) → `references/lista-doble.md`.
- **Recursividad** (método, condición de corte, `return`, función envoltorio, orden = pila) →
  `references/recursividad.md`.
- **Árbol binario** (insertar con cmp iterativo/recursivo, recorridos genéricos, altura,
  completo/balanceado/AVL, conteo por nivel, búsqueda y dato parcial, mín/máx y mayor/menor no
  clave, eliminar, carga ordenada por bisección, índice) → `references/arbol.md`.
- **Archivos** binarios y de texto (genéricos, con puntero a función) → `references/archivos.md`.

Reglas transversales:
- Toda memoria pedida se libera; incluir siempre la función que **vacía** la estructura.
- Chequear `NULL` tras cada `malloc` (patrón `if(!(nue = malloc(...))) return ERROR_MALLOC;`).
- Las operaciones informan su resultado por el código de retorno (`EXITO`, `..._VACIA`, ...).
- Al copiar hacia el buffer del usuario, usar `memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo))`.

## Modo: Revisar / corregir código

1. Ajustar al estilo cátedra: nombres (`sNodo`/`tNodo`/`tXxx`/`pXxx`), llaves Allman,
   códigos de retorno, nodo genérico, puntero a puntero.
2. Adaptar lo no-C89 (declaraciones al inicio, sacar `for (int ...)`, etc.).
3. Marcar lo que un corrector penaliza: fugas de memoria, no chequear `malloc`, no liberar
   `info` **y** el nodo, perder la cabeza de la lista, no actualizar `ult`/`pri` en la cola,
   faltar el caso base en recursión.
4. Devolver el código corregido completo y una lista breve de cambios y su porqué.

## Modo: Explicar / resolver parcial

Explicar con el vocabulario y los ejemplos del apunte. Al resolver, entregar código completo
y compilable en el estilo de la cátedra, con comentarios breves y, debajo, la idea en pocas
líneas y el caso borde que suele mirar el corrector (estructura vacía, un solo nodo, EOF).

**Formato del parcial**: es la resolución de un ejercicio **en máquina**; tiene que compilar
sin errores ni warnings y correr. Dos modalidades posibles: (a) se entrega un código
**precompilado** al que hay que modificar/agregar una funcionalidad, o (b) solo el **enunciado**
y se escribe todo desde cero. En el parcial se permiten como ayuda únicamente dos archivos: el
del ANSI C (funciones de biblioteca estándar) y la tabla ASCII. Suele durar ~4 horas. Varía por
cátedra/docente: algunos (p. ej. Renata) toman un enunciado grande y se resuelve **desde cero**;
otros (p. ej. Néstor Pan) dan el programa hecho y se **completan funciones**.

**Buenas prácticas que se corrigen** (errores frecuentes de parcial):
- **Analizar y dibujar antes de codificar** (memoria, punteros, casos borde); no ir directo al
  código.
- **Lote de prueba significativo**: valores *distintos* (no todos iguales) pensados para revelar
  si se pierde un dato o si el orden salió bien; hacer el sanity check obvio (un promedio no
  puede superar el máximo de los datos).
- **Modularizar de verdad**: un `main` que solo llama a `menu()` con toda la lógica adentro no es
  modularizar; y una función cuyo cuerpo es solo llamar a otra no es envoltorio (un envoltorio
  inicializa/ajusta parámetros, ver `recursividad.md`).
- **`int / int` es división entera** en C (trunca): para promedios castear a `float`/`double`.
  Dejar el `return 0;` del `main`.
- **Recorridos según el orden pedido**: ABB ascendente = In-Orden (izq-raíz-der); descendente =
  In-Orden **inverso** (der-raíz-izq), *no* Pos-Orden. En lista simple, asc/desc se logra
  cambiando el orden de la resta en el `cmp`; en lista doble, recorriendo en un sentido u otro.
- **Manejo correcto de las estructuras**: los enganches mal hechos en las primitivas cuentan
  como errores importantes, no solo la lógica del enunciado.

Para todo lo de archivos (texto vs binario, `feof`/`fseek`, cierre, longitud fija, corte de
control), ver `references/archivos.md`.
