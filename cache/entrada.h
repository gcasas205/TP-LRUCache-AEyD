#ifndef ENTRADA_H_INCLUDED
#define ENTRADA_H_INCLUDED

#include <stddef.h>
#include "comun.h"

int crearEntrada(void **bloque, const void *clave, size_t tamClave, const void *valor, size_t tamValor);
size_t copiarValorEntrada(const void *bloque, size_t tamBloque, size_t tamClave, void *valor, size_t tamValor);

#endif
