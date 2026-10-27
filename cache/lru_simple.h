#ifndef LRU_SIMPLE_H_INCLUDED
#define LRU_SIMPLE_H_INCLUDED

#include <stddef.h>
#include "comun.h"
#include "lista.h"

typedef struct
{
    tLista lista;
    unsigned capacidad;
    unsigned cantidad;
    tCmp cmp;
}tLruS;

int crearLruS(tLruS *pLru, unsigned capacidad, tCmp cmp);
int agregarLruS(tLruS *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor);
int obtenerLruS(tLruS *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor);
int borrarLruS(tLruS *pLru, const void *clave, size_t tamClave);
void vaciarLruS(tLruS *pLru);

#endif
