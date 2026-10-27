#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

#include <stddef.h>
#include "comun.h"

typedef int (*tAgregarImpl)(void *impl, const void *clave, size_t tamClave, const void *valor, size_t tamValor);
typedef int (*tObtenerImpl)(void *impl, const void *clave, size_t tamClave, void *valor, size_t tamValor);
typedef int (*tBorrarImpl)(void *impl, const void *clave, size_t tamClave);
typedef void (*tVaciarImpl)(void *impl);
typedef int (*tAlturaImpl)(const void *impl);

typedef struct
{
    void *impl;
    tAgregarImpl agregar;
    tObtenerImpl obtener;
    tBorrarImpl borrar;
    tVaciarImpl vaciar;
    tAlturaImpl altura;
    const char *nombre;
}tCache;

int crearCacheSimple(tCache *pCache, unsigned capacidad, tCmp cmp);
int crearCacheDoble(tCache *pCache, unsigned capacidad, tCmp cmp);
void crearSinCache(tCache *pCache);

int agregarCache(tCache *pCache, const void *clave, size_t tamClave, const void *valor, size_t tamValor);
int obtenerCache(tCache *pCache, const void *clave, size_t tamClave, void *valor, size_t tamValor);
int borrarCache(tCache *pCache, const void *clave, size_t tamClave);
void vaciarCache(tCache *pCache);

const char *nombreCache(const tCache *pCache);
int alturaIndiceCache(const tCache *pCache);

#endif
