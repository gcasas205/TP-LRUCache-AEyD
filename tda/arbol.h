#ifndef ARBOL_H_INCLUDED
#define ARBOL_H_INCLUDED

#include <stddef.h>
#include "comun.h"

typedef struct sNodoA
{
    void *info;
    size_t tamInfo;
    struct sNodoA *izq;
    struct sNodoA *der;
}tNodoA;

typedef tNodoA *tArbol;

// Solo las primitivas que usa lru_doble (spec 002). El resto del contrato es de la spec 003.
void crearArbol(tArbol *pArbol);

int insArbol(tArbol *pArbol, const void *info, size_t tamInfo, tCmp cmp);
int buscarArbol(const tArbol *pArbol, const void *clave, void *info, size_t tamInfo, tCmp cmp);
int eliminarArbol(tArbol *pArbol, const void *clave, tCmp cmp);
int alturaArbol(const tArbol *pArbol);
int cantNodosArbol(const tArbol *pArbol);
int vaciarArbol(tArbol *pArbol);

#endif // ARBOL_H_INCLUDED
