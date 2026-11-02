#ifndef VERIFICAR_H_INCLUDED
#define VERIFICAR_H_INCLUDED

void verificarCond(int condicion, const char *id, const char *descripcion,
                   const char *esperado, const char *obtenido);
void verificarInt(int esperado, int obtenido, const char *id, const char *descripcion);
void verificarTexto(const char *esperado, const char *obtenido, const char *id,
                    const char *descripcion);

const char *nombreRetorno(int ret);

int cantidadFallas(void);

int imprimirResumen(void);

#endif // VERIFICAR_H_INCLUDED
