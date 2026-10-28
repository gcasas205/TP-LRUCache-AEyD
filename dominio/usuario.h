#ifndef USUARIO_H_INCLUDED
#define USUARIO_H_INCLUDED

#include <stdio.h>
#include "comun.h"

#define TAM_NOMBRE_USUARIO 15
#define CANT_FEED 20

typedef struct
{
    char usuario[TAM_NOMBRE_USUARIO];
    int feed[CANT_FEED];
} tUsuario;

int leerUsuario(FILE *fp, tUsuario *usuario);
int escribirUsuario(FILE *fp, const tUsuario *usuario);

#endif // USUARIO_H_INCLUDED
