
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comun.h"
#include "lista.h"
#include "entrada.h"
#include "lru_simple.h"
#include "verificar.h"
#include "pruebas.h"

#define TAM_TEXTO 256
#define TAM_BUFFER 32

typedef struct
{
    int clave;
    char nombre[10];
} tReg;

typedef struct
{
    int id;
    char texto[64];
} tPosteoPrueba;

static int cmpInt(const void *a, const void *b)
{
    return *(const int*)a - *(const int*)b;
}

static int cmpCadena(const void *a, const void *b)
{
    return strcmp((const char*)a, (const char*)b);
}

static void anexar(char *texto, const char *agregado)
{
    if(*texto)
        strcat(texto, " ");
    strcat(texto, agregado);
}

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

static const char *nombreDe(int clave)
{
    static const char *nombres[] = {"cero", "uno", "dos", "tres", "cuatro", "cinco"};

    if(clave >= 0 && clave <= 5)
        return nombres[clave];
    return "otro";
}

static int insertarReg(tLista *pLista, int clave)
{
    tReg reg;

    memset(&reg, 0, sizeof(reg));
    reg.clave = clave;
    strcpy(reg.nombre, nombreDe(clave));
    return insPrinLista(pLista, &reg, sizeof(reg));
}

static void armarLista(tLista *pLista, const int *claves, int cant)
{
    int i;

    crearLista(pLista);
    for(i = cant - 1; i >= 0; i--)
        insertarReg(pLista, claves[i]);
}

static void acumularClave(void *info, void *contexto)
{
    char num[16];

    sprintf(num, "%d", ((tReg*)info)->clave);
    anexar((char*)contexto, num);
}

static void recorrido(const tLista *pLista, char *texto)
{
    *texto = '\0';
    recorrerLista(pLista, acumularClave, texto);
}

static int agregarTexto(tLruS *pLru, int clave, const char *valor)
{
    return agregarLruS(pLru, &clave, sizeof(clave), valor, strlen(valor) + 1);
}

static void presencia(tLruS *pLru, const int *claves, int cant, char *texto)
{
    char valor[TAM_BUFFER + 1];
    char item[TAM_BUFFER + 16];
    int i;

    for(i = 0; i < cant; i++)
    {
        memset(valor, 0, sizeof(valor));
        if(obtenerLruS(pLru, &claves[i], sizeof(int), valor, TAM_BUFFER) == EXITO)
            sprintf(item, "%d+%s", claves[i], valor);
        else
            sprintf(item, "%d-", claves[i]);
        anexar(texto, item);
    }
}

static void llenarLetras(tLruS *pLru, int cant)
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

static void cp01(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};
    int ret;

    ret = crearLruS(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(ret));
    if(ret == EXITO)
    {
        presencia(&lru, claves, 1, obt);
        vaciarLruS(&lru);
    }
    verificarTexto("EXITO 1-", obt, "CP-001-01", "crear cap 3: EXITO y 1 ausente");
}

static void cp02(void)
{
    tLruS lru;
    int ret;

    ret = crearLruS(&lru, 0, cmpInt);
    if(ret == EXITO)
        vaciarLruS(&lru);
    verificarTexto("ERROR_PARAMETRO", nombreRetorno(ret), "CP-001-02", "crear cap 0");
}

static void cp03(void)
{
    tLruS lru;
    int ret;

    ret = crearLruS(&lru, 3, NULL);
    if(ret == EXITO)
        vaciarLruS(&lru);
    verificarTexto("ERROR_PARAMETRO", nombreRetorno(ret), "CP-001-03", "crear con cmp NULL");
}

static void cp04(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;

    memset(valor, 0, sizeof(valor));
    crearLruS(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(agregarTexto(&lru, 1, "A")));
    anexar(obt, nombreRetorno(obtenerLruS(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, valor);
    vaciarLruS(&lru);
    verificarTexto("EXITO EXITO A", obt, "CP-001-04", "agregar(1,\"A\") y obtener(1)");
}

static void cp05(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3, 4};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarLruS(&lru);
    verificarTexto("1- 2+B 3+C 4+D", obt, "CP-001-05", "llena: agregar(4) descarta el 1");
}

static void cp06(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;
    int claves[] = {2, 1, 3, 4};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    obtenerLruS(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarLruS(&lru);
    verificarTexto("2- 1+A 3+C 4+D", obt, "CP-001-06", "obtener(1) lo salva: se descarta el 2");
}

static void cp07(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {2, 1, 3, 4};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    agregarTexto(&lru, 1, "Z");
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarLruS(&lru);
    verificarTexto("2- 1+Z 3+C 4+D", obt, "CP-001-07", "reemplazar 1 con \"Z\" lo hace reciente");
}

static void cp08(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2};

    crearLruS(&lru, 2, cmpInt);
    llenarLetras(&lru, 2);
    agregarTexto(&lru, 2, "B2");
    presencia(&lru, claves, 2, obt);
    vaciarLruS(&lru);
    verificarTexto("1+A 2+B2", obt, "CP-001-08", "reemplazo con la cache llena no descarta");
}

static void cp09(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};

    crearLruS(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "corto");
    agregarTexto(&lru, 1, "un texto mas largo");
    presencia(&lru, claves, 1, obt);
    vaciarLruS(&lru);
    verificarTexto("1+un texto mas largo", obt, "CP-001-09", "reemplazo por un valor mas largo");
}

static void cp10(void)
{
    tLruS lru;
    char valor[TAM_BUFFER + 1];
    char obt[TAM_TEXTO];
    int clave = 1;
    int restoX = 1;
    int i;

    memset(valor, 'X', TAM_BUFFER);
    valor[TAM_BUFFER] = '\0';
    crearLruS(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "un texto mas largo");
    agregarTexto(&lru, 1, "ab");
    obtenerLruS(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    vaciarLruS(&lru);
    for(i = 3; i < TAM_BUFFER; i++)
        if(valor[i] != 'X')
            restoX = 0;
    sprintf(obt, "\"%s\" y resto %s", valor, restoX ? "en X" : "pisado");
    verificarTexto("\"ab\" y resto en X", obt, "CP-001-10",
                   "reemplazo por un valor mas corto copia solo \"ab\"");
}

static void cp11(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1] = "XXXX";
    int clave = 9;
    int claves[] = {1};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(obtenerLruS(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, valor);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 1, obt);
    vaciarLruS(&lru);
    verificarTexto("NO_ENCONTRADO XXXX 1-", obt, "CP-001-11",
                   "obtener(9) no toca el buffer ni el orden");
}

static void cp12(void)
{
    tLruS lru;
    char valor[TAM_BUFFER + 1];
    int clave = 1;
    int ret;

    crearLruS(&lru, 3, cmpInt);
    ret = obtenerLruS(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    vaciarLruS(&lru);
    verificarTexto("NO_ENCONTRADO", nombreRetorno(ret), "CP-001-12", "obtener en cache vacia");
}

static void cp13(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int clave = 2;
    int claves[] = {2, 1, 3, 4};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(borrarLruS(&lru, &clave, sizeof(clave))));
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarLruS(&lru);
    verificarTexto("EXITO 2- 1+A 3+C 4+D", obt, "CP-001-13", "borrar(2) libera un lugar");
}

static void cp14(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int clave = 3;
    int claves[] = {1, 2, 4, 5};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(borrarLruS(&lru, &clave, sizeof(clave))));
    agregarTexto(&lru, 4, "D");
    agregarTexto(&lru, 5, "E");
    presencia(&lru, claves, 4, obt);
    vaciarLruS(&lru);
    verificarTexto("EXITO 1- 2+B 4+D 5+E", obt, "CP-001-14",
                   "borrar(3) y agregar 4, 5: el 1 sigue siendo el menos reciente");
}

static void cp15(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int clave = 1;
    int claves[] = {1, 2};

    crearLruS(&lru, 1, cmpInt);
    agregarTexto(&lru, 1, "A");
    anexar(obt, nombreRetorno(borrarLruS(&lru, &clave, sizeof(clave))));
    anexar(obt, nombreRetorno(agregarTexto(&lru, 2, "B")));
    presencia(&lru, claves, 2, obt);
    vaciarLruS(&lru);
    verificarTexto("EXITO EXITO 1- 2+B", obt, "CP-001-15", "cap 1: borrar el unico y agregar otro");
}

static void cp16(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int clave = 9;
    int claves[] = {1, 2};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 2);
    anexar(obt, nombreRetorno(borrarLruS(&lru, &clave, sizeof(clave))));
    presencia(&lru, claves, 2, obt);
    vaciarLruS(&lru);
    verificarTexto("NO_ENCONTRADO 1+A 2+B", obt, "CP-001-16", "borrar(9) inexistente");
}

static void cp17(void)
{
    tLruS lru;
    int clave = 1;
    int ret;

    crearLruS(&lru, 3, cmpInt);
    ret = borrarLruS(&lru, &clave, sizeof(clave));
    vaciarLruS(&lru);
    verificarTexto("NO_ENCONTRADO", nombreRetorno(ret), "CP-001-17", "borrar en cache vacia");
}

static void cp18(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3, 4, 5, 6, 7};

    crearLruS(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    vaciarLruS(&lru);
    agregarTexto(&lru, 4, "D");
    agregarTexto(&lru, 5, "E");
    agregarTexto(&lru, 6, "F");
    agregarTexto(&lru, 7, "G");
    presencia(&lru, claves, 7, obt);
    vaciarLruS(&lru);
    verificarTexto("1- 2- 3- 4- 5+E 6+F 7+G", obt, "CP-001-18",
                   "vaciar y reutilizar con la misma capacidad");
}

static void cp19(void)
{
    tLruS lru;
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
    crearLruS(&lru, 3, cmpCadena);
    anexar(obt, nombreRetorno(agregarLruS(&lru, clave, sizeof(clave), &posteo, sizeof(posteo))));
    anexar(obt, nombreRetorno(obtenerLruS(&lru, clave, sizeof(clave), &leido, sizeof(leido))));
    vaciarLruS(&lru);
    leido.texto[sizeof(leido.texto) - 1] = '\0';
    sprintf(item, "%d \"%s\"", leido.id, leido.texto);
    anexar(obt, item);
    verificarTexto("EXITO EXITO 7 \"Hola desde la cache\"", obt, "CP-001-19",
                   "clave char[15] con strcmp y valor tPosteo");
}

static void cp20(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    char item[32];
    int clave = 5;
    double valor = 2.5;
    double leido = 0;

    crearLruS(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(agregarLruS(&lru, &clave, sizeof(clave), &valor, sizeof(valor))));
    anexar(obt, nombreRetorno(obtenerLruS(&lru, &clave, sizeof(clave), &leido, sizeof(leido))));
    vaciarLruS(&lru);
    sprintf(item, "%g", leido);
    anexar(obt, item);
    verificarTexto("EXITO EXITO 2.5", obt, "CP-001-20", "clave int y valor double");
}

static void cp21(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    char valor[5];
    char item[32];
    int clave = 1;

    memset(valor, '.', 4);
    valor[4] = '#';
    crearLruS(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "ABCDEFGH");
    anexar(obt, nombreRetorno(obtenerLruS(&lru, &clave, sizeof(clave), valor, 4)));
    vaciarLruS(&lru);
    sprintf(item, "%.4s centinela %c", valor, valor[4]);
    anexar(obt, item);
    verificarTexto("EXITO ABCD centinela #", obt, "CP-001-21",
                   "buffer de 4: copia 4 bytes y no pisa el centinela");
}

static void cp22(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2};

    crearLruS(&lru, 1, cmpInt);
    llenarLetras(&lru, 2);
    presencia(&lru, claves, 2, obt);
    vaciarLruS(&lru);
    verificarTexto("1- 2+B", obt, "CP-001-22", "cap 1: el segundo desplaza al primero");
}

static void cp23(void)
{
    tLruS lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {0, 450, 899, 900, 950, 999};
    int i;

    crearLruS(&lru, 100, cmpInt);
    for(i = 0; i < 1000; i++)
        agregarTexto(&lru, i, "v");
    presencia(&lru, claves, 6, obt);
    vaciarLruS(&lru);
    verificarTexto("0- 450- 899- 900+v 950+v 999+v", obt, "CP-001-23",
                   "cap 100 con 1000 claves: quedan las ultimas 100");
}

static void cpL01(void)
{
    tLista lista;

    crearLista(&lista);
    verificarTexto("LISTA_VACIA", nombreEstadoLista(listaVacia(&lista)), "CP-001-L01",
                   "lista recien creada esta vacia");
}

static void cpL02(void)
{
    tLista lista;
    char obt[TAM_TEXTO];

    crearLista(&lista);
    insertarReg(&lista, 3);
    insertarReg(&lista, 2);
    insertarReg(&lista, 1);
    recorrido(&lista, obt);
    anexar(obt, nombreEstadoLista(listaVacia(&lista)));
    vaciarLista(&lista);
    verificarTexto("1 2 3 LISTA_DISPONIBLE", obt, "CP-001-L02", "insertar 3, 2, 1 al principio");
}

static void cpL03(void)
{
    tLista lista;
    tReg reg;
    char obt[TAM_TEXTO] = "";
    char item[TAM_TEXTO];
    int claves[] = {1, 2, 3};
    int clave = 2;
    int ret;

    memset(&reg, 0, sizeof(reg));
    armarLista(&lista, claves, 3);
    ret = buscarLista(&lista, &clave, &reg, sizeof(reg), cmpInt);
    sprintf(item, "%s {%d %s} |", nombreRetorno(ret), reg.clave, reg.nombre);
    recorrido(&lista, obt);
    vaciarLista(&lista);
    strcat(item, " ");
    strcat(item, obt);
    verificarTexto("EXITO {2 dos} | 1 2 3", item, "CP-001-L03",
                   "buscar 2 copia el registro y no reordena");
}

static void cpL04(void)
{
    tLista lista;
    tReg reg;
    char item[TAM_TEXTO];
    int claves[] = {1, 2, 3};
    int clave = 9;
    int ret;

    reg.clave = 0;
    strcpy(reg.nombre, "XXX");
    armarLista(&lista, claves, 3);
    ret = buscarLista(&lista, &clave, &reg, sizeof(reg), cmpInt);
    vaciarLista(&lista);
    reg.nombre[sizeof(reg.nombre) - 1] = '\0';
    sprintf(item, "%s {%d %s}", nombreRetorno(ret), reg.clave, reg.nombre);
    verificarTexto("NO_ENCONTRADO {0 XXX}", item, "CP-001-L04", "buscar 9 no toca el destino");
}

static void llevarYVerificar(int clave, const char *esperado, const char *id,
                             const char *descripcion)
{
    tLista lista;
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    int claves[] = {1, 2, 3};

    armarLista(&lista, claves, 3);
    anexar(obt, nombreRetorno(llevarAlPrinLista(&lista, &clave, cmpInt)));
    recorrido(&lista, rec);
    anexar(obt, rec);
    vaciarLista(&lista);
    verificarTexto(esperado, obt, id, descripcion);
}

static void cpL09(void)
{
    tLista lista;
    tReg reg;
    char obt[TAM_TEXTO] = "";
    char item[TAM_TEXTO + 64];
    char rec[TAM_TEXTO];
    int claves[] = {1, 2, 3, 4};
    int aSacar[] = {1, 3, 4};
    int i;
    int ret;

    armarLista(&lista, claves, 4);
    for(i = 0; i < 3; i++)
    {
        memset(&reg, 0, sizeof(reg));
        ret = sacarPorClaveLista(&lista, &aSacar[i], &reg, sizeof(reg), cmpInt);
        recorrido(&lista, rec);
        sprintf(item, "%s {%d %s} [%s]", nombreRetorno(ret), reg.clave, reg.nombre, rec);
        anexar(obt, item);
    }
    vaciarLista(&lista);
    verificarTexto("EXITO {1 uno} [2 3 4] EXITO {3 tres} [2 4] EXITO {4 cuatro} [2]", obt,
                   "CP-001-L09", "sacar 1, 3 y 4 (primero, medio y ultimo)");
}

static void cpL10(void)
{
    tLista lista;
    tReg reg;
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    int claves[] = {1, 2};
    int clave;

    armarLista(&lista, claves, 2);
    clave = 9;
    anexar(obt, nombreRetorno(sacarPorClaveLista(&lista, &clave, &reg, sizeof(reg), cmpInt)));
    clave = 2;
    anexar(obt, nombreRetorno(sacarPorClaveLista(&lista, &clave, NULL, 0, cmpInt)));
    recorrido(&lista, rec);
    anexar(obt, rec);
    vaciarLista(&lista);
    verificarTexto("NO_ENCONTRADO EXITO 1", obt, "CP-001-L10",
                   "sacar 9 inexistente; sacar 2 con info NULL");
}

static void cpL11(void)
{
    tLista lista;
    tReg reg;
    char obt[TAM_TEXTO] = "";
    char item[TAM_TEXTO];
    int claves[] = {1, 2, 3};
    int i;
    int ret;

    armarLista(&lista, claves, 3);
    for(i = 0; i < 3; i++)
    {
        memset(&reg, 0, sizeof(reg));
        ret = sacarUltLista(&lista, &reg, sizeof(reg));
        sprintf(item, "%s %d", nombreRetorno(ret), reg.clave);
        anexar(obt, item);
    }
    anexar(obt, nombreRetorno(sacarUltLista(&lista, &reg, sizeof(reg))));
    vaciarLista(&lista);
    verificarTexto("EXITO 3 EXITO 2 EXITO 1 VACIA", obt, "CP-001-L11",
                   "sacar el ultimo cuatro veces");
}

static void cpL12(void)
{
    tLista lista;
    tReg reg;
    char obt[TAM_TEXTO] = "";
    char item[TAM_TEXTO];
    int claves[] = {5, 7};
    int ret;

    crearLista(&lista);
    anexar(obt, nombreRetorno(verPrimeroLista(&lista, &reg, sizeof(reg))));
    armarLista(&lista, claves, 2);
    memset(&reg, 0, sizeof(reg));
    ret = verPrimeroLista(&lista, &reg, sizeof(reg));
    sprintf(item, "%s %d", nombreRetorno(ret), reg.clave);
    anexar(obt, item);
    vaciarLista(&lista);
    verificarTexto("VACIA EXITO 5", obt, "CP-001-L12", "ver primero en vacia y en 5 7");
}

static void cpL13(void)
{
    tLista lista;
    char obt[TAM_TEXTO];
    int claves[] = {1, 2, 3, 4, 5};
    int primera;
    int segunda;

    armarLista(&lista, claves, 5);
    primera = vaciarLista(&lista);
    segunda = vaciarLista(&lista);
    sprintf(obt, "%d %d %s", primera, segunda, nombreEstadoLista(listaVacia(&lista)));
    verificarTexto("5 0 LISTA_VACIA", obt, "CP-001-L13", "vaciar 5 nodos y despues una vacia");
}

static void cpL14(void)
{
    void *bloque = NULL;
    char buf16[16];
    char buf2[2];
    char obt[TAM_TEXTO];
    int clave = 7;
    int ret;
    size_t copia16 = 0;
    size_t copia2 = 0;

    memset(buf16, 0, sizeof(buf16));
    memset(buf2, 0, sizeof(buf2));
    ret = crearEntrada(&bloque, &clave, sizeof(clave), "hola", 5);
    if(ret == EXITO && bloque)
    {
        copia16 = copiarValorEntrada(bloque, sizeof(int) + 5, sizeof(int), buf16, sizeof(buf16));
        copia2 = copiarValorEntrada(bloque, sizeof(int) + 5, sizeof(int), buf2, sizeof(buf2));
        free(bloque);
    }
    buf16[sizeof(buf16) - 1] = '\0';
    sprintf(obt, "%s \"%s\" %u \"%.2s\" %u", nombreRetorno(ret), buf16, (unsigned)copia16,
            buf2, (unsigned)copia2);
    verificarTexto("EXITO \"hola\" 5 \"ho\" 2", obt, "CP-001-L14",
                   "crear entrada {7, \"hola\"} y copiar con buffer de 16 y de 2");
}

static void cpL15(void)
{
    tLista lista;
    char obt[TAM_TEXTO];
    char rec[TAM_TEXTO];
    int claves[] = {1, 2, 3};
    int antes;
    int despues;

    crearLista(&lista);
    antes = listaLlena(&lista, sizeof(tReg));
    vaciarLista(&lista);
    armarLista(&lista, claves, 3);
    despues = listaLlena(&lista, sizeof(tReg));
    recorrido(&lista, rec);
    vaciarLista(&lista);
    sprintf(obt, "%s %s [%.200s]", nombreEstadoLista(antes), nombreEstadoLista(despues), rec);
    verificarTexto("LISTA_DISPONIBLE LISTA_DISPONIBLE [1 2 3]", obt, "CP-001-L15", "lista llena en vacia y en 1 2 3, sin modificarla");
}

int pruebas001(void)
{
    int fallasAntes = cantidadFallas();

    printf("== Spec 001: LRU sobre lista simple\n");

    cpL01();
    cpL02();
    cpL03();
    cpL04();
    llevarYVerificar(2, "EXITO 2 1 3", "CP-001-L05", "llevar 2 al principio");
    llevarYVerificar(3, "EXITO 3 1 2", "CP-001-L06", "llevar 3 (el ultimo) al principio");
    llevarYVerificar(1, "EXITO 1 2 3", "CP-001-L07", "llevar 1 (ya primero) no cambia nada");
    llevarYVerificar(9, "NO_ENCONTRADO 1 2 3", "CP-001-L08", "llevar 9 inexistente");
    cpL09();
    cpL10();
    cpL11();
    cpL12();
    cpL13();
    cpL14();
    cpL15();

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

    return cantidadFallas() - fallasAntes;
}
