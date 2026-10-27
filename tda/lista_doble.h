#ifndef LISTA_DOBLE_H_INCLUDED
#define LISTA_DOBLE_H_INCLUDED

#include <stddef.h>
#include "comun.h"

typedef struct sNodoD
{
    void *info;
    size_t tamInfo;
    struct sNodoD *ant;
    struct sNodoD *sig;
}tNodoD;

// pri y ult en vez del puntero flotante: la cache necesita los dos extremos sin recorrer
typedef struct
{
    tNodoD *pri;
    tNodoD *ult;
}tListaD;

// Posicion de un nodo, opaca para quien usa la lista (lru_doble la guarda en el arbol).
// Sigue valida mientras el nodo exista: mover o actualizar el info no la cambia.
typedef void* tPosListaD;

void crearListaD(tListaD *pLista);
void moverAlPrinListaD(tListaD *pLista, tPosListaD pos);
void recorrerListaD(const tListaD *pLista, tAccion accion, void *contexto);

int listaVaciaD(const tListaD *pLista);
int insPrinListaD(tListaD *pLista, const void *info, size_t tamInfo, tPosListaD *pos);
size_t verInfoListaD(tPosListaD pos, void *info, size_t tamInfo);
int actualizarInfoListaD(tPosListaD pos, const void *info, size_t tamInfo);
int sacarNodoListaD(tListaD *pLista, tPosListaD pos, void *info, size_t tamInfo);
int verUltListaD(const tListaD *pLista, void *info, size_t tamInfo);
int sacarUltListaD(tListaD *pLista, void *info, size_t tamInfo);
int vaciarListaD(tListaD *pLista);

#endif // LISTA_DOBLE_H_INCLUDED
