#include <stdio.h>
#include <string.h>
#include "cache.h"
#include "posteo.h"
#include "simulador.h"
#include "reporte.h"

#define RUTA_POSTEOS "datos/posteos.dat"
#define RUTA_CSV "informe/resultados.csv"
#define CANT_LOTES 3
#define CANT_MODOS 2
#define CANT_VARIANTES 2
#define CANT_CAPACIDADES 4

typedef int (*tCrearCache)(tCache *pCache, unsigned capacidad, tCmp cmp);

typedef struct
{
    const char *argumento;
    const char *nombre;
    tLectorPosteo lector;
} tModo;

typedef struct
{
    const char *nombre;
    const char *ruta;
} tLote;

static int existeArchivo(const char *ruta)
{
    FILE *fp = fopen(ruta, "rb");

    if(!fp)
        return 0;
    fclose(fp);
    return 1;
}

static void informarFallo(const char *motivo, const tModo *modo, const tLote *lote, const char *cache,
                          unsigned capacidad, int ret)
{
    fprintf(stderr, "ERROR: %s (%s, %s, %s, capacidad %u): codigo %d\n", motivo, modo->nombre,
            lote->nombre, cache, capacidad, ret);
}

static int correr(const tModo *modo, const tLote *lote, tCache *pCache, unsigned capacidad, FILE *fpCsv)
{
    tMetricas metricas;
    int altura;
    int ret;

    ret = simular(RUTA_POSTEOS, lote->ruta, pCache, modo->lector, &metricas);
    if(ret == EXITO)
    {
        altura = alturaIndiceCache(pCache);
        imprimirFila(modo->nombre, lote->nombre, pCache, capacidad, altura, &metricas);
        exportarCsv(fpCsv, modo->nombre, lote->nombre, pCache, capacidad, altura, &metricas);
        fflush(stdout);
        fflush(fpCsv);
    }
    else
        informarFallo("fallo la corrida", modo, lote, nombreCache(pCache), capacidad, ret);
    vaciarCache(pCache);
    return ret;
}

int main(int argc, char *argv[])
{
    tModo modos[CANT_MODOS] = {{"directo", "Directo", leerPosteoPorId},
                               {"seek", "Con seek", leerPosteoConSeek}};
    tLote lotes[CANT_LOTES] = {{"Concentrado", "datos/lote_concentrado.dat"},
                               {"Disperso", "datos/lote_disperso.dat"},
                               {"Ordenado", "datos/lote_ordenado.dat"}};
    tCrearCache variantes[CANT_VARIANTES] = {crearCacheSimple, crearCacheDoble};
    unsigned capacidades[CANT_CAPACIDADES] = {10, 100, 1000, 5000};
    const char *nombresVariantes[CANT_VARIANTES] = {"Lista simple", "Lista doble + ABB"};
    const char *pedido = argc > 1 ? argv[1] : "ambos";
    tCache cache;
    FILE *fpCsv;
    int m;
    int l;
    int v;
    int c;
    int ret;
    int fallos = 0;

    if(argc > 2 || (strcmp(pedido, "directo") && strcmp(pedido, "seek") && strcmp(pedido, "ambos")))
    {
        fprintf(stderr, "Uso: %s [directo | seek | ambos]   (sin argumento: ambos)\n", argv[0]);
        return 1;
    }

    if(!existeArchivo(RUTA_POSTEOS))
    {
        fprintf(stderr, "Falta %s: correr primero el generador\n", RUTA_POSTEOS);
        return 1;
    }
    for(l = 0; l < CANT_LOTES; l++)
        if(!existeArchivo(lotes[l].ruta))
        {
            fprintf(stderr, "Falta %s: correr primero el generador\n", lotes[l].ruta);
            return 1;
        }

    if(!(fpCsv = fopen(RUTA_CSV, "w")))
    {
        fprintf(stderr, "No se pudo crear %s (existe la carpeta informe/?)\n", RUTA_CSV);
        return 1;
    }

    exportarEncabezadoCsv(fpCsv);
    imprimirEncabezado();
    for(m = 0; m < CANT_MODOS; m++)
    {
        if(strcmp(pedido, "ambos") && strcmp(pedido, modos[m].argumento))
            continue;
        for(l = 0; l < CANT_LOTES; l++)
        {
            crearSinCache(&cache);
            if(correr(&modos[m], &lotes[l], &cache, 0, fpCsv) != EXITO)
                fallos++;
            for(v = 0; v < CANT_VARIANTES; v++)
                for(c = 0; c < CANT_CAPACIDADES; c++)
                {
                    ret = variantes[v](&cache, capacidades[c], cmpIdPosteo);
                    if(ret != EXITO)
                    {
                        informarFallo("no se pudo crear la cache", &modos[m], &lotes[l],
                                      nombresVariantes[v], capacidades[c], ret);
                        fallos++;
                    }
                    else if(correr(&modos[m], &lotes[l], &cache, capacidades[c], fpCsv) != EXITO)
                        fallos++;
                }
        }
    }

    fclose(fpCsv);
    printf("\nResultados en %s\n", RUTA_CSV);
    if(fallos)
    {
        fprintf(stderr, "%d corridas fallaron: el CSV esta incompleto\n", fallos);
        return 1;
    }
    return 0;
}
