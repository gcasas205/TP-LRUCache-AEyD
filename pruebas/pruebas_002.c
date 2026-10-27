// Pruebas de la spec 002 (LRU con lista doble e indice). Los IDs y valores son los de
// specs/002-lru-doble/casos-prueba.md. Los casos 24, 25, 33 y 34 son de revision y no estan aca.
// Los casos 01 a 23 son los mismos de pruebas_001.c contra las primitivas LruD; cada operacion
// pasa por un envoltorio (crearV, agregarV, ...) que llama a verificarLruD despues (RF-15).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comun.h"
#include "lista_doble.h"
#include "lru_doble.h"
#include "verificar.h"
#include "pruebas.h"

#define TAM_TEXTO 256
#define TAM_BUFFER 32

// Igual en forma al tPosteo de la spec 004, sin depender de dominio/
typedef struct
{
    int id;
    char texto[64];
} tPosteoPrueba;

// ----------------------------------------------------------------- auxiliares

// Compara el int que esta al principio de cada bloque (clave de la entrada)
static int cmpInt(const void *a, const void *b)
{
    return *(const int*)a - *(const int*)b;
}

static int cmpCadena(const void *a, const void *b)
{
    return strcmp((const char*)a, (const char*)b);
}

// Agrega al final de texto lo que sigue, separado por un espacio si texto no esta vacio
static void anexar(char *texto, const char *agregado)
{
    if(*texto)
        strcat(texto, " ");
    strcat(texto, agregado);
}

// Nombre de los estados de lista de comun.h. Va aparte de nombreRetorno porque
// LISTA_DISPONIBLE vale 0, igual que EXITO.
static const char *nombreEstadoLista(int estado)
{
    switch(estado)
    {
        case LISTA_DISPONIBLE:
            return "LISTA_DISPONIBLE";
        case LISTA_VACIA:
            return "LISTA_VACIA";
        case LISTA_LLENA:
            return "LISTA_LLENA";
        default:
            return "DESCONOCIDO";
    }
}

// ------------------------------------------------- consistencia (RF-15)

// Cantidad de veces que verificarLruD no dio 1 desde el ultimo verificarCaso
static int inconsistencias = 0;

static void comprobar(const tLruD *pLru)
{
    if(verificarLruD(pLru) != 1)
        inconsistencias++;
}

static int crearV(tLruD *pLru, unsigned capacidad, tCmp cmp)
{
    int ret = crearLruD(pLru, capacidad, cmp);

    if(ret == EXITO)
        comprobar(pLru);
    return ret;
}

static int agregarV(tLruD *pLru, const void *clave, size_t tamClave, const void *valor,
                    size_t tamValor)
{
    int ret = agregarLruD(pLru, clave, tamClave, valor, tamValor);

    comprobar(pLru);
    return ret;
}

static int obtenerV(tLruD *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    int ret = obtenerLruD(pLru, clave, tamClave, valor, tamValor);

    comprobar(pLru);
    return ret;
}

static int borrarV(tLruD *pLru, const void *clave, size_t tamClave)
{
    int ret = borrarLruD(pLru, clave, tamClave);

    comprobar(pLru);
    return ret;
}

static void vaciarV(tLruD *pLru)
{
    vaciarLruD(pLru);
    comprobar(pLru);
}

// Como verificarTexto, pero si verificarLruD fallo en alguna operacion del caso lo agrega al
// texto obtenido (asi el caso da FALLA) y reinicia la cuenta para el caso siguiente
static void verificarCaso(const char *esperado, const char *obtenido, const char *id,
                          const char *descripcion)
{
    char texto[TAM_TEXTO + 48];

    if(inconsistencias)
    {
        sprintf(texto, "%.255s [verificarLruD fallo %d veces]", obtenido, inconsistencias);
        verificarTexto(esperado, texto, id, descripcion);
    }
    else
        verificarTexto(esperado, obtenido, id, descripcion);
    inconsistencias = 0;
}

// ------------------------------------------------- casos compartidos con la 001

static int agregarTexto(tLruD *pLru, int clave, const char *valor)
{
    return agregarV(pLru, &clave, sizeof(clave), valor, strlen(valor) + 1);
}

// Para cada clave, en orden, anexa "k-" si esta ausente o "k+valor" si esta presente.
// Obtener cambia el orden de uso: por eso los casos ponen primero las que esperan ausentes.
static void presencia(tLruD *pLru, const int *claves, int cant, char *texto)
{
    char valor[TAM_BUFFER + 1];
    char item[TAM_BUFFER + 16];
    int i;

    for(i = 0; i < cant; i++)
    {
        memset(valor, 0, sizeof(valor));
        if(obtenerV(pLru, &claves[i], sizeof(int), valor, TAM_BUFFER) == EXITO)
            sprintf(item, "%d+%s", claves[i], valor);
        else
            sprintf(item, "%d-", claves[i]);
        anexar(texto, item);
    }
}

// Crea la cache con claves 1..cant y valores "A", "B", "C"...
static void llenarLetras(tLruD *pLru, int cant)
{
    char valor[2];
    int i;

    valor[1] = '\0';
    for(i = 1; i <= cant; i++)
    {
        valor[0] = (char)('A' + i - 1);
        agregarTexto(pLru, i, valor);
    }
}

// ------------------------------------------------------------------- cache

static void cp01(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};
    int ret;

    ret = crearV(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(ret));
    if(ret == EXITO)
    {
        presencia(&lru, claves, 1, obt);
        vaciarV(&lru);
    }
    verificarCaso("EXITO 1-", obt, "CP-002-01", "crear cap 3: EXITO y 1 ausente");
}

static void cp02(void)
{
    tLruD lru;
    int ret;

    ret = crearV(&lru, 0, cmpInt);
    if(ret == EXITO)
        vaciarV(&lru);
    verificarCaso("ERROR_PARAMETRO", nombreRetorno(ret), "CP-002-02", "crear cap 0");
}

static void cp03(void)
{
    tLruD lru;
    int ret;

    ret = crearV(&lru, 3, NULL);
    if(ret == EXITO)
        vaciarV(&lru);
    verificarCaso("ERROR_PARAMETRO", nombreRetorno(ret), "CP-002-03", "crear con cmp NULL");
}

static void cp04(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;

    memset(valor, 0, sizeof(valor));
    crearV(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(agregarTexto(&lru, 1, "A")));
    anexar(obt, nombreRetorno(obtenerV(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, valor);
    vaciarV(&lru);
    verificarCaso("EXITO EXITO A", obt, "CP-002-04", "agregar(1,\"A\") y obtener(1)");
}

static void cp05(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3, 4};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarV(&lru);
    verificarCaso("1- 2+B 3+C 4+D", obt, "CP-002-05", "llena: agregar(4) descarta el 1");
}

static void cp06(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;
    int claves[] = {2, 1, 3, 4};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    obtenerV(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarV(&lru);
    verificarCaso("2- 1+A 3+C 4+D", obt, "CP-002-06", "obtener(1) lo salva: se descarta el 2");
}

static void cp07(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {2, 1, 3, 4};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    agregarTexto(&lru, 1, "Z");
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarV(&lru);
    verificarCaso("2- 1+Z 3+C 4+D", obt, "CP-002-07", "reemplazar 1 con \"Z\" lo hace reciente");
}

static void cp08(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2};

    crearV(&lru, 2, cmpInt);
    llenarLetras(&lru, 2);
    agregarTexto(&lru, 2, "B2");
    presencia(&lru, claves, 2, obt);
    vaciarV(&lru);
    verificarCaso("1+A 2+B2", obt, "CP-002-08", "reemplazo con la cache llena no descarta");
}

static void cp09(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};

    crearV(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "corto");
    agregarTexto(&lru, 1, "un texto mas largo");
    presencia(&lru, claves, 1, obt);
    vaciarV(&lru);
    verificarCaso("1+un texto mas largo", obt, "CP-002-09", "reemplazo por un valor mas largo");
}

static void cp10(void)
{
    tLruD lru;
    char valor[TAM_BUFFER + 1];
    char obt[TAM_TEXTO];
    int clave = 1;
    int restoX = 1;
    int i;

    memset(valor, 'X', TAM_BUFFER);
    valor[TAM_BUFFER] = '\0';
    crearV(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "un texto mas largo");
    agregarTexto(&lru, 1, "ab");
    obtenerV(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    vaciarV(&lru);
    for(i = 3; i < TAM_BUFFER; i++)
        if(valor[i] != 'X')
            restoX = 0;
    sprintf(obt, "\"%s\" y resto %s", valor, restoX ? "en X" : "pisado");
    verificarCaso("\"ab\" y resto en X", obt, "CP-002-10",
                  "reemplazo por un valor mas corto copia solo \"ab\"");
}

static void cp11(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1] = "XXXX";
    int clave = 9;
    int claves[] = {1};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(obtenerV(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, valor);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 1, obt);
    vaciarV(&lru);
    verificarCaso("NO_ENCONTRADO XXXX 1-", obt, "CP-002-11",
                  "obtener(9) no toca el buffer ni el orden");
}

static void cp12(void)
{
    tLruD lru;
    char valor[TAM_BUFFER + 1];
    int clave = 1;
    int ret;

    crearV(&lru, 3, cmpInt);
    ret = obtenerV(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    vaciarV(&lru);
    verificarCaso("NO_ENCONTRADO", nombreRetorno(ret), "CP-002-12", "obtener en cache vacia");
}

static void cp13(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 2;
    int claves[] = {2, 1, 3, 4};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(borrarV(&lru, &clave, sizeof(clave))));
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarV(&lru);
    verificarCaso("EXITO 2- 1+A 3+C 4+D", obt, "CP-002-13", "borrar(2) libera un lugar");
}

static void cp14(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 3;
    int claves[] = {1, 2, 4, 5};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(borrarV(&lru, &clave, sizeof(clave))));
    agregarTexto(&lru, 4, "D");
    agregarTexto(&lru, 5, "E");
    presencia(&lru, claves, 4, obt);
    vaciarV(&lru);
    verificarCaso("EXITO 1- 2+B 4+D 5+E", obt, "CP-002-14",
                  "borrar(3) y agregar 4, 5: el 1 sigue siendo el menos reciente");
}

static void cp15(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 1;
    int claves[] = {1, 2};

    crearV(&lru, 1, cmpInt);
    agregarTexto(&lru, 1, "A");
    anexar(obt, nombreRetorno(borrarV(&lru, &clave, sizeof(clave))));
    anexar(obt, nombreRetorno(agregarTexto(&lru, 2, "B")));
    presencia(&lru, claves, 2, obt);
    vaciarV(&lru);
    verificarCaso("EXITO EXITO 1- 2+B", obt, "CP-002-15", "cap 1: borrar el unico y agregar otro");
}

static void cp16(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 9;
    int claves[] = {1, 2};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 2);
    anexar(obt, nombreRetorno(borrarV(&lru, &clave, sizeof(clave))));
    presencia(&lru, claves, 2, obt);
    vaciarV(&lru);
    verificarCaso("NO_ENCONTRADO 1+A 2+B", obt, "CP-002-16", "borrar(9) inexistente");
}

static void cp17(void)
{
    tLruD lru;
    int clave = 1;
    int ret;

    crearV(&lru, 3, cmpInt);
    ret = borrarV(&lru, &clave, sizeof(clave));
    vaciarV(&lru);
    verificarCaso("NO_ENCONTRADO", nombreRetorno(ret), "CP-002-17", "borrar en cache vacia");
}

static void cp18(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3, 4, 5, 6, 7};

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    vaciarV(&lru);
    agregarTexto(&lru, 4, "D");
    agregarTexto(&lru, 5, "E");
    agregarTexto(&lru, 6, "F");
    agregarTexto(&lru, 7, "G");
    presencia(&lru, claves, 7, obt);
    vaciarV(&lru);
    verificarCaso("1- 2- 3- 4- 5+E 6+F 7+G", obt, "CP-002-18",
                  "vaciar y reutilizar con la misma capacidad");
}

static void cp19(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char clave[15];
    tPosteoPrueba posteo;
    tPosteoPrueba leido;
    char item[TAM_TEXTO];

    memset(clave, 0, sizeof(clave));
    strcpy(clave, "ana");
    memset(&posteo, 0, sizeof(posteo));
    posteo.id = 7;
    strcpy(posteo.texto, "Hola desde la cache");
    memset(&leido, 0, sizeof(leido));
    crearV(&lru, 3, cmpCadena);
    anexar(obt, nombreRetorno(agregarV(&lru, clave, sizeof(clave), &posteo, sizeof(posteo))));
    anexar(obt, nombreRetorno(obtenerV(&lru, clave, sizeof(clave), &leido, sizeof(leido))));
    vaciarV(&lru);
    leido.texto[sizeof(leido.texto) - 1] = '\0';
    sprintf(item, "%d \"%s\"", leido.id, leido.texto);
    anexar(obt, item);
    verificarCaso("EXITO EXITO 7 \"Hola desde la cache\"", obt, "CP-002-19",
                  "clave char[15] con strcmp y valor tPosteo");
}

static void cp20(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char item[32];
    int clave = 5;
    double valor = 2.5;
    double leido = 0;

    crearV(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(agregarV(&lru, &clave, sizeof(clave), &valor, sizeof(valor))));
    anexar(obt, nombreRetorno(obtenerV(&lru, &clave, sizeof(clave), &leido, sizeof(leido))));
    vaciarV(&lru);
    sprintf(item, "%g", leido);
    anexar(obt, item);
    verificarCaso("EXITO EXITO 2.5", obt, "CP-002-20", "clave int y valor double");
}

static void cp21(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[5];
    char item[32];
    int clave = 1;

    memset(valor, '.', 4);
    valor[4] = '#';
    crearV(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "ABCDEFGH");
    anexar(obt, nombreRetorno(obtenerV(&lru, &clave, sizeof(clave), valor, 4)));
    vaciarV(&lru);
    sprintf(item, "%.4s centinela %c", valor, valor[4]);
    anexar(obt, item);
    verificarCaso("EXITO ABCD centinela #", obt, "CP-002-21",
                  "buffer de 4: copia 4 bytes y no pisa el centinela");
}

static void cp22(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2};

    crearV(&lru, 1, cmpInt);
    llenarLetras(&lru, 2);
    presencia(&lru, claves, 2, obt);
    vaciarV(&lru);
    verificarCaso("1- 2+B", obt, "CP-002-22", "cap 1: el segundo desplaza al primero");
}

static void cp23(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {0, 450, 899, 900, 950, 999};
    int i;

    crearV(&lru, 100, cmpInt);
    for(i = 0; i < 1000; i++)
        agregarTexto(&lru, i, "v");
    presencia(&lru, claves, 6, obt);
    vaciarV(&lru);
    verificarCaso("0- 450- 899- 900+v 950+v 999+v", obt, "CP-002-23",
                  "cap 100 con 1000 claves: quedan las ultimas 100");
}

// ------------------------------------------------------ propios de la Parte 2

static void anexarAltura(tLruD *pLru, char *texto)
{
    char item[24];

    sprintf(item, "altura=%d", alturaIndiceLruD(pLru));
    anexar(texto, item);
}

static void cp26(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";

    crearV(&lru, 10, cmpInt);
    anexarAltura(&lru, obt);
    agregarTexto(&lru, 1, "A");
    anexarAltura(&lru, obt);
    vaciarV(&lru);
    verificarCaso("altura=0 altura=1", obt, "CP-002-26", "altura vacia y con un elemento");
}

static void cp27(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";

    crearV(&lru, 10, cmpInt);
    agregarTexto(&lru, 2, "B");
    agregarTexto(&lru, 1, "A");
    agregarTexto(&lru, 3, "C");
    anexarAltura(&lru, obt);
    vaciarV(&lru);
    verificarCaso("altura=2", obt, "CP-002-27", "agregar 2, 1, 3: altura 2");
}

static void cp28(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int i;

    crearV(&lru, 1000, cmpInt);
    for(i = 0; i < 1000; i++)
        agregarTexto(&lru, i, "v");
    anexarAltura(&lru, obt);
    vaciarV(&lru);
    verificarCaso("altura=1000", obt, "CP-002-28", "0 a 999 en orden: el indice degenera");
}

static void cp29(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {2, 1, 3, 4};

    crearV(&lru, 3, cmpInt);
    agregarTexto(&lru, 2, "B");
    agregarTexto(&lru, 1, "A");
    agregarTexto(&lru, 3, "C");
    agregarTexto(&lru, 4, "D");
    anexarAltura(&lru, obt);
    presencia(&lru, claves, 4, obt);
    vaciarV(&lru);
    verificarCaso("altura=2 2- 1+A 3+C 4+D", obt, "CP-002-29",
                  "descartar la raiz del indice con dos hijos");
}

static void cp30(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};
    int i;

    crearV(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "A");
    for(i = 0; i < 10; i++)
        agregarTexto(&lru, 1, "B");
    anexarAltura(&lru, obt);
    presencia(&lru, claves, 1, obt);
    vaciarV(&lru);
    verificarCaso("altura=1 1+B", obt, "CP-002-30", "reemplazar diez veces: el indice no crece");
}

static void cp31(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {2, 1, 3};

    crearV(&lru, 2, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(agregarTexto(&lru, 1, "otra vez")));
    presencia(&lru, claves, 3, obt);
    vaciarV(&lru);
    verificarCaso("EXITO 2- 1+otra vez 3+C", obt, "CP-002-31",
                  "volver a agregar una clave descartada");
}

static void cp32(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int i;

    crearV(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    for(i = 1; i <= 3; i++)
        anexar(obt, nombreRetorno(borrarV(&lru, &i, sizeof(i))));
    anexarAltura(&lru, obt);
    vaciarV(&lru);
    verificarCaso("EXITO EXITO EXITO altura=0", obt, "CP-002-32", "borrar todo deja el indice vacio");
}

// ------------------------------------------------------- lista doble (D01 a D10)

static void acumularEntero(void *info, void *contexto)
{
    char num[16];

    sprintf(num, "%d", *(int*)info);
    anexar((char*)contexto, num);
}

// Deja en texto los enteros de pri a ult, separados por espacio
static void recorridoD(const tListaD *pLista, char *texto)
{
    *texto = '\0';
    recorrerListaD(pLista, acumularEntero, texto);
}

static int insertarEntero(tListaD *pLista, int valor, tPosListaD *pos)
{
    return insPrinListaD(pLista, &valor, sizeof(valor), pos);
}

// Inserta 1, 2 y 3 al principio (recorrido 3 2 1); pos[k] queda con la posicion de k
static void armarTres(tListaD *pLista, tPosListaD pos[4])
{
    int k;

    crearListaD(pLista);
    for(k = 1; k <= 3; k++)
        insertarEntero(pLista, k, &pos[k]);
}

// Saca el ultimo tres veces y anexa cada valor sacado
static void sacarTres(tListaD *pLista, char *texto)
{
    char num[16];
    int valor;
    int i;

    for(i = 0; i < 3; i++)
    {
        valor = 0;
        sacarUltListaD(pLista, &valor, sizeof(valor));
        sprintf(num, "%d", valor);
        anexar(texto, num);
    }
}

static void cpD01(void)
{
    tListaD lista;
    char obt[TAM_TEXTO] = "";
    int valor = 0;

    crearListaD(&lista);
    anexar(obt, nombreEstadoLista(listaVaciaD(&lista)));
    anexar(obt, nombreRetorno(verUltListaD(&lista, &valor, sizeof(valor))));
    verificarTexto("LISTA_VACIA VACIA", obt, "CP-002-D01", "crear; vacia; ver ultimo en vacia");
}

static void cpD02(void)
{
    tListaD lista;
    tPosListaD pos = NULL;
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    char item[32];
    int valor = 0;
    int ret;

    crearListaD(&lista);
    ret = insertarEntero(&lista, 7, &pos);
    verUltListaD(&lista, &valor, sizeof(valor));
    recorridoD(&lista, rec);
    sprintf(item, "%s %s ult=%d [%.16s]", nombreRetorno(ret), pos ? "pos" : "NULL", valor, rec);
    anexar(obt, item);
    vaciarListaD(&lista);
    verificarTexto("EXITO pos ult=7 [7]", obt, "CP-002-D02", "insertar 7 en vacia: primero y ultimo");
}

static void cpD03(void)
{
    tListaD lista;
    tPosListaD pos[4];
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    int valor;

    armarTres(&lista, pos);
    recorridoD(&lista, rec);
    anexar(obt, rec);
    sacarTres(&lista, obt);
    anexar(obt, nombreRetorno(sacarUltListaD(&lista, &valor, sizeof(valor))));
    verificarTexto("3 2 1 1 2 3 VACIA", obt, "CP-002-D03", "insertar 1, 2, 3; sacar ultimo cuatro veces");
}

// D04 a D06: mover una posicion de 3 2 1 al principio
static void moverYVerificar(int k, const char *esperado, const char *id, const char *descripcion)
{
    tListaD lista;
    tPosListaD pos[4];
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    char item[16];
    int valor = 0;

    armarTres(&lista, pos);
    moverAlPrinListaD(&lista, pos[k]);
    recorridoD(&lista, rec);
    anexar(obt, rec);
    verUltListaD(&lista, &valor, sizeof(valor));
    sprintf(item, "ult=%d |", valor);
    anexar(obt, item);
    sacarTres(&lista, obt);
    verificarTexto(esperado, obt, id, descripcion);
}

static void cpD07(void)
{
    tListaD lista;
    tPosListaD pos[5];
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    char item[16];
    int aSacar[] = {2, 4, 1};
    int valor = 0;
    int k;

    crearListaD(&lista);
    for(k = 1; k <= 4; k++)
        insertarEntero(&lista, k, &pos[k]);
    for(k = 0; k < 3; k++)
    {
        sacarNodoListaD(&lista, pos[aSacar[k]], NULL, 0);
        recorridoD(&lista, rec);
        sprintf(item, "[%.12s]", rec);
        anexar(obt, item);
    }
    verUltListaD(&lista, &valor, sizeof(valor));
    sprintf(item, "ult=%d", valor);
    anexar(obt, item);
    vaciarListaD(&lista);
    verificarTexto("[4 3 1] [3 1] [3] ult=3", obt, "CP-002-D07", "sacar del medio, el primero y el ultimo");
}

static void cpD08(void)
{
    tListaD lista;
    tPosListaD pos;
    char obt[TAM_TEXTO] = "";
    int valor = 0;

    crearListaD(&lista);
    insertarEntero(&lista, 5, &pos);
    anexar(obt, nombreRetorno(sacarNodoListaD(&lista, pos, &valor, sizeof(valor))));
    anexar(obt, nombreEstadoLista(listaVaciaD(&lista)));
    anexar(obt, nombreRetorno(verUltListaD(&lista, &valor, sizeof(valor))));
    verificarTexto("EXITO LISTA_VACIA VACIA", obt, "CP-002-D08", "sacar el unico nodo");
}

static void cpD09(void)
{
    tListaD lista;
    tPosListaD pos;
    char obt[TAM_TEXTO];
    char buf[8];
    int ret;
    unsigned tam;

    memset(buf, 0, sizeof(buf));
    crearListaD(&lista);
    insPrinListaD(&lista, "corto", 6, &pos);
    ret = actualizarInfoListaD(pos, "un texto mas largo", 19);
    tam = (unsigned)verInfoListaD(pos, buf, 4);
    sprintf(obt, "%s %u \"%.4s\" nodos=%d", nombreRetorno(ret), tam, buf, vaciarListaD(&lista));
    verificarTexto("EXITO 19 \"un t\" nodos=1", obt, "CP-002-D09",
                   "actualizar con un valor mas largo y ver con buffer de 4");
}

static void cpD10(void)
{
    tListaD lista;
    tPosListaD pos;
    char obt[TAM_TEXTO];
    int primera;
    int segunda;
    int k;

    crearListaD(&lista);
    for(k = 1; k <= 4; k++)
        insertarEntero(&lista, k, &pos);
    primera = vaciarListaD(&lista);
    segunda = vaciarListaD(&lista);
    sprintf(obt, "%d %d %s ult=%s", primera, segunda, nombreEstadoLista(listaVaciaD(&lista)),
            lista.ult ? "no NULL" : "NULL");
    verificarTexto("4 0 LISTA_VACIA ult=NULL", obt, "CP-002-D10", "vaciar 4 nodos y despues una vacia");
}

// ------------------------------------------------------- funcion de la spec

int pruebas002(void)
{
    int fallasAntes = cantidadFallas();

    printf("== Spec 002: LRU con lista doble e indice\n");

    // Lista doble primero (T2 a T4)
    cpD01();
    cpD02();
    cpD03();
    moverYVerificar(1, "1 3 2 ult=2 | 2 3 1", "CP-002-D04", "mover el ultimo al principio");
    moverYVerificar(2, "2 3 1 ult=1 | 1 3 2", "CP-002-D05", "mover el del medio al principio");
    moverYVerificar(3, "3 2 1 ult=1 | 1 2 3", "CP-002-D06", "mover el primero no cambia nada");
    cpD07();
    cpD08();
    cpD09();
    cpD10();

    // Cache: los mismos de la Parte 1 con verificarLruD tras cada operacion (T6 a T8)
    cp01();
    cp02();
    cp03();
    cp04();
    cp05();
    cp06();
    cp07();
    cp08();
    cp09();
    cp10();
    cp11();
    cp12();
    cp13();
    cp14();
    cp15();
    cp16();
    cp17();
    cp18();
    cp19();
    cp20();
    cp21();
    cp22();
    cp23();

    // Propios de la Parte 2 (la altura necesita el arbol real: T9)
    cp26();
    cp27();
    cp28();
    cp29();
    cp30();
    cp31();
    cp32();

    return cantidadFallas() - fallasAntes;
}
