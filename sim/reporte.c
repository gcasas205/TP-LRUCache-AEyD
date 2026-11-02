#include <string.h>
#include "reporte.h"

#define TAM_CAMPO 32

static void textoEntero(char *destino, long valor, long sinValor)
{
    if(valor == sinValor)
        strcpy(destino, "-");
    else
        sprintf(destino, "%ld", valor);
}

static void textoDecimal(char *destino, const char *formato, double valor)
{
    char *punto;

    sprintf(destino, formato, valor);
    if((punto = strchr(destino, '.')) != NULL)
        *punto = ',';
}

void imprimirEncabezado(void)
{
    printf("%-9s %-12s %-18s %6s %10s %12s %12s %9s %9s %9s %6s\n",
           "Acceso", "Lote", "Cache", "Cap.", "Total (s)", "Usuario (s)", "Posteo (s)",
           "Aciertos", "Fallos", "Lecturas", "Altura");
}

void imprimirFila(const char *acceso, const char *lote, const tCache *pCache, unsigned capacidad,
                  int altura, const tMetricas *pMetricas)
{
    char txtCapacidad[TAM_CAMPO];
    char txtAltura[TAM_CAMPO];

    textoEntero(txtCapacidad, (long)capacidad, 0L);
    textoEntero(txtAltura, (long)altura, -1L);
    printf("%-9s %-12s %-18s %6s %10.3f %12.3e %12.3e %9ld %9ld %9ld %6s\n",
           acceso, lote, nombreCache(pCache), txtCapacidad, pMetricas->segundos,
           promedioPorUsuario(pMetricas), promedioPorPosteo(pMetricas),
           pMetricas->aciertos, pMetricas->fallos, pMetricas->lecturas, txtAltura);
}

void exportarEncabezadoCsv(FILE *fp)
{
    fprintf(fp, "acceso;lote;cache;capacidad;total_s;por_usuario_s;por_posteo_s;"
                "aciertos;fallos;lecturas;altura\n");
}

void exportarCsv(FILE *fp, const char *acceso, const char *lote, const tCache *pCache,
                 unsigned capacidad, int altura, const tMetricas *pMetricas)
{
    char txtCapacidad[TAM_CAMPO];
    char txtAltura[TAM_CAMPO];
    char txtTotal[TAM_CAMPO];
    char txtUsuario[TAM_CAMPO];
    char txtPosteo[TAM_CAMPO];

    textoEntero(txtCapacidad, (long)capacidad, 0L);
    textoEntero(txtAltura, (long)altura, -1L);
    textoDecimal(txtTotal, "%.6f", pMetricas->segundos);
    textoDecimal(txtUsuario, "%.9f", promedioPorUsuario(pMetricas));
    textoDecimal(txtPosteo, "%.12f", promedioPorPosteo(pMetricas));
    fprintf(fp, "%s;%s;%s;%s;%s;%s;%s;%ld;%ld;%ld;%s\n",
            acceso, lote, nombreCache(pCache), txtCapacidad, txtTotal, txtUsuario, txtPosteo,
            pMetricas->aciertos, pMetricas->fallos, pMetricas->lecturas, txtAltura);
}
