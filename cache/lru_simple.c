#include "lru_simple.h"
#include "entrada.h"
#include <stdlib.h>
#include <string.h>

int crearLruS(tLruS *pLru, unsigned capacidad, tCmp cmp)
{
    if(!cmp || !capacidad)
        return ERROR_PARAMETRO;

    crearLista(&pLru->lista);
    pLru->capacidad = capacidad;
    pLru->cmp = cmp;
    pLru->cantidad = 0;

    return EXITO;
}

int agregarLruS(tLruS *pLru, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    void *bloque = NULL;
    int estaba;
    if(crearEntrada(&bloque, clave, tamClave, valor, tamValor) == ERROR_MALLOC)
        return ERROR_MALLOC;

    estaba = sacarPorClaveLista(&pLru->lista, clave, NULL, 0, pLru->cmp);

    if(insPrinLista(&pLru->lista, bloque, tamClave + tamValor) == ERROR_MALLOC)
    {
        if(estaba == EXITO)
            pLru->cantidad--;
        free(bloque);
        return ERROR_MALLOC;
    }

    if(estaba == NO_ENCONTRADO)
    {
        pLru->cantidad++;
        if(pLru->cantidad > pLru->capacidad)
        {
            sacarUltLista(&pLru->lista, NULL, 0);
            pLru->cantidad--;
        }
    }

    free(bloque);
    return EXITO;
}

int obtenerLruS(tLruS *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    void *temp;
    int encontrado = llevarAlPrinLista(&pLru->lista, clave, pLru->cmp);
    if(encontrado == NO_ENCONTRADO)
        return NO_ENCONTRADO;

    temp = malloc(tamClave + tamValor);
    if(!temp)
        return ERROR_MALLOC;

    memcpy((char*)temp + tamClave, valor, tamValor);

    verPrimeroLista(&pLru->lista, temp, tamClave + tamValor);

    copiarValorEntrada(temp, tamClave + tamValor, tamClave, valor, tamValor);

    free(temp);
    return EXITO;
}

int borrarLruS(tLruS *pLru, const void *clave, size_t tamClave)
{
    int encontrado = sacarPorClaveLista(&pLru->lista, clave, NULL, 0, pLru->cmp);
    (void)tamClave;
    if(encontrado == EXITO)
        pLru->cantidad--;

    return encontrado;
}

void vaciarLruS(tLruS* pLru)
{
    vaciarLista(&pLru->lista);
    pLru->cantidad = 0;
}
