#ifndef SIMULADOR_H_INCLUDED
#define SIMULADOR_H_INCLUDED

#include "comun.h"
#include "cache.h"
#include "posteo.h"
#include "metricas.h"

int simular(const char *rutaPosteos, const char *rutaLote, tCache *pCache, tLectorPosteo lector,
            tMetricas *pMetricas);

#endif // SIMULADOR_H_INCLUDED
