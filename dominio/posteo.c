#include "posteo.h"

int leerPosteoPorId(FILE *fp, int id, tPosteo *posteo)
{
    if(id < 0)
        return NO_ENCONTRADO;

    if(fseek(fp, (long)id * (long)sizeof(tPosteo), SEEK_SET))
        return ERROR_ARCHIVO;

    if(fread(posteo, sizeof(tPosteo), 1, fp) != 1)
        return NO_ENCONTRADO;

    return EXITO;
}

int leerPosteoConSeek(FILE *fp, int id, tPosteo *posteo)
{
    if(fseek(fp, 0L, SEEK_END))
        return ERROR_ARCHIVO;

    return leerPosteoPorId(fp, id, posteo);
}

int escribirPosteo(FILE *fp, const tPosteo *posteo)
{
    if(fwrite(posteo, sizeof(tPosteo), 1, fp) != 1)
        return ERROR_ARCHIVO;

    return EXITO;
}

int cmpIdPosteo(const void *a, const void *b)
{
    int x = *(const int*)a;
    int y = *(const int*)b;

    return (x > y) - (x < y);
}
