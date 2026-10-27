#include "lista_doble.h"
#include <stdlib.h>
#include <string.h>


void crearListaD(tListaD *pLista)
{
    pLista->pri = pLista->ult = NULL;
}

int listaVaciaD(const tListaD *pLista)
{
    return pLista->pri?LISTA_DISPONIBLE:LISTA_VACIA;
}

int insPrinListaD(tListaD *pLista, const void *info, size_t tamInfo, tPosListaD *pos)
{
    tNodoD *nue;
    nue = (tNodoD*)malloc(sizeof(tNodoD));
    if(!nue)
    {
        *pos = NULL;
        return ERROR_MALLOC;
    }

    nue->info = malloc(tamInfo);
    if(!nue->info)
    {
        *pos = NULL;
        free(nue);
        return ERROR_MALLOC;
    }

    memcpy(nue->info, info, tamInfo);
    nue->tamInfo = tamInfo;
    nue->ant = NULL;

    nue->sig = pLista->pri;

    if(pLista->pri)
        pLista->pri->ant = nue; // Si hay nodo insertado, pongo como anterior al nuevo
    else
        pLista->ult = nue; // Sino hay nodo insertado, el ultimo es el primero.
    pLista->pri = nue; // Asigno como primero siempre al que inserto.

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
    void *nue;
    tNodoD *nodoPos = (tNodoD*)pos;

    nue = malloc(tamInfo);
    if(!nue)
        return ERROR_MALLOC;

    memcpy(nue, info, tamInfo);
    free(nodoPos->info);
    nodoPos->tamInfo = tamInfo;
    nodoPos->info = nue;

    return EXITO;
}

void moverAlPrinListaD(tListaD *pLista, tPosListaD pos)
{
    tNodoD *nodo = (tNodoD*)pos;
    if(nodo == pLista->pri)
        return;

    nodo->ant->sig = nodo->sig;
    if(nodo->sig)
        nodo->sig->ant = nodo->ant;
    else
        pLista->ult = nodo->ant;

    nodo->ant = NULL;
    nodo->sig = pLista->pri;
    pLista->pri->ant = nodo;
    pLista->pri = nodo;
}

int sacarNodoListaD(tListaD *pLista, tPosListaD pos, void *info, size_t tamInfo)
{
    tNodoD *nodo = (tNodoD*)pos;
    if(listaVaciaD(pLista) == LISTA_VACIA)
        return VACIA;

    // LADO IZQUIERDO
    if(nodo->ant)
        nodo->ant->sig = nodo->sig;
    else
        pLista->pri = nodo->sig;

    // LADO DERECHO
    if(nodo->sig)
        nodo->sig->ant = nodo->ant;
    else
        pLista->ult = nodo->ant;

    // COPIAR (solo si quien llama dio un buffer)
    if(info)
        memcpy(info, nodo->info, MIN(nodo->tamInfo,tamInfo));

    // LIBERAR: primero el info, despues el nodo
    free(nodo->info);
    free(nodo);

    return EXITO;
}

int verUltListaD(const tListaD *pLista, void *info, size_t tamInfo)
{
    if(listaVaciaD(pLista) == LISTA_VACIA)
        return VACIA;

    memcpy(info, pLista->ult->info, MIN(pLista->ult->tamInfo, tamInfo));

    return EXITO;
}

int sacarUltListaD(tListaD *pLista, void *info, size_t tamInfo)
{
    if(listaVaciaD(pLista) == LISTA_VACIA)
        return VACIA;

    return sacarNodoListaD(pLista, pLista->ult, info, tamInfo);
}

void recorrerListaD(const tListaD *pLista, tAccion accion, void *contexto)
{
    tNodoD *nodo = pLista->pri;
    while(nodo)
    {
        accion(nodo->info, contexto);
        nodo = nodo->sig;
    }
}

int vaciarListaD(tListaD *pLista)
{
    int cont = 0;
    tNodoD *nodo = pLista->pri;
    while(nodo)
    {
        tNodoD *elim = nodo;
        nodo = nodo->sig;
        free(elim->info);
        free(elim);
        cont++;
    }
    pLista->pri = pLista->ult = NULL;

    return cont;
}
