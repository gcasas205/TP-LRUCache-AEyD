#ifndef REPORTE_H_INCLUDED
#define REPORTE_H_INCLUDED

#include <stdio.h>
#include "cache.h"
#include "metricas.h"

void imprimirEncabezado(void);
void imprimirFila(const char *acceso, const char *lote, const tCache *pCache, unsigned capacidad,
                  int altura, const tMetricas *pMetricas);

void exportarEncabezadoCsv(FILE *fp);
void exportarCsv(FILE *fp, const char *acceso, const char *lote, const tCache *pCache,
                 unsigned capacidad, int altura, const tMetricas *pMetricas);

#endif // REPORTE_H_INCLUDED
