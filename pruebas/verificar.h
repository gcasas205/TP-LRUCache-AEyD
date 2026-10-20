#ifndef VERIFICAR_H_INCLUDED
#define VERIFICAR_H_INCLUDED

// Imprime "[OK] <id> <descripcion>" o "[FALLA] <id> <descripcion> (esperado ..., obtenido ...)"
// y lleva la cuenta de OK y FALLA de toda la corrida. La usan las pruebas de las 4 specs.

void verificarCond(int condicion, const char *id, const char *descripcion,
                   const char *esperado, const char *obtenido);
void verificarInt(int esperado, int obtenido, const char *id, const char *descripcion);
void verificarTexto(const char *esperado, const char *obtenido, const char *id,
                    const char *descripcion);

// Nombre simbolico de un codigo de retorno de comun.h ("EXITO", "NO_ENCONTRADO", ...)
const char *nombreRetorno(int ret);

// Cantidad de FALLA acumuladas hasta ahora (sirve para que cada pruebasNNN devuelva las suyas)
int cantidadFallas(void);

// Imprime "RESUMEN: <n> OK, <m> FALLA" y devuelve la cantidad de FALLA
int imprimirResumen(void);

#endif // VERIFICAR_H_INCLUDED
