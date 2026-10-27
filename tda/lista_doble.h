#ifndef LISTA_DOBLE_H_INCLUDED
#define LISTA_DOBLE_H_INCLUDED

#include <stddef.h>
#include "comun.h"

typedef struct sNodoD
{
    void *info;
    size_t tamInfo;
    struct sNodoD *ant, *sig;
}tNodoD;

typedef struct
{
    tNodoD *pri, *ult;
}tListaD;

typedef void *tPosListaD;

void crearListaD(tListaD *pLista);
int listaVaciaD(const tListaD *pLista);
int insPrinListaD(tListaD *pLista, const void *info, size_t tamInfo, tPosListaD *pos);
size_t verInfoListaD(tPosListaD pos, void *info, size_t tamInfo);
int actualizarInfoListaD(tPosListaD pos, const void *info, size_t tamInfo);
void moverAlPrinListaD(tListaD *pLista, tPosListaD pos);
int sacarNodoListaD(tListaD *pLista, tPosListaD pos, void *info, size_t tamInfo);
int verUltListaD(const tListaD *pLista, void *info, size_t tamInfo);
int sacarUltListaD(tListaD *pLista, void *info, size_t tamInfo);
void recorrerListaD(const tListaD *pLista, tAccion accion, void *contexto);
int vaciarListaD(tListaD *pLista);


#endif
