#include <stdio.h>
#include "simulador.h"
#include "usuario.h"

static void calentarArchivo(FILE *fpPosteos)
{
    tPosteo posteo;

    while(fread(&posteo, sizeof(tPosteo),1,fpPosteos) == 1)
        ;
    rewind(fpPosteos);
}

static int obtenerPosteo(tCache *pCache, FILE *fpPosteos, tLectorPosteo lector, int id,
                         tPosteo *posteo, tMetricas *pMetricas)
{
    int ret = EXITO;

    if(obtenerCache(pCache, &id, sizeof(id), posteo, sizeof(tPosteo)) == EXITO)
        registrarAcierto(pMetricas);
    else
    {
        registrarFallo(pMetricas);
        ret = lector(fpPosteos, id, posteo);
        registrarLectura(pMetricas);
        if(ret == EXITO)
            agregarCache(pCache, &id, sizeof(id), posteo, sizeof(tPosteo));
    }
    registrarPosteo(pMetricas);
    return ret;
}

int simular(const char *rutaPosteos, const char *rutaLote, tCache *pCache, tLectorPosteo lector,
            tMetricas *pMetricas)
{
    FILE *fpPosteos;
    FILE *fpLote;
    tUsuario usuario;
    tPosteo posteo;
    clock_t inicio;
    int i;

    iniciarMetricas(pMetricas);

    if(!(fpPosteos = fopen(rutaPosteos, "rb")))
        return ERROR_ARCHIVO;
    if(!(fpLote = fopen(rutaLote, "rb")))
    {
        fclose(fpPosteos);
        return ERROR_ARCHIVO;
    }

    calentarArchivo(fpPosteos);
    inicio = clock();

    while(leerUsuario(fpLote, &usuario) == EXITO)
    {
        for(i = 0; i < CANT_FEED; i++)
            obtenerPosteo(pCache, fpPosteos, lector, usuario.feed[i], &posteo, pMetricas);
        registrarUsuario(pMetricas);
    }

    fijarTiempo(pMetricas, inicio, clock());
    fclose(fpPosteos);
    fclose(fpLote);
    return EXITO;
}
