#ifndef LRU_DOBLE_H_INCLUDED
#define LRU_DOBLE_H_INCLUDED

#include <stddef.h>
#include "comun.h"
#include "lista_doble.h"
#include "arbol.h"
#include "entrada.h"

// La lista es duena de los datos ([clave][valor]) y lleva el orden de uso.
// El arbol es dueno solo de su indice: cada info es [clave][tPosListaD], una copia de la clave
// y la posicion del nodo de la lista.
typedef struct
{
    tListaD lista;
    tArbol indice;
    unsigned capacidad;
    unsigned cantidad;
    tCmp cmp;
}tLruD;

int crearLruD(tLruD *pLru, unsigned capacidad, tCmp cmp);
int agregarLruD(tLruD *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor);
int obtenerLruD(tLruD *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor);
int borrarLruD(tLruD *pLru, const void *clave, size_t tamClave);
void vaciarLruD(tLruD *pLru);
int alturaIndiceLruD(const tLruD *pLru);
int verificarLruD(const tLruD *pLru);

#endif
