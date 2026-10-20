# Memoria dinámica (estilo cátedra UNLaM, base C89)

## Reglas base

- Reservar con `malloc` de `<stdlib.h>`. Casteo al tipo (estilo cátedra: `(tNodo*)malloc(...)`).
- Chequear siempre el resultado. Idiom de la cátedra, todo en el `if`:

```c
if(!(nue = (tNodo*)malloc(sizeof(tNodo))))
    return ERROR_MALLOC;
```

- Reservar la info con su tamaño y copiar con `memcpy` (de `<string.h>`):

```c
if(!(nue->info = malloc(tamInfo)))
{
    free(nue);                 // si falla la 2da reserva, liberar lo ya pedido
    return ERROR_MALLOC;
}
memcpy(nue->info, info, tamInfo);
nue->tamInfo = tamInfo;
```

- Al liberar un nodo genérico, liberar **primero la info y después el nodo**:

```c
free(aux->info);
free(aux);
```

- Tras liberar, si el puntero se sigue usando, ponerlo en `NULL`. Nunca `free` dos veces.

## Arreglo dinámico

```c
int *v;

if(!(v = (int*)malloc(n * sizeof(int))))
    return ERROR_MALLOC;
/* usar v[0..n-1] */
free(v);
```

## Matriz dinámica (arreglo de punteros)

Reservar el vector de filas y cada fila; liberar en orden inverso.

```c
int **crearMatriz(int filas, int cols)
{
    int **m;
    int i;

    if(!(m = (int**)malloc(filas * sizeof(int*))))
        return NULL;
    for(i = 0; i < filas; ++i)
    {
        if(!(m[i] = (int*)malloc(cols * sizeof(int))))
        {
            while(--i >= 0)
                free(m[i]);
            free(m);
            return NULL;
        }
    }
    return m;
}

void liberarMatriz(int **m, int filas)
{
    int i;

    for(i = 0; i < filas; ++i)
        free(m[i]);
    free(m);
}
```

## realloc

Guardar el resultado en un temporal para no perder el bloque original si devuelve `NULL`.

```c
int *tmp;

if(!(tmp = (int*)realloc(v, nuevo * sizeof(int))))
    return ERROR_MALLOC;         // v sigue valido
v = tmp;
```

## Lo que penaliza un corrector

- No chequear `NULL` tras `malloc`.
- Fugas: no liberar la info **y** el nodo; olvidar la función que vacía la estructura.
- `sizeof` del tipo equivocado.
- Usar o liberar memoria dos veces, o usar un puntero después de `free`.
- Devolver la dirección de una variable local (memoria que deja de existir al salir).
