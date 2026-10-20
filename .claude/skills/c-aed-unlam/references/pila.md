# TDA Pila (estilo cátedra UNLaM)

LIFO. Ver `convenciones.md` para el nodo genérico y los códigos de retorno.

## Pila dinámica

Tipo: `typedef tNodo* tPila;`. Todas las operaciones reciben `tPila *pPila`.

```c
void crearPila(tPila *pPila)
{
    *pPila = NULL;                 // pila vacía = puntero a NULL
}

int pilaVacia(tPila *pPila)
{
    return *pPila ? PILA_DISPONIBLE : PILA_VACIA;
}

int insPila(tPila *pPila, void *info, size_t tamInfo)
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

    nue->sig = *pPila;             // el nuevo apunta al viejo tope
    *pPila = nue;                  // el nuevo es el tope
    return EXITO;
}

int outPila(tPila *pPila, void *info, size_t tamInfo)
{
    tNodo *aux = *pPila;

    if(!aux)
        return PILA_VACIA;

    *pPila = aux->sig;             // el tope pasa al siguiente
    memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo));
    free(aux->info);
    free(aux);
    return EXITO;
}

int verTopePila(const tPila *pPila, void *info, size_t tamInfo)
{
    if(!*pPila)
        return PILA_VACIA;

    memcpy(info, (*pPila)->info, MIN((*pPila)->tamInfo, tamInfo));
    return EXITO;
}

void vaciarPila(tPila *pPila)
{
    tNodo *aux;

    while(*pPila)
    {
        aux = *pPila;
        *pPila = aux->sig;
        free(aux->info);
        free(aux);
    }
}
```

### `pilaLlena` en la versión dinámica

Con memoria dinámica la pila "nunca se llena" en la práctica, pero la primitiva se implementa
igual para **mantener la misma interfaz que la estática** (las 7 primitivas): se intenta
reservar nodo e info, se liberan de inmediato y se responde según si se pudo.

```c
int pilaLlena(tPila *pPila, size_t tamInfo)
{
    tNodo *nodo = (tNodo*)malloc(sizeof(tNodo));
    void  *dato = malloc(tamInfo);
    int    llena = (!nodo || !dato);   // si alguno falló, no hay lugar

    free(nodo);                        // free(NULL) es seguro
    free(dato);
    return llena ? PILA_LLENA : EXITO;
}
```

### Intercambiabilidad estática ↔ dinámica

Pila estática y pila dinámica exponen **las mismas primitivas con la misma firma**; solo
cambia la implementación interna. Por eso un programa que usa únicamente las primitivas (sin
tocar la estructura por dentro) puede pasar de una a otra cambiando el archivo `.c` que se
compila. En la práctica conviene **reemplazar el archivo de implementación** en el proyecto en
vez de compilar los dos juntos, porque las funciones se llaman igual y colisionan.

## Pila estática (con arreglo de bytes)

La cátedra también ve la pila **estática**: un arreglo `unsigned char` de tamaño fijo y un
índice `tope`. Se apila copiando los bytes de la info seguidos de su `tamInfo`.

```c
#define TAM_PILA 1000

typedef struct
{
    unsigned char datos[TAM_PILA];
    size_t tope;                   // índice del primer byte libre
} tPilaE;

void crearPilaE(tPilaE *pila)
{
    pila->tope = 0;
}

int pilaVaciaE(tPilaE *pila)
{
    return pila->tope == 0 ? PILA_VACIA : EXITO;
}

int pilaLlenaE(tPilaE *pila, size_t tamInfo)
{
    return pila->tope + tamInfo + sizeof(size_t) > TAM_PILA ? PILA_LLENA : EXITO;
}
```

(La inserción copia `tamInfo` bytes de la info y luego `sizeof(size_t)` bytes con el tamaño,
para poder recuperarlo al sacar. Detallar según lo pida el enunciado.)

## Pila circular

Variante de lista circular: el `sig` del último nodo apunta al primero. El puntero del TDA
suele guardar el **último** nodo (así se accede al primero como `ult->sig` en O(1)). Al
insertar/sacar hay que mantener el enganche circular:

```c
int insPilaCirc(tNodo **pUlt, void *info, size_t tamInfo)
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

    if(!*pUlt)                     // estaba vacía: se apunta a sí mismo
    {
        nue->sig = nue;
        *pUlt = nue;
    }
    else                          // insertar como nuevo tope
    {
        nue->sig = (*pUlt)->sig;  // el nuevo apunta al primero
        (*pUlt)->sig = nue;       // el último apunta al nuevo
    }
    return EXITO;
}
```

## Lo que penaliza un corrector

- No liberar `info` **y** el nodo al sacar/vaciar.
- No chequear `malloc` (nodo e info por separado).
- Al sacar de una pila de un solo nodo, dejar el puntero colgando en vez de `NULL`.
- En circular: romper el enlace del último con el primero.
