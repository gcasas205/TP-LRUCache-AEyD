#ifndef ARBOL_H_INCLUDED
#define ARBOL_H_INCLUDED

#include <stddef.h>
#include "comun.h"

typedef struct sNodoA
{
    void *info;
    size_t tamInfo;
    struct sNodoA *izq, *der;
}tNodoA;

typedef tNodoA *tArbol;

void crearArbol(tArbol *pArbol);
int insArbol(tArbol *pArbol, const void *info, size_t tamInfo, tCmp cmp);
int buscarArbol(const tArbol *pArbol, const void *clave, void *info, size_t tamInfo, tCmp cmp);
int eliminarArbol(tArbol *pArbol, const void *clave, tCmp cmp);
int alturaArbol(const tArbol *pArbol);
int cantNodosArbol(const tArbol *pArbol);
void recorrerInOrdenArbol(const tArbol *pArbol, tAccion accion, void *contexto);
int vaciarArbol(tArbol *pArbol);

#endif
