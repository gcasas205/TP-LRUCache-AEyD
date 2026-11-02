#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "generador.h"

#define RUTA_POSTEOS "datos/posteos.dat"
#define RUTA_CONCENTRADO "datos/lote_concentrado.dat"
#define RUTA_DISPERSO "datos/lote_disperso.dat"
#define RUTA_ORDENADO "datos/lote_ordenado.dat"

static int leerEntero(const char *texto, long *valor)
{
    char *fin;

    errno = 0;
    *valor = strtol(texto, &fin, 10);
    return *texto != '\0' && *fin == '\0' && errno != ERANGE;
}

static int mostrarUso(const char *programa)
{
    fprintf(stderr, "Uso: %s <semilla> <usuarios> <grupo_concentrado>\n", programa);
    fprintf(stderr, "  usuarios entre %d y %d; grupo_concentrado entre 1 y %d\n", MIN_USUARIOS, MAX_USUARIOS,
            CANT_POSTEOS);
    fprintf(stderr, "  Ejemplo: %s 2026 50000 300\n", programa);
    return 1;
}

int main(int argc, char *argv[])
{
    long semilla;
    long usuarios;
    long grupo;
    int cantUsuarios;
    int grupoConcentrado;

    if(argc != 4 || !leerEntero(argv[1], &semilla) || !leerEntero(argv[2], &usuarios) ||
       !leerEntero(argv[3], &grupo))
        return mostrarUso(argv[0]);

    if(usuarios < MIN_USUARIOS || usuarios > MAX_USUARIOS || grupo < 1 || grupo > CANT_POSTEOS)
        return mostrarUso(argv[0]);

    cantUsuarios = (int)usuarios;
    grupoConcentrado = (int)grupo;
    srand((unsigned)semilla);

    printf("Generando posteos (%d)...\n", CANT_POSTEOS);
    if(generarPosteos(RUTA_POSTEOS, CANT_POSTEOS) != EXITO)
    {
        fprintf(stderr, "No se pudo escribir %s (existe la carpeta datos/?)\n", RUTA_POSTEOS);
        return 1;
    }

    printf("Generando lote concentrado (%d usuarios, G = %d)...\n", cantUsuarios, grupoConcentrado);
    if(generarLote(RUTA_CONCENTRADO, cantUsuarios, feedConcentrado, &grupoConcentrado) != EXITO)
    {
        fprintf(stderr, "No se pudo escribir %s\n", RUTA_CONCENTRADO);
        return 1;
    }

    printf("Generando lote disperso (%d usuarios)...\n", cantUsuarios);
    if(generarLote(RUTA_DISPERSO, cantUsuarios, feedDisperso, NULL) != EXITO)
    {
        fprintf(stderr, "No se pudo escribir %s\n", RUTA_DISPERSO);
        return 1;
    }

    printf("Generando lote ordenado (%d usuarios)...\n", cantUsuarios);
    if(generarLote(RUTA_ORDENADO, cantUsuarios, feedOrdenado, &cantUsuarios) != EXITO)
    {
        fprintf(stderr, "No se pudo escribir %s\n", RUTA_ORDENADO);
        return 1;
    }

    printf("Listo. Semilla %ld, %d usuarios, G = %d.\n", semilla, cantUsuarios, grupoConcentrado);
    return 0;
}
