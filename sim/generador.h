#ifndef GENERADOR_H_INCLUDED
#define GENERADOR_H_INCLUDED

#include "comun.h"
#include "posteo.h"
#include "usuario.h"

#define MIN_USUARIOS 200
#define MAX_USUARIOS 1000000

typedef void (*tEstrategiaFeed)(int feed[CANT_FEED], int nUsuario, void *contexto);

int generarPosteos(const char *ruta, int cantidad);
int generarLote(const char *ruta, int cantUsuarios, tEstrategiaFeed estrategia, void *contexto);

void feedConcentrado(int feed[CANT_FEED], int nUsuario, void *contexto);
void feedDisperso(int feed[CANT_FEED], int nUsuario, void *contexto);
void feedOrdenado(int feed[CANT_FEED], int nUsuario, void *contexto);

#endif // GENERADOR_H_INCLUDED
