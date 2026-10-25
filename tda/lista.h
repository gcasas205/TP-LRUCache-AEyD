#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#include <stddef.h>
#include "comun.h"

typedef struct sNodo
{
    void *info;
    size_t tamInfo;
    struct sNodo *sig;
}tNodo;

typedef tNodo *tLista;

void crearLista(tLista *pLista);
void recorrerLista(const tLista *pLista, tAccion accion, void *contexto);

int vaciarLista(tLista *pLista);
int listaLlena(const tLista *pLista, size_t tamInfo);
int listaVacia(const tLista *pLista);
int insPrinLista(tLista *pLista, const void *info, size_t tamInfo);
int buscarLista(const tLista *pLista, const void *clave, void *info, size_t tamInfo, tCmp cmp);
int llevarAlPrinLista(tLista *pLista, const void *clave, tCmp cmp);
int verPrimeroLista(const tLista *pLista, void *info, size_t tamInfo);
int sacarPorClaveLista(tLista *pLista, const void *clave, void *info, size_t tamInfo, tCmp cmp);
int sacarUltLista(tLista *pLista, void *info, size_t tamInfo);



#endif // LISTA_H_INCLUDED
