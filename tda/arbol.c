#include "arbol.h"
#include <stdlib.h>
#include <string.h>

static tArbol *buscarNodoArbol(tArbol *pArbol, const void *clave, tCmp cmp)
{
    int rc;

    while(*pArbol)
    {
        if((rc = cmp(clave, (*pArbol)->info)) < 0)
            pArbol = &(*pArbol)->izq;
        else if(rc > 0)
            pArbol = &(*pArbol)->der;
        else
            return pArbol;
    }

    return pArbol;
}

static tArbol *menorNodoArbol(tArbol *pArbol)
{
    while((*pArbol)->izq)
        pArbol = &(*pArbol)->izq;
    return pArbol;
}

void crearArbol(tArbol *pArbol)
{
    *pArbol = NULL;
}

int insArbol(tArbol *pArbol, const void *info, size_t tamInfo, tCmp cmp)
{
    tNodoA *nue;

    pArbol = buscarNodoArbol(pArbol, info, cmp);
    if(*pArbol)
        return DUPLICADO;

    if(!(nue = (tNodoA*)malloc(sizeof(tNodoA))))
        return ERROR_MALLOC;
    if(!(nue->info = malloc(tamInfo)))
    {
        free(nue);
        return ERROR_MALLOC;
    }
    memcpy(nue->info, info, tamInfo);
    nue->tamInfo = tamInfo;
    nue->izq = NULL;
    nue->der = NULL;
    *pArbol = nue;
    return EXITO;
}

int buscarArbol(const tArbol *pArbol, const void *clave, void *info, size_t tamInfo, tCmp cmp)
{
    tArbol *arbol;

    arbol = buscarNodoArbol((tArbol*)pArbol, clave, cmp);
    if(!*arbol)
        return NO_ENCONTRADO;

    memcpy(info, (*arbol)->info, MIN((*arbol)->tamInfo, tamInfo));

    return EXITO;
}

int eliminarArbol(tArbol *pArbol, const void *clave, tCmp cmp)
{
    tArbol *pos;
    tArbol *menor;
    tNodoA *elim;
    void *auxInfo;
    size_t auxTam;

    pos = buscarNodoArbol(pArbol, clave, cmp);
    if(!*pos)
        return NO_ENCONTRADO;

    elim = *pos;

    if(elim->izq && elim->der)
    {
        menor = menorNodoArbol(&elim->der);

        auxInfo = elim->info;
        elim->info = (*menor)->info;
        (*menor)->info = auxInfo;

        auxTam = elim->tamInfo;
        elim->tamInfo = (*menor)->tamInfo;
        (*menor)->tamInfo = auxTam;

        pos = menor;
        elim = *pos;
    }

    *pos = elim->izq ? elim->izq : elim->der;

    free(elim->info);
    free(elim);
    return EXITO;
}

int alturaArbol(const tArbol *pArbol)
{
    int hi, hd;

    if(!*pArbol)
        return 0;
    hi = alturaArbol(&(*pArbol)->izq);
    hd = alturaArbol(&(*pArbol)->der);
    return (hi > hd ? hi : hd) + 1;

}

int cantNodosArbol(const tArbol *pArbol)
{
    if(!*pArbol)
        return 0;
    return cantNodosArbol(&(*pArbol)->izq)
         + cantNodosArbol(&(*pArbol)->der) + 1;
}

void recorrerInOrdenArbol(const tArbol *pArbol, tAccion accion, void *contexto)
{
    if(!*pArbol)
        return;

    recorrerInOrdenArbol(&(*pArbol)->izq, accion, contexto);
    accion((*pArbol)->info, contexto);
    recorrerInOrdenArbol(&(*pArbol)->der, accion, contexto);
}

int vaciarArbol(tArbol *pArbol)
{
    int cantIzq, cantDer;

    if(!*pArbol)
        return 0;

    cantIzq = vaciarArbol(&(*pArbol)->izq);
    cantDer = vaciarArbol(&(*pArbol)->der);

    free((*pArbol)->info);
    free(*pArbol);
    *pArbol = NULL;

    return cantIzq + cantDer + 1;
}
