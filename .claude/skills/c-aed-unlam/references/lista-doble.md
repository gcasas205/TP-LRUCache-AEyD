# TDA Lista doblemente enlazada (estilo cátedra UNLaM)

Cada nodo tiene dos enlaces: `ant` (anterior) y `sig` (siguiente). Permite recorrer en ambos
sentidos y sacar del final en O(1) si se guarda el último.

## Estructura

```c
typedef struct sNodoD
{
    void *info;
    size_t tamInfo;
    struct sNodoD *ant;   // nodo anterior
    struct sNodoD *sig;   // nodo siguiente
} tNodoD;

typedef tNodoD* tListaD;
```

## Operaciones al principio

Al insertar/sacar hay que mantener **ambos** enlaces coherentes. El detalle fino es actualizar
el `ant` del que era primero.

```c
void crearListaD(tListaD *pLista)
{
    *pLista = NULL;
}

int insPrinListaD(tListaD *pLista, void *info, size_t tamInfo)
{
    tNodoD *nue;

    if(!(nue = (tNodoD*)malloc(sizeof(tNodoD))))
        return ERROR_MALLOC;
    if(!(nue->info = malloc(tamInfo)))
    {
        free(nue);
        return ERROR_MALLOC;
    }
    memcpy(nue->info, info, tamInfo);
    nue->tamInfo = tamInfo;

    nue->ant = NULL;
    nue->sig = *pLista;
    if(*pLista)                    // si había primero, su ant ahora es el nuevo
        (*pLista)->ant = nue;
    *pLista = nue;
    return EXITO;
}

int outPrinListaD(tListaD *pLista, void *info, size_t tamInfo)
{
    tNodoD *aux = *pLista;

    if(!aux)
        return LISTA_VACIA;

    *pLista = aux->sig;
    if(*pLista)                    // el nuevo primero no tiene anterior
        (*pLista)->ant = NULL;
    memcpy(info, aux->info, MIN(aux->tamInfo, tamInfo));
    free(aux->info);
    free(aux);
    return EXITO;
}
```

## Operaciones al final y en posición

Si se recorre hasta el último (`while((*p)->sig) ...`), al insertar al final el `ant` del nuevo
es el que era último. En posición determinada, al enganchar el nuevo nodo entre `a` y `b` hay
que fijar `nue->ant = a; nue->sig = b; a->sig = nue; b->ant = nue;` cuidando los extremos
(`a` o `b` en `NULL`).

## Variante de la cátedra: puntero "flotante" (pivot)

La implementación que enseña esta cátedra **no** ancla la lista siempre al primer nodo: el
puntero de la lista queda **cerca de la última operación** (el último nodo insertado,
eliminado o buscado). Como desde cualquier nodo se llega a toda la lista, insertar en orden
sobre datos semi-ordenados es más barato: no hay que recorrer desde el principio, se avanza o
retrocede unos pocos nodos desde el pivot.

En vez del desplazamiento `pLista = &(*pLista)->sig` de la lista simple, la búsqueda usa **dos
auxiliares** `auxAnt`/`auxSig` (nodo anterior y siguiente al punto de inserción) y **dos
`while` mutuamente excluyentes**: uno avanza mientras el dato sea mayor, el otro retrocede
mientras sea menor. Si la lista está ordenada, solo uno de los dos se ejecuta.

```c
tNodoD *auxSig = *pLista;                       // arranco en el pivot
tNodoD *auxAnt = auxSig ? auxSig->ant : NULL;

while(auxSig && cmp(auxSig->info, dato) < 0)    // avanzar hacia adelante
{
    auxAnt = auxSig;
    auxSig = auxSig->sig;
}
while(auxAnt && cmp(auxAnt->info, dato) > 0)    // o retroceder (excluyente)
{
    auxSig = auxAnt;
    auxAnt = auxAnt->ant;
}
```

Los cuatro enganches al insertar entre `auxAnt` y `auxSig`, con sus casos borde:

```c
nue->ant = auxAnt;
nue->sig = auxSig;
if(auxAnt)                     // si no hay anterior, es el nuevo primero
    auxAnt->sig = nue;
if(auxSig)                     // si no hay siguiente, es el nuevo último
    auxSig->ant = nue;
*pLista = nue;                 // dejar el pivot en la última operación
```

Al **eliminar** se reengancha `elim->ant->sig` y `elim->sig->ant` (validando extremos) y se
mueve el pivot a un vecino del nodo borrado.

## Recorrido tipo iterador (a veces lo piden en el parcial)

Aprovechando el pivot y el doble enlace, en lugar de `recorrerLista` con acción por puntero a
función se pueden ofrecer cuatro primitivas de recorrido que devuelven la info y **dejan el
pivot posicionado**, para recorrer con un `while` (como `fread` hasta EOF):

- `primeroLista`: va al primer nodo, deja el pivot ahí y devuelve su info.
- `siguienteLista`: mueve el pivot al siguiente y devuelve su info (o "no hay más" al final).
- `ultimoLista` / `anteriorLista`: lo inverso, para recorrer de atrás hacia adelante.

## Lo que penaliza un corrector

- Actualizar `sig` pero olvidar `ant` (o al revés): la lista queda inconsistente.
- No poner en `NULL` el `ant` del nuevo primero o el `sig` del nuevo último.
- No contemplar lista vacía o de un solo nodo.
- En la variante con pivot: no dejar el puntero de la lista sobre la última operación.
- Fugas: no liberar `info` y el nodo.
