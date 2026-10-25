#include "lista.h"
#include <stdlib.h>
#include <string.h>

void crearLista(tLista* pLista)
{
    *pLista = NULL;
}

int vaciarLista(tLista* pLista)
{
    tNodo *elim;
    int cont = 0;
    while(*pLista)
    {
        elim = *pLista;
        *pLista = elim->sig;
        free(elim->info);
        free(elim);
        cont++;
    }
    return cont;
}

void recorrerLista(const tLista* pLista, tAccion accion, void* contexto)
{
    while(*pLista)
    {
        accion((*pLista)->info, contexto);
        pLista = &(*pLista)->sig;
    }
}

int listaLlena(const tLista* pLista, size_t tamInfo)
{
    void *nue, *aux;
    (void)pLista;
    nue = malloc(sizeof(tNodo));
    aux = malloc(tamInfo);
    free(nue);
    free(aux);

    return (!nue || !aux)?LISTA_LLENA:LISTA_DISPONIBLE;
}

int listaVacia(const tLista* pLista)
{
    return (*pLista)?LISTA_DISPONIBLE:LISTA_VACIA;
}

int insPrinLista(tLista* pLista, const void* info, size_t tamInfo)
{
    tNodo *nue;
    if(listaLlena(pLista, tamInfo) == LISTA_LLENA)
        return ERROR_MALLOC;
    nue = (tNodo*)malloc(sizeof(tNodo));
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

    nue->sig = *pLista;
    *pLista = nue;

    return EXITO;
}

int buscarLista(const tLista* pLista, const void* clave, void* info, size_t tamInfo, tCmp cmp)
{
    while(*pLista)
    {
        if(cmp((*pLista)->info, clave) == 0)
        {
            memcpy(info, (*pLista)->info, MIN((*pLista)->tamInfo, tamInfo));
            return EXITO;
        }
        pLista = &(*pLista)->sig;
    }
    return NO_ENCONTRADO;
}

int llevarAlPrinLista(tLista* pLista, const void* clave, tCmp cmp)
{
    tLista *ini = pLista;
    tNodo *mover;
    while(*pLista && cmp((*pLista)->info, clave) != 0)
    {
        pLista = &(*pLista)->sig;
    }

    if(!(*pLista))
        return NO_ENCONTRADO;

    mover = *pLista;
    *pLista = mover->sig;
    mover->sig = *ini;
    *ini = mover;

    return EXITO;
}

int verPrimeroLista(const tLista* pLista, void* info, size_t tamInfo)
{
    if(listaVacia(pLista) == LISTA_VACIA)
        return VACIA;

    memcpy(info, (*pLista)->info, MIN((*pLista)->tamInfo, tamInfo));

    return EXITO;
}

int sacarPorClaveLista(tLista* pLista, const void* clave, void* info, size_t tamInfo, tCmp cmp)
{
    tNodo *elim;
    if(listaVacia(pLista) == LISTA_VACIA)
        return NO_ENCONTRADO;

    while(*pLista && cmp((*pLista)->info, clave) != 0)
        pLista = &(*pLista)->sig;

    if(!(*pLista))
        return NO_ENCONTRADO;

    if(info)
        memcpy(info, (*pLista)->info, MIN((*pLista)->tamInfo, tamInfo));

    elim = *pLista;
    *pLista = elim->sig;

    free(elim->info);
    free(elim);

    return EXITO;
}

int sacarUltLista(tLista* pLista, void* info, size_t tamInfo)
{
    tNodo *elim;
    if(listaVacia(pLista) == LISTA_VACIA)
        return VACIA;

    while((*pLista)->sig)
        pLista = &(*pLista)->sig;

    if(info)
        memcpy(info, (*pLista)->info, MIN((*pLista)->tamInfo, tamInfo));

    elim = *pLista;
    *pLista = elim->sig;

    free(elim->info);
    free(elim);

    return EXITO;
}
