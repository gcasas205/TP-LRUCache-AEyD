#ifndef POSTEO_H_INCLUDED
#define POSTEO_H_INCLUDED

#include <stdio.h>
#include "comun.h"

#define CANT_POSTEOS 50000
#define TAM_TEXTO_POSTEO 141

typedef struct
{
    int id;
    char texto[TAM_TEXTO_POSTEO];
} tPosteo;

typedef int (*tLectorPosteo)(FILE *fp, int id, tPosteo *posteo);

int leerPosteoPorId(FILE *fp, int id, tPosteo *posteo);
int leerPosteoConSeek(FILE *fp, int id, tPosteo *posteo);
int escribirPosteo(FILE *fp, const tPosteo *posteo);
int cmpIdPosteo(const void *a, const void *b);

#endif // POSTEO_H_INCLUDED
