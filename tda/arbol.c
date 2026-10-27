#include "arbol.h"
#include <stdlib.h>
#include <string.h>

void crearArbol(tArbol *pArbol)
{

}

int insArbol(tArbol *pArbol, const void *info, size_t tamInfo, tCmp cmp)
{
    return EXITO;
}

int buscarArbol(const tArbol *pArbol, const void *clave, void *info, size_t tamInfo, tCmp cmp)
{
    return NO_ENCONTRADO;
}

int eliminarArbol(tArbol *pArbol, const void *clave, tCmp cmp)
{
    return NO_ENCONTRADO;
}

int alturaArbol(const tArbol *pArbol)
{
    return 0;
}

int cantNodosArbol(const tArbol *pArbol)
{
    return 0;
}

void recorrerInOrdenArbol(const tArbol *pArbol, tAccion accion, void *contexto)
{

}

int vaciarArbol(tArbol *pArbol)
{
    return 0;
}

static tArbol *buscarNodoArbol(tArbol *pArbol, const void *clave, tCmp cmp)
{
    return NULL;
}

static tArbol *menorNodoArbol(tArbol *pArbol)
{
    return NULL;
}
