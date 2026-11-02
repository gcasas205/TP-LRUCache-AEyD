#include "usuario.h"

int leerUsuario(FILE *fp, tUsuario *usuario)
{
    if(fread(usuario, sizeof(tUsuario), 1, fp) != 1)
        return FIN_ARCHIVO;

    return EXITO;
}

int escribirUsuario(FILE *fp, const tUsuario *usuario)
{
    if(fwrite(usuario, sizeof(tUsuario), 1, fp) != 1)
        return ERROR_ARCHIVO;

    return EXITO;
}
