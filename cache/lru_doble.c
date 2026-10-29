#include "lru_doble.h"
#include <stdlib.h>
#include <string.h>

// Busca la clave en el arbol y deja en *pos la posicion del nodo de la lista.
// indice es un buffer de tamClave + sizeof(tPosListaD) que pide quien llama.
static int buscarPosLruD(const tLruD *pLru, const void *clave, size_t tamClave, void *indice,
                         tPosListaD *pos)
{
    if(buscarArbol(&pLru->indice, clave, indice, tamClave + sizeof(tPosListaD), pLru->cmp) != EXITO)
        return NO_ENCONTRADO;

    // memcpy y no cast: (char*)indice + tamClave puede no estar alineado para un puntero
    memcpy(pos, (char*)indice + tamClave, sizeof(tPosListaD));

    return EXITO;
}

static void contarNodoLruD(void *info, void *contexto)
{
    (void)info;
    (*(unsigned*)contexto)++;
}

int crearLruD(tLruD *pLru, unsigned capacidad, tCmp cmp)
{
    if(!cmp || !capacidad)
        return ERROR_PARAMETRO;

    crearListaD(&pLru->lista);
    crearArbol(&pLru->indice);
    pLru->capacidad = capacidad;
    pLru->cantidad = 0;
    pLru->cmp = cmp;

    return EXITO;
}

int agregarLruD(tLruD *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    void *bloque = NULL;
    void *indice;
    tPosListaD pos;
    int ret;

    indice = malloc(tamClave + sizeof(tPosListaD));
    if(!indice)
        return ERROR_MALLOC;

    if(crearEntrada(&bloque, clave, tamClave, valor, tamValor) == ERROR_MALLOC)
    {
        free(indice);
        return ERROR_MALLOC;
    }

    if(buscarPosLruD(pLru, clave, tamClave, indice, &pos) == EXITO)
    {
        // clave existente: se reemplaza en el lugar, la posicion no cambia y el arbol no se toca
        ret = actualizarInfoListaD(pos, bloque, tamClave + tamValor);
        if(ret == EXITO)
            moverAlPrinListaD(&pLru->lista, pos);
    }
    else
    {
        // clave nueva con la cache llena: se descarta el menos reciente antes de insertar
        if(pLru->cantidad == pLru->capacidad)
        {
            verUltListaD(&pLru->lista, indice, tamClave);
            eliminarArbol(&pLru->indice, indice, pLru->cmp);
            sacarUltListaD(&pLru->lista, NULL, 0);
            pLru->cantidad--;
        }

        // primero la lista, porque el arbol necesita la posicion
        ret = insPrinListaD(&pLru->lista, bloque, tamClave + tamValor, &pos);
        if(ret == EXITO)
        {
            memcpy(indice, clave, tamClave);
            memcpy((char*)indice + tamClave, &pos, sizeof(tPosListaD));
            ret = insArbol(&pLru->indice, indice, tamClave + sizeof(tPosListaD), pLru->cmp);
            if(ret == EXITO)
                pLru->cantidad++;
            else
                sacarNodoListaD(&pLru->lista, pos, NULL, 0);
        }
    }

    free(bloque);
    free(indice);
    return ret;
}

int obtenerLruD(tLruD *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    void *indice;
    void *temp;
    tPosListaD pos;

    indice = malloc(tamClave + sizeof(tPosListaD));
    if(!indice)
        return ERROR_MALLOC;

    if(buscarPosLruD(pLru, clave, tamClave, indice, &pos) == NO_ENCONTRADO)
    {
        free(indice);
        return NO_ENCONTRADO;
    }
    free(indice);

    // el buffer se pide antes de mover: si falla, la cache queda como estaba
    temp = malloc(tamClave + tamValor);
    if(!temp)
        return ERROR_MALLOC;

    moverAlPrinListaD(&pLru->lista, pos);

    // precarga igual que obtenerLruS: si el valor guardado es mas corto que tamValor,
    // el resto del buffer del llamador queda como estaba
    memcpy((char*)temp + tamClave, valor, tamValor);
    verInfoListaD(pos, temp, tamClave + tamValor);
    copiarValorEntrada(temp, tamClave + tamValor, tamClave, valor, tamValor);

    free(temp);
    return EXITO;
}

int borrarLruD(tLruD *pLru, const void *clave, size_t tamClave)
{
    void *indice;
    tPosListaD pos;

    indice = malloc(tamClave + sizeof(tPosListaD));
    if(!indice)
        return ERROR_MALLOC;

    // la posicion se lee antes de eliminar del arbol: despues ya no hay de donde sacarla
    if(buscarPosLruD(pLru, clave, tamClave, indice, &pos) == NO_ENCONTRADO)
    {
        free(indice);
        return NO_ENCONTRADO;
    }

    eliminarArbol(&pLru->indice, clave, pLru->cmp);
    sacarNodoListaD(&pLru->lista, pos, NULL, 0);
    pLru->cantidad--;

    free(indice);
    return EXITO;
}

void vaciarLruD(tLruD *pLru)
{
    // el orden da igual: el arbol nunca libera nodos de la lista
    vaciarArbol(&pLru->indice);
    vaciarListaD(&pLru->lista);
    pLru->cantidad = 0;
}

int alturaIndiceLruD(const tLruD *pLru)
{
    return alturaArbol(&pLru->indice);
}

int verificarLruD(const tLruD *pLru)
{
    unsigned nodosLista = 0;

    recorrerListaD(&pLru->lista, contarNodoLruD, &nodosLista);

    return (unsigned)cantNodosArbol(&pLru->indice) == nodosLista &&
           nodosLista == pLru->cantidad &&
           pLru->cantidad <= pLru->capacidad;
}
