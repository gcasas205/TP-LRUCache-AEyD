# TDA Cola (estilo cátedra UNLaM)

FIFO. Se inserta por el **fin** (`ult`) y se saca por el **frente** (`pri`). Ver
`convenciones.md` para el nodo genérico y los códigos.

## Estructura

La cola dinámica usa una estructura con dos punteros a nodo:

```c
typedef struct sNodo
{
    void *info;
    size_t tamInfo;
    struct sNodo *sig;
} tNodo;

typedef struct
{
    tNodo *pri;    // primer nodo (se retira por acá)
    tNodo *ult;    // último nodo (se agrega por acá)
} tCola;
```

## Operaciones

```c
void crearCola(tCola *pCola)
{
    pCola->pri = NULL;
    pCola->ult = NULL;
}

int colaVacia(tCola *pCola)
{
    return pCola->pri ? EXITO : COLA_VACIA;
}

int insCola(tCola *pCola, void *info, size_t tamInfo)
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
    nue->sig = NULL;               // el nuevo va al final

    if(!pCola->ult)                // cola vacía
        pCola->pri = nue;
    else
        pCola->ult->sig = nue;     // enganchar al final actual
    pCola->ult = nue;              // el nuevo es el último
    return EXITO;
}

int outCola(tCola *pCola, void *info, size_t tamInfo)
{
    tNodo *aux = pCola->pri;

    if(!aux)
        return COLA_VACIA;

    pCola->pri = aux->sig;         // avanzar el frente
    memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo));
    free(aux->info);
    free(aux);

    if(!pCola->pri)                // si quedó vacía, resetear el último
        pCola->ult = NULL;
    return EXITO;
}

int verFrenteCola(tCola *pCola, void *info, size_t tamInfo)
{
    if(!pCola->pri)
        return COLA_VACIA;

    memcpy(info, pCola->pri->info, MIN(pCola->pri->tamInfo, tamInfo));
    return EXITO;
}

void vaciarCola(tCola *pCola)
{
    int basura;
    char buffer[256];

    while(outCola(pCola, buffer, sizeof(buffer)) == EXITO)
        ;
    (void)basura;
}
```

(En `vaciarCola` alcanza con recorrer y liberar; el `buffer` es un descarte. Si el tamaño de
la info puede superar el buffer, liberar directamente recorriendo los nodos como en la pila.)

## Cola circular

El último nodo enlaza con el primero. Se mantiene un solo puntero al **último**; el frente es
`ult->sig`. Encolar inserta después del último y desencolar saca `ult->sig`, actualizando el
enganche circular y poniendo el puntero en `NULL` cuando queda vacía.

## Lo que penaliza un corrector

- Olvidar poner `ult = NULL` cuando la cola queda vacía al sacar.
- Olvidar setear `pri` la primera vez que se encola.
- No poner `nue->sig = NULL` al encolar (deja el final enganchado a basura).
- Fugas: no liberar `info` y el nodo.
