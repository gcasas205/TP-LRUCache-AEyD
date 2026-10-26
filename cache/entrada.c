#include "entrada.h"
#include <stdlib.h>
#include <string.h>

int crearEntrada(void **bloque, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    *bloque = malloc(tamClave + tamValor);
    if(!(*bloque))
        return ERROR_MALLOC;

    memcpy(*bloque, clave, tamClave);
    memcpy((char*)(*bloque) + tamClave, valor, tamValor);

    return EXITO;
}

size_t copiarValorEntrada(const void *bloque, size_t tamBloque, size_t tamClave, void *valor, size_t tamValor)
{
    memcpy(valor, (const char*)bloque + tamClave, MIN(tamBloque - tamClave, tamValor));

    return MIN(tamBloque - tamClave, tamValor);
}
