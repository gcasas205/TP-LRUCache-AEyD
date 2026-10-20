# TDA Lista simplemente enlazada (estilo cátedra UNLaM)

Misma estructura de nodo que Pila (`convenciones.md`), con `typedef tNodo* tLista;`. Casi todo
se resuelve con el **desplazamiento puntero a puntero** (`pLista = &(*pLista)->sig`).

## Crear y operaciones al principio

```c
void crearLista(tLista *pLista)
{
    *pLista = NULL;
}

int insPrinLista(tLista *pLista, void *info, size_t tamInfo)
{
    tNodo *nue;

    if(!(nue = (tNodo*)malloc(sizeof(tNodo))))
        return ERROR_MALLOC;
    if(!(nue->info = malloc(tamInfo)))
    {
        free(nue);
        return ERROR_MALLOC;
    }
    memcpy(nue->info, info, tamInfo);
    nue->tamInfo = tamInfo;

    nue->sig = *pLista;            // apunta al que era primero
    *pLista = nue;                 // ocupa la cabeza
    return EXITO;
}

int verPrinLista(tLista *pLista, void *info, size_t tamInfo)
{
    if(!*pLista)
        return LISTA_VACIA;

    memcpy(info, (*pLista)->info, MIN((*pLista)->tamInfo, tamInfo));
    return EXITO;
}

int outPrinLista(tLista *pLista, void *info, size_t tamInfo)
{
    tNodo *aux = *pLista;

    if(!aux)
        return LISTA_VACIA;

    *pLista = aux->sig;
    memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo));
    free(aux->info);
    free(aux);
    return EXITO;
}
```

## Operaciones al final (con puntero a puntero)

Insertar al final = avanzar hasta el `sig` que vale `NULL` e insertar ahí. Como se trabaja
con la **dirección** del puntero, insertar al final es igual que al principio pero desplazado:

```c
int insFinLista(tLista *pLista, void *info, size_t tamInfo)
{
    tNodo *nue;

    while(*pLista)                 // avanzar hasta el final
        pLista = &(*pLista)->sig;

    if(!(nue = (tNodo*)malloc(sizeof(tNodo))))
        return ERROR_MALLOC;
    if(!(nue->info = malloc(tamInfo)))
    {
        free(nue);
        return ERROR_MALLOC;
    }
    memcpy(nue->info, info, tamInfo);
    nue->tamInfo = tamInfo;

    nue->sig = *pLista;            // *pLista vale NULL acá
    *pLista = nue;
    return EXITO;
}

int outFinLista(tLista *pLista, void *info, size_t tamInfo)
{
    tNodo *aux;

    if(!*pLista)
        return LISTA_VACIA;

    while((*pLista)->sig)          // parar en el ANTEúltimo
        pLista = &(*pLista)->sig;

    aux = *pLista;
    memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo));
    *pLista = NULL;
    free(aux->info);
    free(aux);
    return EXITO;
}
```

## Operaciones en posición determinada

`insPosLista(tLista *pLista, void *info, size_t tamInfo, size_t pos)`: desplazar `pos` veces
con `pLista = &(*pLista)->sig;` y ahí insertar (igual que al principio sobre ese `*pLista`).
`outPosLista` y `verPosLista`, análogas. Chequear que `pos` no supere el largo (si al salir
del desplazamiento `pos` no llegó a 0, se salió por fin de lista → posición inexistente). Si la
primera posición es `0` o `1` **se aclara en el enunciado**; cambia solo el valor inicial.

## Insertar ordenado

`insOrdLista(tLista *pLista, void *info, size_t tamInfo, int cmp(void*, void*))`: avanzar
mientras `cmp((*pLista)->info, info) < 0` y luego insertar en esa posición. El `cmp` lo provee
quien usa el TDA (define el criterio de orden).

## Recorrer, reduce, filter y búsqueda (con puntero a función)

El estilo funcional de la cátedra: se pasa una función que decide qué hacer con cada dato.

```c
void recorrerLista(tLista *pLista, void accion(void*, void*), void *param)
{
    while(*pLista)
    {
        accion((*pLista)->info, param);   // aplicar acción al dato
        pLista = &(*pLista)->sig;
    }
}
```

- `param` es un dato extra genérico (p. ej. un `FILE*` para escribir, un acumulador, etc.);
  si no se necesita, se pasa `NULL`.
- **reduce**: `reduceLista(tLista *pLista, void redu(void*, void*, void*), void *result, void *param)`
  acumula en `result` recorriendo la lista.
- **filter**: recorre y conserva solo los nodos que cumplen un predicado.
- **búsqueda**: `busqClaveLista(tLista *pLista, int cmp(void*, void*), void *clave)` devuelve
  `EXITO` si algún nodo matchea la clave según `cmp`.

## Sacar por clave (patrón de parcial)

Sacar un nodo buscándolo por una clave. Hay dos variantes según si la lista está ordenada:

- **Desordenada** (`outElemLista`): avanzar mientras `cmp(...) != 0`. Corta al encontrarlo o al
  llegar al final (`*pLista == NULL` → no estaba).
- **Ordenada** (`outElemOrdLista`): avanzar mientras `cmp(...) > 0`. Al cortar, si el resultado
  **no** es 0 (se pasó) o `*pLista == NULL`, el elemento no existe. Es más barata porque puede
  frenar antes.

```c
int outElemOrdLista(tLista *pLista, void *dato, size_t tamDato, int cmp(void*, void*))
{
    tNodo *elim;

    while(*pLista && cmp((*pLista)->info, dato) < 0)  // avanzar mientras el nodo sea menor
        pLista = &(*pLista)->sig;

    if(!*pLista || cmp((*pLista)->info, dato) != 0)   // fin de lista o se pasó: no está
        return LISTA_VACIA;

    elim = *pLista;
    memcpy(dato, elim->info, MIN(elim->tamInfo, tamDato));  // devolver el dato completo
    *pLista = elim->sig;                              // desenganchar (elim se pierde si no se guardó)
    free(elim->info);
    free(elim);
    return EXITO;
}
```

### El idiom del "dato parcial" (esto entra al parcial)

Para buscar/sacar por clave se pasa un puntero a una **estructura con solo el campo clave
lleno**; el resto se ignora en la comparación. Al encontrar el nodo, se **copia el registro
completo en ese mismo puntero** (por eso `dato` es entrada *y* salida). Ej.: recibo un
`tAlumno` con solo `legajo` cargado, busco por legajo y al encontrarlo copio nombre, edad, etc.
en esa misma estructura. La `cmp` debe comparar únicamente por la clave.

## Corte de control (acumular por clave)

Sobre una lista **ordenada por una clave**, agrupar nodos consecutivos con la misma clave en
uno solo (acumulando, p. ej., cantidades). Se recorre comparando cada nodo con el siguiente:
mientras la clave del actual sea igual a la del siguiente, se acumula y se elimina el
siguiente; cuando cambia, se pasa al próximo grupo. Es un ejercicio típico (ej.: pedidos
desordenados por código de producto que hay que ordenar y luego acumular por código).

## Invertir, sacar duplicados, ordenar

- `invLista(tLista *pLista)`: invierte los enlaces (iterativo con tres punteros, o recursivo).
- `outDupliLista(tLista *pLista, int cmp(void*, void*))`: elimina nodos repetidos según `cmp`.
- `ordenarLista(tLista *pLista, int cmp(void*, void*))`: ordena (p. ej. inserción sobre otra
  lista, o burbujeo intercambiando info).

## Lo que penaliza un corrector

- No usar el desplazamiento puntero a puntero y terminar con casos especiales rotos para la
  cabeza o el final.
- Perder la cabeza (no trabajar sobre `*pLista`).
- No liberar `info` y nodo al sacar.
- En `outFinLista`, parar en el último en vez del anteúltimo, o no poner `NULL`.
