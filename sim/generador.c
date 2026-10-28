#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generador.h"

static int aleatorioEntre(int min, int max)
{
    long r;

    r = (long)rand() * (RAND_MAX + 1L) + rand();
    return min + (int)(r % (max - min + 1));
}


int generarPosteos(const char *ruta, int cantidad)
{
    FILE *fp;
    tPosteo posteo;
    int i;

    if(!(fp = fopen(ruta, "wb")))
        return ERROR_ARCHIVO;

    for(i = 0; i < cantidad; i++)
    {
        memset(&posteo, 0, sizeof(posteo));
        posteo.id = i;
        sprintf(posteo.texto, "Posteo numero %d", posteo.id);
        if(escribirPosteo(fp, &posteo) != EXITO)
        {
            fclose(fp);
            return ERROR_ARCHIVO;
        }
    }

    fclose(fp);
    return EXITO;
}

int generarLote(const char *ruta, int cantUsuarios, tEstrategiaFeed estrategia, void *contexto)
{
    FILE *fp;
    tUsuario usuario;
    int n;

    if(cantUsuarios < MIN_USUARIOS)
        return ERROR_PARAMETRO;

    if(!(fp = fopen(ruta, "wb")))
        return ERROR_ARCHIVO;

    for(n = 0; n < cantUsuarios; n++)
    {
        memset(&usuario, 0, sizeof(usuario));
        sprintf(usuario.usuario, "usuario%04u", (unsigned)(n + 1) % 10000000u);
        estrategia(usuario.feed, n, contexto);
        if(escribirUsuario(fp, &usuario) != EXITO)
        {
            fclose(fp);
            return ERROR_ARCHIVO;
        }
    }

    fclose(fp);
    return EXITO;
}

void feedConcentrado(int feed[CANT_FEED], int nUsuario, void *contexto)
{
    int grupo = *(int*)contexto;
    int i;

    (void)nUsuario;
    for(i = 0; i < CANT_FEED; i++)
        feed[i] = aleatorioEntre(0, grupo - 1);
}

void feedDisperso(int feed[CANT_FEED], int nUsuario, void *contexto)
{
    int i;

    (void)nUsuario;
    (void)contexto;
    for(i = 0; i < CANT_FEED; i++)
        feed[i] = aleatorioEntre(0, CANT_POSTEOS - 1);
}

void feedOrdenado(int feed[CANT_FEED], int nUsuario, void *contexto)
{
    int usuarios = *(int*)contexto;
    double j;
    int i;

    for(i = 0; i < CANT_FEED; i++)
    {
        j = (double)nUsuario * CANT_FEED + i;
        feed[i] = (int)(j * CANT_POSTEOS / ((double)usuarios * CANT_FEED));
    }
}
