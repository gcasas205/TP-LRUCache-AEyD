#include "lista_doble.h"
#include <stdlib.h>
#include <string.h>

// Saca el nodo de la cadena sin liberarlo. Cada extremo se resuelve por separado:
// si no tiene anterior era el primero (mueve pri); si no tiene siguiente era el ultimo (mueve ult).
static void desengancharNodoD(tListaD *pLista, tNodoD *nodo)
{
    if(nodo->ant)
        nodo->ant->sig = nodo->sig;
    else
        pLista->pri = nodo->sig;

    if(nodo->sig)
        nodo->sig->ant = nodo->ant;
    else
        pLista->ult = nodo->ant;
}

void crearListaD(tListaD *pLista)
{
    pLista->pri = NULL;
    pLista->ult = NULL;
}

int listaVaciaD(const tListaD *pLista)
{
    return (pLista->pri)?LISTA_DISPONIBLE:LISTA_VACIA;
}

int insPrinListaD(tListaD *pLista, const void *info, size_t tamInfo, tPosListaD *pos)
{
    tNodoD *nue;
    *pos = NULL;

    nue = (tNodoD*)malloc(sizeof(tNodoD));
    if(!nue)
        return ERROR_MALLOC;

    nue->info = malloc(tamInfo);
    if(!nue->info)
    {
        free(nue);
        return ERROR_MALLOC;
    }

    memcpy(nue->info, info, tamInfo);
    nue->tamInfo = tamInfo;

    nue->ant = NULL;
    nue->sig = pLista->pri;
    if(pLista->pri)
        pLista->pri->ant = nue;
    else
        pLista->ult = nue;
    pLista->pri = nue;

    *pos = nue;
    return EXITO;
}

size_t verInfoListaD(tPosListaD pos, void *info, size_t tamInfo)
{
    tNodoD *nodo = (tNodoD*)pos;

    memcpy(info, nodo->info, MIN(nodo->tamInfo, tamInfo));

    return nodo->tamInfo;
}

int actualizarInfoListaD(tPosListaD pos, const void *info, size_t tamInfo)
{
    tNodoD *nodo = (tNodoD*)pos;
    void *nuevo;

    // primero el bloque nuevo: si falla, el nodo conserva su info anterior
    nuevo = malloc(tamInfo);
    if(!nuevo)
        return ERROR_MALLOC;

    memcpy(nuevo, info, tamInfo);
    free(nodo->info);
    nodo->info = nuevo;
    nodo->tamInfo = tamInfo;

    return EXITO;
}

void moverAlPrinListaD(tListaD *pLista, tPosListaD pos)
{
    tNodoD *nodo = (tNodoD*)pos;

    if(nodo == pLista->pri)
        return;

    // como no era el primero, la lista tiene al menos otro nodo y pri no queda en NULL
    desengancharNodoD(pLista, nodo);

    nodo->ant = NULL;
    nodo->sig = pLista->pri;
    pLista->pri->ant = nodo;
    pLista->pri = nodo;
}

int sacarNodoListaD(tListaD *pLista, tPosListaD pos, void *info, size_t tamInfo)
{
    tNodoD *elim = (tNodoD*)pos;

    desengancharNodoD(pLista, elim);

    if(info)
        memcpy(info, elim->info, MIN(elim->tamInfo, tamInfo));

    free(elim->info);
    free(elim);

    return EXITO;
}

int verUltListaD(const tListaD *pLista, void *info, size_t tamInfo)
{
    if(!pLista->ult)
        return VACIA;

    memcpy(info, pLista->ult->info, MIN(pLista->ult->tamInfo, tamInfo));

    return EXITO;
}

int sacarUltListaD(tListaD *pLista, void *info, size_t tamInfo)
{
    if(!pLista->ult)
        return VACIA;

    return sacarNodoListaD(pLista, pLista->ult, info, tamInfo);
}

void recorrerListaD(const tListaD *pLista, tAccion accion, void *contexto)
{
    tNodoD *act = pLista->pri;

    while(act)
    {
        accion(act->info, contexto);
        act = act->sig;
    }
}

int vaciarListaD(tListaD *pLista)
{
    tNodoD *elim;
    int cont = 0;

    while(pLista->pri)
    {
        elim = pLista->pri;
        pLista->pri = elim->sig;
        free(elim->info);
        free(elim);
        cont++;
    }
    pLista->ult = NULL;

    return cont;
}
