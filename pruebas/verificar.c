#include <stdio.h>
#include <string.h>

#include "comun.h"
#include "verificar.h"

static int cantOk = 0;
static int cantFallas = 0;

void verificarCond(int condicion, const char *id, const char *descripcion,
                   const char *esperado, const char *obtenido)
{
    if(condicion)
    {
        printf("[OK] %s %s\n", id, descripcion);
        cantOk++;
    }
    else
    {
        printf("[FALLA] %s %s (esperado %s, obtenido %s)\n", id, descripcion, esperado, obtenido);
        cantFallas++;
    }
}

void verificarInt(int esperado, int obtenido, const char *id, const char *descripcion)
{
    char txtEsperado[16];
    char txtObtenido[16];

    sprintf(txtEsperado, "%d", esperado);
    sprintf(txtObtenido, "%d", obtenido);
    verificarCond(esperado == obtenido, id, descripcion, txtEsperado, txtObtenido);
}

void verificarTexto(const char *esperado, const char *obtenido, const char *id,
                    const char *descripcion)
{
    verificarCond(strcmp(esperado, obtenido) == 0, id, descripcion, esperado, obtenido);
}

const char *nombreRetorno(int ret)
{
    switch(ret)
    {
        case EXITO:
            return "EXITO";
        case ERROR_MALLOC:
            return "ERROR_MALLOC";
        case NO_ENCONTRADO:
            return "NO_ENCONTRADO";
        case DUPLICADO:
            return "DUPLICADO";
        case VACIA:
            return "VACIA";
        case ERROR_ARCHIVO:
            return "ERROR_ARCHIVO";
        case FIN_ARCHIVO:
            return "FIN_ARCHIVO";
        case ERROR_PARAMETRO:
            return "ERROR_PARAMETRO";
        default:
            return "DESCONOCIDO";
    }
}

int cantidadFallas(void)
{
    return cantFallas;
}

int imprimirResumen(void)
{
    printf("RESUMEN: %d OK, %d FALLA\n", cantOk, cantFallas);
    return cantFallas;
}
