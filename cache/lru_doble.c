#include "lru_doble.h"
#include "entrada.h"
#include <stdlib.h>
#include <string.h>

static void contarNodo(void *info, void *contexto)
{
    (void)info;
    (*((unsigned*)contexto))++;
}

int crearLruD(tLruD *pLru, unsigned capacidad, tCmp cmp)
{
    if(!cmp || !capacidad)
        return ERROR_PARAMETRO;

    crearListaD(&pLru->lista);
    crearArbol(&pLru->indice);
    pLru->capacidad = capacidad;
    pLru->cmp = cmp;
    pLru->cantidad = 0;

    return EXITO;
}

int agregarLruD(tLruD *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    void *bloque = NULL, *busqueda, *indice;
    tPosListaD pos;
    int estaba, ret;
    if(crearEntrada(&bloque, clave, tamClave, valor, tamValor) == ERROR_MALLOC)
        return ERROR_MALLOC;

    busqueda = malloc(tamClave + sizeof(tPosListaD));
    if(!busqueda)
    {
        free(bloque);
        return ERROR_MALLOC;
    }

    estaba = buscarArbol(&pLru->indice, clave, busqueda, tamClave + sizeof(tPosListaD), pLru->cmp);

    if(estaba == EXITO)
    {
        memcpy(&pos, (char*)busqueda + tamClave, sizeof(tPosListaD));
        estaba = actualizarInfoListaD(pos, bloque, tamClave + tamValor);
        if(estaba == EXITO)
            moverAlPrinListaD(&pLru->lista, pos);
        free(bloque);
        free(busqueda);
        return estaba;
    }

    if(pLru->cantidad == pLru->capacidad)
    {
        verUltListaD(&pLru->lista, busqueda, tamClave);
        eliminarArbol(&pLru->indice, busqueda, pLru->cmp);
        sacarUltListaD(&pLru->lista, NULL, 0);
        pLru->cantidad--;
    }

    ret = insPrinListaD(&pLru->lista, bloque, tamClave + tamValor, &pos);
    free(bloque);
    if(ret != EXITO)
    {
        free(busqueda);
        return ERROR_MALLOC;
    }

    if(crearEntrada(&indice, clave, tamClave, &pos, sizeof(tPosListaD)) != EXITO)
    {
        sacarNodoListaD(&pLru->lista, pos, NULL, 0);
        free(busqueda);
        return ERROR_MALLOC;
    }

    ret = insArbol(&pLru->indice, indice, tamClave + sizeof(tPosListaD), pLru->cmp);
    free(indice);

    if(ret != EXITO)
    {
        sacarNodoListaD(&pLru->lista, pos, NULL, 0);
        free(busqueda);
        return ERROR_MALLOC;
    }

    pLru->cantidad++;
    free(busqueda);
    return EXITO;
}

int obtenerLruD(tLruD *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    void *temp, *busqueda;
    tPosListaD pos;
    int estaba;
    size_t tamElem;

    busqueda = malloc(tamClave + sizeof(tPosListaD));
    if(!busqueda)
        return ERROR_MALLOC;

    estaba = buscarArbol(&pLru->indice, clave, busqueda, tamClave + sizeof(tPosListaD), pLru->cmp);
    if(estaba != EXITO)
    {
        free(busqueda);
        return NO_ENCONTRADO;
    }

    memcpy(&pos, (char*)busqueda + tamClave, sizeof(tPosListaD));
    free(busqueda);

    moverAlPrinListaD(&pLru->lista, pos);

    temp = malloc(tamClave + tamValor);
    if(!temp)
        return ERROR_MALLOC;

    tamElem = verInfoListaD(pos, temp, tamClave + tamValor);
    copiarValorEntrada(temp, tamElem, tamClave, valor, tamValor);


    free(temp);

    return EXITO;
}

int borrarLruD(tLruD *pLru, const void *clave, size_t tamClave)
{
    void *busqueda;
    tPosListaD pos;
    int estaba;

    busqueda = malloc(tamClave + sizeof(tPosListaD));
    if(!busqueda)
        return ERROR_MALLOC;

    estaba = buscarArbol(&pLru->indice, clave, busqueda, tamClave + sizeof(tPosListaD), pLru->cmp);

    if(estaba != EXITO)
    {
        free(busqueda);
        return NO_ENCONTRADO;
    }

    memcpy(&pos, (char*)busqueda + tamClave, sizeof(tPosListaD));
    free(busqueda);

    eliminarArbol(&pLru->indice, clave, pLru->cmp);
    sacarNodoListaD(&pLru->lista, pos, NULL, 0);
    pLru->cantidad--;

    return EXITO;
}

void vaciarLruD(tLruD *pLru)
{
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
    unsigned contadorNodosLista = 0;
    recorrerListaD(&pLru->lista, contarNodo, &contadorNodosLista);
    if((unsigned)cantNodosArbol(&pLru->indice) == contadorNodosLista && contadorNodosLista == pLru->cantidad && pLru->cantidad <= pLru->capacidad)
        return 1;
    return 0;
}


