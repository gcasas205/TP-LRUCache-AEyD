# Convenciones de la cátedra (UNLaM AyED)

Base para todos los TDA. Leer antes de escribir cualquier estructura.

## Códigos de retorno

Van en un header propio (p. ej. `retornos.h`). Los valores exactos los define la cátedra;
lo que se corrige es el **uso de los nombres simbólicos**, no el número. Un conjunto típico:

```c
#define EXITO            0
#define ERROR_MALLOC    -1

#define PILA_VACIA       1
#define PILA_LLENA       2
#define PILA_DISPONIBLE  0

#define COLA_VACIA       1
#define COLA_LLENA       2

#define LISTA_VACIA      1
```

Uso: las operaciones devuelven `EXITO` si salió bien y un código si no. Los bucles de vaciado
recorren mientras el retorno sea `EXITO`:

```c
int x;

while(outPila(&pila, &x, sizeof(x)) == EXITO)
    printf("%d\n", x);
```

## Macro MIN

Se usa al copiar hacia el buffer del usuario, para no escribir más de lo que entra:

```c
#define MIN(X, Y) ((X) > (Y) ? (Y) : (X))
```

(En el apunte aparece sin paréntesis; conviene parentizar los argumentos para evitar
sorpresas por precedencia.)

## Nodo genérico

```c
typedef struct sNodo
{
    void *info;         // puntero genérico; la info se reserva con malloc
    size_t tamInfo;     // tamaño en bytes de info
    struct sNodo *sig;  // siguiente nodo
} tNodo;

typedef tNodo* tPila;   // el tipo del TDA es un alias del puntero (tLista, tCola, ...)
```

El `struct sNodo` necesita el tag `sNodo` para **auto-referenciarse** en `struct sNodo *sig`;
el `typedef` a `tNodo` es para no escribir `struct sNodo` en todos lados.

## Patrón de inserción genérica

```c
tNodo *nue;

if(!(nue = (tNodo*)malloc(sizeof(tNodo))))   // 1. nodo
    return ERROR_MALLOC;

if(!(nue->info = malloc(tamInfo)))           // 2. espacio para la info
{
    free(nue);                               //    si falla, liberar el nodo
    return ERROR_MALLOC;
}

memcpy(nue->info, info, tamInfo);            // 3. copiar la info
nue->tamInfo = tamInfo;                      // 4. guardar el tamaño
/* 5. enganchar el nodo según el TDA */
```

## Patrón de extracción genérica

```c
tNodo *aux = *pTDA;

if(!aux)
    return TDA_VACIA;

*pTDA = aux->sig;                              // desconectar el nodo
memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo));  // copiar al buffer del usuario
free(aux->info);                               // liberar la info...
free(aux);                                     // ...y el nodo
return EXITO;
```

## Idiom de desplazamiento (puntero a puntero)

Las funciones reciben `tLista *pLista` (dirección del puntero a la cabeza) y avanzan con:

```c
while(*pLista)
    pLista = &(*pLista)->sig;
```

- `*pLista` → nodo actual.
- `(*pLista)->sig` → a dónde apunta el nodo actual.
- `&(*pLista)->sig` → **dirección** del campo `sig` dentro del nodo actual.

Ventaja: insertar/sacar en cualquier posición (incluida la cabeza) usa el mismo código, sin
casos especiales. Insertar en la posición actual:

```c
nue->sig = *pLista;   // el nuevo apunta a lo que había
*pLista = nue;        // y ocupa el lugar
```

## Estilo (recordatorio)

Llaves Allman (llave sola en la línea siguiente) en funciones y en `if`/`while`/`for`;
`if(`, `while(`, `for(` sin espacio; comentarios `//` en español; declaraciones al inicio
del bloque (C89).
