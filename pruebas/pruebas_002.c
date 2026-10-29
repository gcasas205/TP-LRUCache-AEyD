// Pruebas de la spec 002 (LRU con lista doble e indice). Los IDs y valores son los de
// specs/002-lru-doble/casos-prueba.md. Los casos 24, 25, 33 y 34 son de revision y no estan aca.
// CP-002-01 a 23 repiten CP-001-01 a 23 con las primitivas LruD y llaman a verificarLruD despues
// de cada operacion: si alguna vez da 0, el caso termina en "INCONSISTENTE" y falla.

#include <stdio.h>
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

// Cantidad de veces que verificarLruD dio 0 en el caso actual
static int inconsistencias = 0;

// ----------------------------------------------------------------- auxiliares

// Agrega al final de texto lo que sigue, separado por un espacio si texto no esta vacio
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

static void acumularInt(void *info, void *contexto)
{
    char num[16];

    sprintf(num, "%d", *(int*)info);
    anexar((char*)contexto, num);
}

static void contarNodo(void *info, void *contexto)
{
    (void)info;
    (*(int*)contexto)++;
}

// Deja en texto los enteros de pri a ult, separados por espacio
static void recorrido(const tListaD *pLista, char *texto)
{
    *texto = '\0';
    recorrerListaD(pLista, acumularInt, texto);
}

// Inserta al principio los enteros de valores[] en ese orden y guarda la posicion de cada uno
static void armarListaD(tListaD *pLista, const int *valores, tPosListaD *pos, int cant)
{
    int i;

    crearListaD(pLista);
    for(i = 0; i < cant; i++)
        insPrinListaD(pLista, &valores[i], sizeof(int), &pos[i]);
}

// Saca el ultimo hasta vaciar y anexa a texto cada valor sacado (de ult a pri: prueba los ant)
static void sacarTodos(tListaD *pLista, char *texto)
{
    int valor;
    char num[16];

    while(sacarUltListaD(pLista, &valor, sizeof(valor)) == EXITO)
    {
        sprintf(num, "%d", valor);
        anexar(texto, num);
    }
}

// ----------------------------------------------------------------- lista doble

static void cpD01(void)
{
    tListaD lista;
    int valor = 0;
    char obt[TAM_TEXTO];

    crearListaD(&lista);
    sprintf(obt, "%s %s", nombreEstadoLista(listaVaciaD(&lista)),
            nombreRetorno(verUltListaD(&lista, &valor, sizeof(valor))));
    verificarTexto("LISTA_VACIA VACIA", obt, "CP-002-D01", "lista recien creada esta vacia");
}

static void cpD02(void)
{
    tListaD lista;
    tPosListaD pos = NULL;
    int valor = 7;
    int ult = 0;
    int ret;
    char obt[TAM_TEXTO];
    char rec[TAM_TEXTO];

    crearListaD(&lista);
    ret = insPrinListaD(&lista, &valor, sizeof(valor), &pos);
    verUltListaD(&lista, &ult, sizeof(ult));
    recorrido(&lista, rec);
    sprintf(obt, "%s %s ult=%d |", nombreRetorno(ret), pos ? "pos" : "NULL", ult);
    anexar(obt, rec);
    vaciarListaD(&lista);
    verificarTexto("EXITO pos ult=7 | 7", obt, "CP-002-D02", "insertar 7 en vacia: pri y ult iguales");
}

static void cpD03(void)
{
    tListaD lista;
    tPosListaD pos[3];
    int valores[] = {1, 2, 3};
    int valor;
    char obt[TAM_TEXTO];

    armarListaD(&lista, valores, pos, 3);
    recorrido(&lista, obt);
    anexar(obt, "|");
    sacarTodos(&lista, obt);
    anexar(obt, nombreRetorno(sacarUltListaD(&lista, &valor, sizeof(valor))));
    verificarTexto("3 2 1 | 1 2 3 VACIA", obt, "CP-002-D03",
                   "insertar 1, 2, 3 y sacar el ultimo cuatro veces");
}

// D04 a D06: mover al principio una posicion de 3 2 1
static void moverYVerificar(int indice, const char *esperado, const char *id,
                            const char *descripcion)
{
    tListaD lista;
    tPosListaD pos[3];
    int valores[] = {1, 2, 3};
    int ult = 0;
    char obt[TAM_TEXTO];
    char num[16];

    armarListaD(&lista, valores, pos, 3);
    moverAlPrinListaD(&lista, pos[indice]);
    recorrido(&lista, obt);
    verUltListaD(&lista, &ult, sizeof(ult));
    sprintf(num, "ult=%d |", ult);
    anexar(obt, num);
    sacarTodos(&lista, obt);
    verificarTexto(esperado, obt, id, descripcion);
}

static void cpD07(void)
{
    tListaD lista;
    tPosListaD pos[4];
    int valores[] = {1, 2, 3, 4};
    int ult = 0;
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];
    char num[16];

    armarListaD(&lista, valores, pos, 4);
    sacarNodoListaD(&lista, pos[1], NULL, 0);
    recorrido(&lista, rec);
    anexar(obt, rec);
    anexar(obt, "->");
    sacarNodoListaD(&lista, pos[3], NULL, 0);
    recorrido(&lista, rec);
    anexar(obt, rec);
    anexar(obt, "->");
    sacarNodoListaD(&lista, pos[0], NULL, 0);
    recorrido(&lista, rec);
    anexar(obt, rec);
    verUltListaD(&lista, &ult, sizeof(ult));
    sprintf(num, "ult=%d", ult);
    anexar(obt, num);
    vaciarListaD(&lista);
    verificarTexto("4 3 1 -> 3 1 -> 3 ult=3", obt, "CP-002-D07",
                   "sacar nodo del medio, el primero y el ultimo");
}

static void cpD08(void)
{
    tListaD lista;
    tPosListaD pos[1];
    int valores[] = {5};
    int valor = 0;
    int ret;
    char obt[TAM_TEXTO];

    armarListaD(&lista, valores, pos, 1);
    ret = sacarNodoListaD(&lista, pos[0], &valor, sizeof(valor));
    sprintf(obt, "%s %d %s %s", nombreRetorno(ret), valor, nombreEstadoLista(listaVaciaD(&lista)),
            nombreRetorno(verUltListaD(&lista, &valor, sizeof(valor))));
    verificarTexto("EXITO 5 LISTA_VACIA VACIA", obt, "CP-002-D08", "sacar el unico nodo");
}

static void cpD09(void)
{
    tListaD lista;
    tPosListaD pos = NULL;
    const char *corto = "corto";
    const char *largo = "un texto mas largo";
    char buf[5];
    size_t tam;
    int ret;
    int nodos = 0;
    char obt[TAM_TEXTO];

    crearListaD(&lista);
    insPrinListaD(&lista, corto, strlen(corto) + 1, &pos);
    ret = actualizarInfoListaD(pos, largo, strlen(largo) + 1);
    memset(buf, 0, sizeof(buf));
    tam = verInfoListaD(pos, buf, 4);
    recorrerListaD(&lista, contarNodo, &nodos);
    sprintf(obt, "%s %u \"%s\" nodos=%d", nombreRetorno(ret), (unsigned)tam, buf, nodos);
    vaciarListaD(&lista);
    verificarTexto("EXITO 19 \"un t\" nodos=1", obt, "CP-002-D09",
                   "actualizar con un texto mas largo y ver con buffer de 4");
}

static void cpD10(void)
{
    tListaD lista;
    tPosListaD pos[4];
    int valores[] = {1, 2, 3, 4};
    int primera;
    int segunda;
    char obt[TAM_TEXTO];

    armarListaD(&lista, valores, pos, 4);
    primera = vaciarListaD(&lista);
    segunda = vaciarListaD(&lista);
    sprintf(obt, "%d %d %s", primera, segunda, nombreEstadoLista(listaVaciaD(&lista)));
    verificarTexto("4 0 LISTA_VACIA", obt, "CP-002-D10", "vaciar dos veces");
}

// ----------------------------------------------------------------- cache: auxiliares

static int cmpInt(const void *a, const void *b)
{
    return *(const int*)a - *(const int*)b;
}

static int cmpCadena(const void *a, const void *b)
{
    return strcmp((const char*)a, (const char*)b);
}

static void chequear(const tLruD *pLru)
{
    if(!verificarLruD(pLru))
        inconsistencias++;
}

// Envoltorios: cada operacion sobre la cache va seguida de verificarLruD
static int crearD(tLruD *pLru, unsigned capacidad, tCmp cmp)
{
    int ret = crearLruD(pLru, capacidad, cmp);

    if(ret == EXITO)
        chequear(pLru);
    return ret;
}

static int agregarD(tLruD *pLru, const void *clave, size_t tamClave, const void *valor,
                    size_t tamValor)
{
    int ret = agregarLruD(pLru, clave, tamClave, valor, tamValor);

    chequear(pLru);
    return ret;
}

static int obtenerD(tLruD *pLru, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    int ret = obtenerLruD(pLru, clave, tamClave, valor, tamValor);

    chequear(pLru);
    return ret;
}

static int borrarD(tLruD *pLru, const void *clave, size_t tamClave)
{
    int ret = borrarLruD(pLru, clave, tamClave);

    chequear(pLru);
    return ret;
}

static void vaciarD(tLruD *pLru)
{
    vaciarLruD(pLru);
    chequear(pLru);
}

static int agregarTexto(tLruD *pLru, int clave, const char *valor)
{
    return agregarD(pLru, &clave, sizeof(clave), valor, strlen(valor) + 1);
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
        if(obtenerD(pLru, &claves[i], sizeof(int), valor, TAM_BUFFER) == EXITO)
            sprintf(item, "%d+%s", claves[i], valor);
        else
            sprintf(item, "%d-", claves[i]);
        anexar(texto, item);
    }
}

// Agrega claves 1..cant con valores "A", "B", "C"...
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

// Agrega las claves de claves[] con su letra ('A' + clave - 1) como valor
static void agregarClaves(tLruD *pLru, const int *claves, int cant)
{
    char valor[2];
    int i;

    valor[1] = '\0';
    for(i = 0; i < cant; i++)
    {
        valor[0] = (char)('A' + claves[i] - 1);
        agregarTexto(pLru, claves[i], valor);
    }
}

static void verificarCaso(const char *esperado, char *obt, const char *id, const char *descripcion)
{
    if(inconsistencias)
        anexar(obt, "INCONSISTENTE");
    inconsistencias = 0;
    verificarTexto(esperado, obt, id, descripcion);
}

// ----------------------------------------------------------------- cache: igual que la Parte 1

static void cp01(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};
    int ret;

    ret = crearD(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(ret));
    if(ret == EXITO)
    {
        presencia(&lru, claves, 1, obt);
        vaciarD(&lru);
    }
    verificarCaso("EXITO 1-", obt, "CP-002-01", "crear cap 3: EXITO y 1 ausente");
}

static void cp02(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int ret;

    ret = crearD(&lru, 0, cmpInt);
    if(ret == EXITO)
        vaciarD(&lru);
    anexar(obt, nombreRetorno(ret));
    verificarCaso("ERROR_PARAMETRO", obt, "CP-002-02", "crear cap 0");
}

static void cp03(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int ret;

    ret = crearD(&lru, 3, NULL);
    if(ret == EXITO)
        vaciarD(&lru);
    anexar(obt, nombreRetorno(ret));
    verificarCaso("ERROR_PARAMETRO", obt, "CP-002-03", "crear con cmp NULL");
}

static void cp04(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;

    memset(valor, 0, sizeof(valor));
    crearD(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(agregarTexto(&lru, 1, "A")));
    anexar(obt, nombreRetorno(obtenerD(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, valor);
    vaciarD(&lru);
    verificarCaso("EXITO EXITO A", obt, "CP-002-04", "agregar(1,\"A\") y obtener(1)");
}

static void cp05(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3, 4};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarD(&lru);
    verificarCaso("1- 2+B 3+C 4+D", obt, "CP-002-05", "llena: agregar(4) descarta el 1");
}

static void cp06(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;
    int claves[] = {2, 1, 3, 4};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    obtenerD(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarD(&lru);
    verificarCaso("2- 1+A 3+C 4+D", obt, "CP-002-06", "obtener(1) lo salva: se descarta el 2");
}

static void cp07(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {2, 1, 3, 4};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    agregarTexto(&lru, 1, "Z");
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarD(&lru);
    verificarCaso("2- 1+Z 3+C 4+D", obt, "CP-002-07", "reemplazar 1 con \"Z\" lo hace reciente");
}

static void cp08(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2};

    crearD(&lru, 2, cmpInt);
    llenarLetras(&lru, 2);
    agregarTexto(&lru, 2, "B2");
    presencia(&lru, claves, 2, obt);
    vaciarD(&lru);
    verificarCaso("1+A 2+B2", obt, "CP-002-08", "reemplazo con la cache llena no descarta");
}

static void cp09(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1};

    crearD(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "corto");
    agregarTexto(&lru, 1, "un texto mas largo");
    presencia(&lru, claves, 1, obt);
    vaciarD(&lru);
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
    crearD(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "un texto mas largo");
    agregarTexto(&lru, 1, "ab");
    obtenerD(&lru, &clave, sizeof(clave), valor, TAM_BUFFER);
    vaciarD(&lru);
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

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(obtenerD(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, valor);
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 1, obt);
    vaciarD(&lru);
    verificarCaso("NO_ENCONTRADO XXXX 1-", obt, "CP-002-11",
                  "obtener(9) no toca el buffer ni el orden");
}

static void cp12(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1];
    int clave = 1;

    crearD(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(obtenerD(&lru, &clave, sizeof(clave), valor, TAM_BUFFER)));
    vaciarD(&lru);
    verificarCaso("NO_ENCONTRADO", obt, "CP-002-12", "obtener en cache vacia");
}

static void cp13(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 2;
    int claves[] = {2, 1, 3, 4};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(borrarD(&lru, &clave, sizeof(clave))));
    agregarTexto(&lru, 4, "D");
    presencia(&lru, claves, 4, obt);
    vaciarD(&lru);
    verificarCaso("EXITO 2- 1+A 3+C 4+D", obt, "CP-002-13", "borrar(2) libera un lugar");
}

static void cp14(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 3;
    int claves[] = {1, 2, 4, 5};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(borrarD(&lru, &clave, sizeof(clave))));
    agregarTexto(&lru, 4, "D");
    agregarTexto(&lru, 5, "E");
    presencia(&lru, claves, 4, obt);
    vaciarD(&lru);
    verificarCaso("EXITO 1- 2+B 4+D 5+E", obt, "CP-002-14",
                  "borrar(3) y agregar 4, 5: el 1 sigue siendo el menos reciente");
}

static void cp15(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 1;
    int claves[] = {1, 2};

    crearD(&lru, 1, cmpInt);
    agregarTexto(&lru, 1, "A");
    anexar(obt, nombreRetorno(borrarD(&lru, &clave, sizeof(clave))));
    anexar(obt, nombreRetorno(agregarTexto(&lru, 2, "B")));
    presencia(&lru, claves, 2, obt);
    vaciarD(&lru);
    verificarCaso("EXITO EXITO 1- 2+B", obt, "CP-002-15", "cap 1: borrar el unico y agregar otro");
}

static void cp16(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 9;
    int claves[] = {1, 2};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 2);
    anexar(obt, nombreRetorno(borrarD(&lru, &clave, sizeof(clave))));
    presencia(&lru, claves, 2, obt);
    vaciarD(&lru);
    verificarCaso("NO_ENCONTRADO 1+A 2+B", obt, "CP-002-16", "borrar(9) inexistente");
}

static void cp17(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int clave = 1;

    crearD(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(borrarD(&lru, &clave, sizeof(clave))));
    vaciarD(&lru);
    verificarCaso("NO_ENCONTRADO", obt, "CP-002-17", "borrar en cache vacia");
}

static void cp18(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3, 4, 5, 6, 7};

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    vaciarD(&lru);
    agregarTexto(&lru, 4, "D");
    agregarTexto(&lru, 5, "E");
    agregarTexto(&lru, 6, "F");
    agregarTexto(&lru, 7, "G");
    presencia(&lru, claves, 7, obt);
    vaciarD(&lru);
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
    crearD(&lru, 3, cmpCadena);
    anexar(obt, nombreRetorno(agregarD(&lru, clave, sizeof(clave), &posteo, sizeof(posteo))));
    anexar(obt, nombreRetorno(obtenerD(&lru, clave, sizeof(clave), &leido, sizeof(leido))));
    vaciarD(&lru);
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

    crearD(&lru, 3, cmpInt);
    anexar(obt, nombreRetorno(agregarD(&lru, &clave, sizeof(clave), &valor, sizeof(valor))));
    anexar(obt, nombreRetorno(obtenerD(&lru, &clave, sizeof(clave), &leido, sizeof(leido))));
    vaciarD(&lru);
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
    crearD(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "ABCDEFGH");
    anexar(obt, nombreRetorno(obtenerD(&lru, &clave, sizeof(clave), valor, 4)));
    vaciarD(&lru);
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

    crearD(&lru, 1, cmpInt);
    llenarLetras(&lru, 2);
    presencia(&lru, claves, 2, obt);
    vaciarD(&lru);
    verificarCaso("1- 2+B", obt, "CP-002-22", "cap 1: el segundo desplaza al primero");
}

static void cp23(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {0, 450, 899, 900, 950, 999};
    int i;

    crearD(&lru, 100, cmpInt);
    for(i = 0; i < 1000; i++)
        agregarTexto(&lru, i, "v");
    presencia(&lru, claves, 6, obt);
    vaciarD(&lru);
    verificarCaso("0- 450- 899- 900+v 950+v 999+v", obt, "CP-002-23",
                  "cap 100 con 1000 claves: quedan las ultimas 100");
}

// ----------------------------------------------------------------- cache: propios de la Parte 2

static void cp26(void)
{
    tLruD lru;
    char obt[TAM_TEXTO];
    int antes;

    crearD(&lru, 10, cmpInt);
    antes = alturaIndiceLruD(&lru);
    agregarTexto(&lru, 1, "A");
    sprintf(obt, "%d %d", antes, alturaIndiceLruD(&lru));
    vaciarD(&lru);
    verificarCaso("0 1", obt, "CP-002-26", "altura vacia y con un elemento");
}

static void cp27(void)
{
    tLruD lru;
    char obt[TAM_TEXTO];
    int claves[] = {2, 1, 3};

    crearD(&lru, 10, cmpInt);
    agregarClaves(&lru, claves, 3);
    sprintf(obt, "%d", alturaIndiceLruD(&lru));
    vaciarD(&lru);
    verificarCaso("2", obt, "CP-002-27", "claves 2, 1, 3: altura 2");
}

static void cp28(void)
{
    tLruD lru;
    char obt[TAM_TEXTO];
    int i;

    crearD(&lru, 1000, cmpInt);
    for(i = 0; i < 1000; i++)
        agregarTexto(&lru, i, "v");
    sprintf(obt, "%d", alturaIndiceLruD(&lru));
    vaciarD(&lru);
    verificarCaso("1000", obt, "CP-002-28", "claves 0 a 999 en orden: el indice degenera");
}

static void cp29(void)
{
    tLruD lru;
    char obt[TAM_TEXTO];
    int claves[] = {2, 1, 3};
    int presentes[] = {2, 1, 3, 4};

    crearD(&lru, 3, cmpInt);
    agregarClaves(&lru, claves, 3);
    agregarTexto(&lru, 4, "D");
    sprintf(obt, "%d |", alturaIndiceLruD(&lru));
    presencia(&lru, presentes, 4, obt);
    vaciarD(&lru);
    verificarCaso("2 | 2- 1+A 3+C 4+D", obt, "CP-002-29",
                  "descartar la raiz del indice con dos hijos");
}

static void cp30(void)
{
    tLruD lru;
    char obt[TAM_TEXTO];
    int claves[] = {1};
    int i;

    crearD(&lru, 3, cmpInt);
    agregarTexto(&lru, 1, "A");
    for(i = 0; i < 10; i++)
        agregarTexto(&lru, 1, "B");
    sprintf(obt, "%d", alturaIndiceLruD(&lru));
    presencia(&lru, claves, 1, obt);
    vaciarD(&lru);
    verificarCaso("1 1+B", obt, "CP-002-30", "reemplazar diez veces: el indice no crece");
}

static void cp31(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    int claves[] = {2, 1, 3};

    crearD(&lru, 2, cmpInt);
    llenarLetras(&lru, 3);
    anexar(obt, nombreRetorno(agregarTexto(&lru, 1, "otra vez")));
    presencia(&lru, claves, 3, obt);
    vaciarD(&lru);
    verificarCaso("EXITO 2- 1+otra vez 3+C", obt, "CP-002-31",
                  "volver a agregar una clave descartada");
}

static void cp32(void)
{
    tLruD lru;
    char obt[TAM_TEXTO] = "";
    char num[16];
    int i;

    crearD(&lru, 3, cmpInt);
    llenarLetras(&lru, 3);
    for(i = 1; i <= 3; i++)
        anexar(obt, nombreRetorno(borrarD(&lru, &i, sizeof(i))));
    sprintf(num, "%d", alturaIndiceLruD(&lru));
    anexar(obt, num);
    vaciarD(&lru);
    verificarCaso("EXITO EXITO EXITO 0", obt, "CP-002-32", "borrar todo deja el indice vacio");
}

int pruebas002(void)
{
    int fallasAntes = cantidadFallas();

    printf("== Spec 002: LRU con lista doble e indice\n");

    // Primitivas de la lista doble (T2 a T4)
    cpD01();
    cpD02();
    cpD03();
    moverYVerificar(0, "1 3 2 ult=2 | 2 3 1", "CP-002-D04", "mover el ultimo al principio");
    moverYVerificar(1, "2 3 1 ult=1 | 1 3 2", "CP-002-D05", "mover uno del medio al principio");
    moverYVerificar(2, "3 2 1 ult=1 | 1 2 3", "CP-002-D06", "mover el que ya es primero no cambia");
    cpD07();
    cpD08();
    cpD09();
    cpD10();

    // Cache: mismo comportamiento que la Parte 1 (T6 a T8)
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

    // Cache: propios de la Parte 2 (T9)
    cp26();
    cp27();
    cp28();
    cp29();
    cp30();
    cp31();
    cp32();

    return cantidadFallas() - fallasAntes;
}
