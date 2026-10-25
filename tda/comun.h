#ifndef COMUN_H_INCLUDED
#define COMUN_H_INCLUDED

#define EXITO 0
#define ERROR_MALLOC 1
#define NO_ENCONTRADO 2
#define DUPLICADO 3
#define VACIA 4
#define ERROR_ARCHIVO 5
#define FIN_ARCHIVO 6
#define ERROR_PARAMETRO 7

#define LISTA_DISPONIBLE 0
#define LISTA_VACIA 8
#define LISTA_LLENA 9

#define MIN(a,b) ((a) < (b) ? (a):(b))

typedef int (*tCmp)(const void*, const void*);
typedef void (*tAccion)(void *info, void *contexto);

#endif // COMUN_H_INCLUDED
