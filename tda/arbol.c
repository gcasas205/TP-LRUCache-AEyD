#include "arbol.h"
#include <stdlib.h>
#include <string.h>

// Devuelve la direccion del puntero que apunta al nodo de esa clave; si no esta, la del NULL donde
// iria. Iterativa: en el lote ordenado el arbol degenera y una recursiva podria agotar la pila.
static tArbol *buscarNodoArbol(tArbol *pArbol, const void *clave, tCmp cmp)
{
    int comp;

    while(*pArbol && (comp = cmp(clave, (*pArbol)->info)) != 0)
        pArbol = (comp < 0)?&(*pArbol)->izq:&(*pArbol)->der;

    return pArbol;
}

// Direccion del puntero al nodo mas a la izquierda del subarbol (que no es vacio)
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

    nue = (tNodoA*)malloc(sizeof(tNodoA));
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
    nue->izq = NULL;
    nue->der = NULL;
    *pArbol = nue;

    return EXITO;
}

int buscarArbol(const tArbol *pArbol, const void *clave, void *info, size_t tamInfo, tCmp cmp)
{
    const tNodoA *act = *pArbol;
    int comp;

    while(act && (comp = cmp(clave, act->info)) != 0)
        act = (comp < 0)?act->izq:act->der;

    if(!act)
        return NO_ENCONTRADO;

    memcpy(info, act->info, MIN(act->tamInfo, tamInfo));

    return EXITO;
}

int eliminarArbol(tArbol *pArbol, const void *clave, tCmp cmp)
{
    tNodoA *elim;
    tArbol *pMenor;
    tNodoA *menor;

    pArbol = buscarNodoArbol(pArbol, clave, cmp);
    if(!*pArbol)
        return NO_ENCONTRADO;

    elim = *pArbol;

    if(elim->izq && elim->der)
    {
        // dos hijos: el nodo se queda con el info del menor del subarbol derecho, y se elimina el
        // nodo del menor (que no tiene hijo izquierdo). El info se pasa por puntero, sin copiarlo.
        pMenor = menorNodoArbol(&elim->der);
        menor = *pMenor;
        free(elim->info);
        elim->info = menor->info;
        elim->tamInfo = menor->tamInfo;
        *pMenor = menor->der;
        free(menor);
        return EXITO;
    }

    // hoja o un hijo: el hijo (o NULL) ocupa su lugar
    *pArbol = (elim->izq)?elim->izq:elim->der;
    free(elim->info);
    free(elim);

    return EXITO;
}

int alturaArbol(const tArbol *pArbol)
{
    int altIzq, altDer;

    if(!*pArbol)
        return 0;

    altIzq = alturaArbol(&(*pArbol)->izq);
    altDer = alturaArbol(&(*pArbol)->der);

    return 1 + ((altIzq > altDer)?altIzq:altDer);
}

int cantNodosArbol(const tArbol *pArbol)
{
    if(!*pArbol)
        return 0;

    return 1 + cantNodosArbol(&(*pArbol)->izq) + cantNodosArbol(&(*pArbol)->der);
}

int vaciarArbol(tArbol *pArbol)
{
    int cont;

    if(!*pArbol)
        return 0;

    // pos-orden: primero los hijos, despues el nodo (si no, se pierden los subarboles)
    cont = vaciarArbol(&(*pArbol)->izq) + vaciarArbol(&(*pArbol)->der);
    free((*pArbol)->info);
    free(*pArbol);
    *pArbol = NULL;

    return cont + 1;
}
