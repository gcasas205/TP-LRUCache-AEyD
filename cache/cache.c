#include "cache.h"
#include "lru_simple.h"
#include "lru_doble.h"
#include <stdlib.h>
#include <string.h>

static int agregarAdapS(void *impl, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    return agregarLruS((tLruS*)impl, clave, tamClave, valor, tamValor);
}

static int obtenerAdapS(void *impl, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    return obtenerLruS((tLruS*)impl, clave, tamClave, valor, tamValor);
}

static int borrarAdapS(void *impl, const void *clave, size_t tamClave)
{
    return borrarLruS((tLruS*)impl, clave, tamClave);
}

static void vaciarAdapS(void *impl)
{
    vaciarLruS((tLruS*)impl);
}

static int alturaAdapS(const void *impl)
{
    (void)impl;
    return -1;
}

static int agregarAdapD(void *impl, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    return agregarLruD((tLruD*)impl, clave, tamClave, valor, tamValor);
}

static int obtenerAdapD(void *impl, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    return obtenerLruD((tLruD*)impl, clave, tamClave, valor, tamValor);
}

static int borrarAdapD(void *impl, const void *clave, size_t tamClave)
{
    return borrarLruD((tLruD*)impl, clave, tamClave);
}

static void vaciarAdapD(void *impl)
{
    vaciarLruD((tLruD*)impl);
}

static int alturaAdapD(const void *impl)
{
    return alturaIndiceLruD((const tLruD*)impl);
}

static int agregarAdapSinCache(void *impl, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    (void)impl;
    (void)clave;
    (void)tamClave;
    (void)valor;
    (void)tamValor;
    return EXITO;
}

static int obtenerAdapSinCache(void *impl, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    (void)impl;
    (void)clave;
    (void)tamClave;
    (void)valor;
    (void)tamValor;
    return NO_ENCONTRADO;
}

static int borrarAdapSinCache(void *impl, const void *clave, size_t tamClave)
{
    (void)impl;
    (void)clave;
    (void)tamClave;
    return NO_ENCONTRADO;
}

static void vaciarAdapSinCache(void *impl)
{
    (void)impl;
}

int crearCacheSimple(tCache *pCache, unsigned capacidad, tCmp cmp)
{
    int ret;
    tLruS *lruS = (tLruS*)malloc(sizeof(tLruS));
    if(!lruS)
        return ERROR_MALLOC;

    ret = crearLruS(lruS, capacidad, cmp);
    if(ret != EXITO)
    {
        free(lruS);
        return ret;
    }

    pCache->impl = lruS;
    pCache->agregar = agregarAdapS;
    pCache->obtener = obtenerAdapS;
    pCache->borrar = borrarAdapS;
    pCache->vaciar = vaciarAdapS;
    pCache->altura = alturaAdapS;
    pCache->nombre = "Lista simple";

    return EXITO;
}

int crearCacheDoble(tCache *pCache, unsigned capacidad, tCmp cmp)
{
    int ret;
    tLruD *lruD = (tLruD*)malloc(sizeof(tLruD));
    if(!lruD)
        return ERROR_MALLOC;

    ret = crearLruD(lruD, capacidad, cmp);
    if(ret != EXITO)
    {
        free(lruD);
        return ret;
    }

    pCache->impl = lruD;
    pCache->agregar = agregarAdapD;
    pCache->obtener = obtenerAdapD;
    pCache->borrar = borrarAdapD;
    pCache->vaciar = vaciarAdapD;
    pCache->altura = alturaAdapD;
    pCache->nombre = "Lista doble + ABB";

    return EXITO;
}

void crearSinCache(tCache *pCache)
{
    pCache->impl = NULL;
    pCache->agregar = agregarAdapSinCache;
    pCache->obtener = obtenerAdapSinCache;
    pCache->borrar = borrarAdapSinCache;
    pCache->vaciar = vaciarAdapSinCache;
    pCache->altura = alturaAdapS;
    pCache->nombre = "Sin cache";
}

int agregarCache(tCache *pCache, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    return pCache->agregar(pCache->impl, clave, tamClave, valor, tamValor);
}

int obtenerCache(tCache *pCache, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    return pCache->obtener(pCache->impl, clave, tamClave, valor, tamValor);
}

int borrarCache(tCache *pCache, const void *clave, size_t tamClave)
{
    return pCache->borrar(pCache->impl, clave, tamClave);
}

void vaciarCache(tCache *pCache)
{
    pCache->vaciar(pCache->impl);
    free(pCache->impl);
    crearSinCache(pCache);
}

const char *nombreCache(const tCache *pCache)
{
    return pCache->nombre;
}

int alturaIndiceCache(const tCache *pCache)
{
    return pCache->altura(pCache->impl);
}
