// Pruebas de la spec 003 (arbol indice e interfaz comun). Los IDs y valores son los de
// specs/003-arbol-e-interfaz/casos-prueba.md. Los casos 16 y 25 son de revision y no estan aca.
// Arbol: datos tReg {clave, nombre} con nombre "n" + la clave; T7 = 50 30 70 20 40 60 80.
// Interfaz: las secuencias de CP-001-06 y 07 se corren por tCache y por la primitiva directa
// (LruS / LruD) y se exige el mismo resultado (RF-11).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comun.h"
#include "arbol.h"
#include "lru_simple.h"
#include "lru_doble.h"
#include "cache.h"
#include "verificar.h"
#include "pruebas.h"

#define TAM_TEXTO 256
#define TAM_BUFFER 32
#define CANT_DEGENERADO 5000
#define CANT_PUNTEROS 5

typedef struct
{
    int clave;
    char nombre[10];
} tReg;

// Para CP-003-15: el dato apunta a memoria que el arbol no es duenio
typedef struct
{
    int clave;
    int *dato;
} tRegPtr;

// ----------------------------------------------------------------- auxiliares

// Compara el int que esta al principio de cada bloque, sin resta (no desborda)
static int cmpInt(const void *a, const void *b)
{
    int x = *(const int*)a;
    int y = *(const int*)b;

    return (x > y) - (x < y);
}

// Agrega al final de texto lo que sigue, separado por un espacio si texto no esta vacio
static void anexar(char *texto, const char *agregado)
{
    if(*texto)
        strcat(texto, " ");
    strcat(texto, agregado);
}

static void anexarInt(char *texto, const char *rotulo, int valor)
{
    char item[48];

    sprintf(item, "%s%d", rotulo, valor);
    anexar(texto, item);
}

// ----------------------------------------------------------------- arbol

static tReg armarReg(int clave)
{
    tReg reg;

    memset(&reg, 0, sizeof(reg));
    reg.clave = clave;
    sprintf(reg.nombre, "n%d", clave);
    return reg;
}

static int insertarClave(tArbol *pArbol, int clave)
{
    tReg reg = armarReg(clave);

    return insArbol(pArbol, &reg, sizeof(reg), cmpInt);
}

static int eliminarClave(tArbol *pArbol, int clave)
{
    tReg reg = armarReg(clave);

    return eliminarArbol(pArbol, &reg, cmpInt);
}

static int buscarClave(const tArbol *pArbol, int clave, tReg *destino)
{
    tReg parcial;

    memset(&parcial, 0, sizeof(parcial));
    parcial.clave = clave;                     // dato parcial: solo la clave cargada
    return buscarArbol(pArbol, &parcial, destino, sizeof(tReg), cmpInt);
}

static void acumularClave(void *info, void *contexto)
{
    char num[16];

    sprintf(num, "%d", ((tReg*)info)->clave);
    anexar((char*)contexto, num);
}

// Deja en texto las claves que imprime el recorrido en orden
static void inOrden(const tArbol *pArbol, char *texto)
{
    *texto = '\0';
    recorrerInOrdenArbol(pArbol, acumularClave, texto);
}

// Inserta T7 y devuelve cuantas inserciones no dieron EXITO
static int armarT7(tArbol *pArbol)
{
    int claves[] = {50, 30, 70, 20, 40, 60, 80};
    int i;
    int errores = 0;

    crearArbol(pArbol);
    for(i = 0; i < 7; i++)
        if(insertarClave(pArbol, claves[i]) != EXITO)
            errores++;
    return errores;
}

// Anexa "[in-orden] altura=A cant=C"
static void estadoArbol(const tArbol *pArbol, char *texto)
{
    char rec[TAM_TEXTO];
    char item[TAM_TEXTO + 8];

    inOrden(pArbol, rec);
    sprintf(item, "[%.200s]", rec);
    anexar(texto, item);
    anexarInt(texto, "altura=", alturaArbol(pArbol));
    anexarInt(texto, "cant=", cantNodosArbol(pArbol));
}

static void cp01(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    crearArbol(&arbol);
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    anexarInt(obt, "cant=", cantNodosArbol(&arbol));
    verificarTexto("altura=0 cant=0", obt, "CP-003-01", "arbol vacio: altura y cantidad");
}

static void cp02(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    anexarInt(obt, "errores=", armarT7(&arbol));
    estadoArbol(&arbol, obt);
    vaciarArbol(&arbol);
    verificarTexto("errores=0 [20 30 40 50 60 70 80] altura=3 cant=7", obt, "CP-003-02",
                   "construir T7");
}

static void cp03(void)
{
    tArbol arbol;
    tReg reg;
    tReg destino;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    reg.clave = 40;
    strcpy(reg.nombre, "otro");
    anexar(obt, nombreRetorno(insArbol(&arbol, &reg, sizeof(reg), cmpInt)));
    anexarInt(obt, "cant=", cantNodosArbol(&arbol));
    memset(&destino, 0, sizeof(destino));
    buscarClave(&arbol, 40, &destino);
    anexar(obt, destino.nombre);
    vaciarArbol(&arbol);
    verificarTexto("DUPLICADO cant=7 n40", obt, "CP-003-03", "insertar 40 duplicado no modifica");
}

static void cp04(void)
{
    tArbol arbol;
    tReg destino;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    memset(&destino, 0, sizeof(destino));
    anexar(obt, nombreRetorno(buscarClave(&arbol, 60, &destino)));
    anexarInt(obt, "", destino.clave);
    anexar(obt, destino.nombre);
    vaciarArbol(&arbol);
    verificarTexto("EXITO 60 n60", obt, "CP-003-04", "buscar 60 por dato parcial");
}

static void cp05(void)
{
    tArbol arbol;
    tReg destino;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    destino.clave = 0;
    strcpy(destino.nombre, "XXX");
    anexar(obt, nombreRetorno(buscarClave(&arbol, 65, &destino)));
    anexarInt(obt, "", destino.clave);
    anexar(obt, destino.nombre);
    vaciarArbol(&arbol);
    verificarTexto("NO_ENCONTRADO 0 XXX", obt, "CP-003-05", "buscar 65 no toca el destino");
}

static void cp06(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 20)));
    estadoArbol(&arbol, obt);
    vaciarArbol(&arbol);
    verificarTexto("EXITO [30 40 50 60 70 80] altura=3 cant=6", obt, "CP-003-06",
                   "eliminar 20 (hoja)");
}

static void cp07(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];

    armarT7(&arbol);
    eliminarClave(&arbol, 20);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 30)));
    inOrden(&arbol, rec);
    anexar(obt, rec);
    anexarInt(obt, "cant=", cantNodosArbol(&arbol));
    vaciarArbol(&arbol);
    verificarTexto("EXITO 40 50 60 70 80 cant=5", obt, "CP-003-07", "eliminar 30 (un hijo)");
}

static void cp08(void)
{
    tArbol arbol;
    tReg destino;
    char obt[TAM_TEXTO] = "";
    char rec[TAM_TEXTO];

    armarT7(&arbol);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 70)));
    inOrden(&arbol, rec);
    anexar(obt, rec);
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    anexar(obt, nombreRetorno(buscarClave(&arbol, 60, &destino)));
    anexar(obt, nombreRetorno(buscarClave(&arbol, 80, &destino)));
    vaciarArbol(&arbol);
    verificarTexto("EXITO 20 30 40 50 60 80 altura=3 EXITO EXITO", obt, "CP-003-08",
                   "eliminar 70 (dos hijos)");
}

static void cp09(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 50)));
    estadoArbol(&arbol, obt);
    vaciarArbol(&arbol);
    verificarTexto("EXITO [20 30 40 60 70 80] altura=3 cant=6", obt, "CP-003-09",
                   "eliminar 50 (raiz con dos hijos)");
}

static void cp10(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    crearArbol(&arbol);
    insertarClave(&arbol, 5);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 5)));
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    anexarInt(obt, "cant=", cantNodosArbol(&arbol));
    vaciarArbol(&arbol);
    verificarTexto("EXITO altura=0 cant=0", obt, "CP-003-10", "eliminar el unico nodo");
}

static void cp11(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 99)));
    estadoArbol(&arbol, obt);
    vaciarArbol(&arbol);
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 1)));
    verificarTexto("NO_ENCONTRADO [20 30 40 50 60 70 80] altura=3 cant=7 NO_ENCONTRADO", obt,
                   "CP-003-11", "eliminar 99 en T7 y 1 en vacio");
}

static void cp12(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";
    int k;

    crearArbol(&arbol);
    for(k = 1; k <= 3; k++)
        insertarClave(&arbol, k);
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    insertarClave(&arbol, 0);
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    anexarInt(obt, "cant=", cantNodosArbol(&arbol));
    vaciarArbol(&arbol);
    verificarTexto("altura=3 altura=3 cant=4", obt, "CP-003-12", "insertar 1, 2, 3 en orden; despues 0");
}

static void cp13(void)
{
    tArbol arbol;
    tReg destino;
    char obt[TAM_TEXTO] = "";
    int k;

    crearArbol(&arbol);
    for(k = 0; k < CANT_DEGENERADO; k++)
        insertarClave(&arbol, k);
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    anexar(obt, nombreRetorno(buscarClave(&arbol, CANT_DEGENERADO - 1, &destino)));
    anexar(obt, nombreRetorno(eliminarClave(&arbol, 0)));
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    anexarInt(obt, "vaciar=", vaciarArbol(&arbol));
    verificarTexto("altura=5000 EXITO EXITO altura=4999 vaciar=4999", obt, "CP-003-13",
                   "5000 claves en orden: arbol degenerado");
}

static void cp14(void)
{
    tArbol arbol;
    char obt[TAM_TEXTO] = "";

    armarT7(&arbol);
    anexarInt(obt, "vaciar=", vaciarArbol(&arbol));
    anexarInt(obt, "altura=", alturaArbol(&arbol));
    verificarTexto("vaciar=7 altura=0", obt, "CP-003-14", "vaciar T7");
}

static void cp15(void)
{
    tArbol arbol;
    tRegPtr reg;
    int *datos[CANT_PUNTEROS];
    char obt[TAM_TEXTO] = "";
    int k;
    int pedidos = 0;
    int suma = 0;

    crearArbol(&arbol);
    for(k = 0; k < CANT_PUNTEROS; k++)
    {
        datos[k] = (int*)malloc(sizeof(int));
        if(datos[k])
        {
            *datos[k] = (k + 1) * 100;
            reg.clave = k;
            reg.dato = datos[k];
            insArbol(&arbol, &reg, sizeof(reg), cmpInt);
            pedidos++;
        }
    }
    anexarInt(obt, "vaciar=", vaciarArbol(&arbol));
    // si el arbol hubiera liberado los dato, leerlos aca lo marca el sanitizer
    for(k = 0; k < CANT_PUNTEROS; k++)
        if(datos[k])
        {
            suma += *datos[k];
            free(datos[k]);
        }
    anexarInt(obt, "pedidos=", pedidos);
    anexarInt(obt, "suma=", suma);
    verificarTexto("vaciar=5 pedidos=5 suma=1500", obt, "CP-003-15",
                   "vaciar no libera la memoria a la que apuntan los datos");
}

// ----------------------------------------------------------------- interfaz

static int agregarTextoCache(tCache *pCache, int clave, const char *valor)
{
    return agregarCache(pCache, &clave, sizeof(clave), valor, strlen(valor) + 1);
}

// Para cada clave, en orden, anexa "k-" si esta ausente o "k+valor" si esta presente.
// Obtener cambia el orden de uso: por eso se piden primero las que se esperan ausentes.
static void presenciaCache(tCache *pCache, const int *claves, int cant, char *texto)
{
    char valor[TAM_BUFFER + 1];
    char item[TAM_BUFFER + 16];
    int i;

    for(i = 0; i < cant; i++)
    {
        memset(valor, 0, sizeof(valor));
        if(obtenerCache(pCache, &claves[i], sizeof(int), valor, TAM_BUFFER) == EXITO)
            sprintf(item, "%d+%s", claves[i], valor);
        else
            sprintf(item, "%d-", claves[i]);
        anexar(texto, item);
    }
}

// Las secuencias de CP-001-06 y 07, escritas una sola vez sobre tCache (sin ningun if por
// variante). Anexan los codigos de cada operacion y despues la presencia de 2, 1, 3 y 4.
static void secuencia06(tCache *pCache, char *texto)
{
    int claves[] = {2, 1, 3, 4};
    char valor[TAM_BUFFER + 1];
    int clave = 1;

    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 1, "A")));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 2, "B")));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 3, "C")));
    anexar(texto, nombreRetorno(obtenerCache(pCache, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 4, "D")));
    anexar(texto, "|");
    presenciaCache(pCache, claves, 4, texto);
}

static void secuencia07(tCache *pCache, char *texto)
{
    int claves[] = {2, 1, 3, 4};

    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 1, "A")));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 2, "B")));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 3, "C")));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 1, "Z")));
    anexar(texto, nombreRetorno(agregarTextoCache(pCache, 4, "D")));
    anexar(texto, "|");
    presenciaCache(pCache, claves, 4, texto);
}

// La misma secuencia contra la primitiva directa, para comparar (RF-11). Se arma un tCache
// "a mano" que apunta a una tLruS / tLruD local con adaptadores de prueba.
static int agregarDirS(void *impl, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    return agregarLruS((tLruS*)impl, clave, tamClave, valor, tamValor);
}

static int obtenerDirS(void *impl, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    return obtenerLruS((tLruS*)impl, clave, tamClave, valor, tamValor);
}

static int agregarDirD(void *impl, const void *clave, size_t tamClave, const void *valor, size_t tamValor)
{
    return agregarLruD((tLruD*)impl, clave, tamClave, valor, tamValor);
}

static int obtenerDirD(void *impl, const void *clave, size_t tamClave, void *valor, size_t tamValor)
{
    return obtenerLruD((tLruD*)impl, clave, tamClave, valor, tamValor);
}

// Corre la secuencia con la primitiva directa (variante 'S' o 'D') y deja el texto
static void secuenciaDirecta(char variante, void (*secuencia)(tCache*, char*), char *texto)
{
    tLruS lruS;
    tLruD lruD;
    tCache directa;

    memset(&directa, 0, sizeof(directa));
    *texto = '\0';
    if(variante == 'S')
    {
        crearLruS(&lruS, 3, cmpInt);
        directa.impl = &lruS;
        directa.agregar = agregarDirS;
        directa.obtener = obtenerDirS;
        secuencia(&directa, texto);
        vaciarLruS(&lruS);
    }
    else
    {
        crearLruD(&lruD, 3, cmpInt);
        directa.impl = &lruD;
        directa.agregar = agregarDirD;
        directa.obtener = obtenerDirD;
        secuencia(&directa, texto);
        vaciarLruD(&lruD);
    }
}

// Corre la secuencia por la interfaz y verifica el esperado; si difiere de la primitiva
// directa, lo agrega al obtenido para que el caso de FALLA
static void compararConDirecta(int (*crear)(tCache*, unsigned, tCmp), char variante,
                               void (*secuencia)(tCache*, char*), const char *esperado,
                               const char *id, const char *descripcion)
{
    tCache cache;
    char obt[TAM_TEXTO] = "";
    char dir[TAM_TEXTO];
    char total[2 * TAM_TEXTO + 32];

    if(crear(&cache, 3, cmpInt) != EXITO)
    {
        verificarTexto(esperado, "no se pudo crear la cache", id, descripcion);
        return;
    }
    secuencia(&cache, obt);
    vaciarCache(&cache);
    secuenciaDirecta(variante, secuencia, dir);
    if(strcmp(obt, dir) != 0)
    {
        sprintf(total, "%s [primitiva directa: %s]", obt, dir);
        verificarTexto(esperado, total, id, descripcion);
    }
    else
        verificarTexto(esperado, obt, id, descripcion);
}

static void cp17(void)
{
    tCache cache;
    char obt[TAM_TEXTO] = "";
    char valor[TAM_BUFFER + 1] = "sin tocar";
    int clave = 1;

    crearSinCache(&cache);
    anexar(obt, nombreRetorno(agregarTextoCache(&cache, 1, "A")));
    anexar(obt, nombreRetorno(obtenerCache(&cache, &clave, sizeof(clave), valor, TAM_BUFFER)));
    anexar(obt, nombreRetorno(borrarCache(&cache, &clave, sizeof(clave))));
    anexar(obt, nombreCache(&cache));
    anexarInt(obt, "altura=", alturaIndiceCache(&cache));
    vaciarCache(&cache);
    verificarTexto("EXITO NO_ENCONTRADO NO_ENCONTRADO Sin cache altura=-1", obt, "CP-003-17",
                   "sin cache: no guarda nada");
}

static void cp18(void)
{
    const char *esperado = "EXITO EXITO EXITO EXITO EXITO | 2- 1+A 3+C 4+D";

    compararConDirecta(crearCacheSimple, 'S', secuencia06, esperado, "CP-003-18",
                       "secuencia de CP-001-06 por la interfaz (simple)");
    compararConDirecta(crearCacheDoble, 'D', secuencia06, esperado, "CP-003-18",
                       "secuencia de CP-001-06 por la interfaz (doble)");
}

static void cp19(void)
{
    const char *esperado = "EXITO EXITO EXITO EXITO EXITO | 2- 1+Z 3+C 4+D";

    compararConDirecta(crearCacheSimple, 'S', secuencia07, esperado, "CP-003-19",
                       "secuencia de CP-001-07 por la interfaz (simple)");
    compararConDirecta(crearCacheDoble, 'D', secuencia07, esperado, "CP-003-19",
                       "secuencia de CP-001-07 por la interfaz (doble)");
}

static void cp20(void)
{
    tCache simple;
    tCache doble;
    char obt[TAM_TEXTO] = "";

    crearCacheSimple(&simple, 3, cmpInt);
    crearCacheDoble(&doble, 3, cmpInt);
    anexar(obt, nombreCache(&simple));
    anexar(obt, "/");
    anexar(obt, nombreCache(&doble));
    vaciarCache(&simple);
    vaciarCache(&doble);
    verificarTexto("Lista simple / Lista doble + ABB", obt, "CP-003-20", "nombre de cada variante");
}

static void cp21(void)
{
    tCache simple;
    tCache doble;
    char obt[TAM_TEXTO] = "";

    crearCacheSimple(&simple, 10, cmpInt);
    crearCacheDoble(&doble, 10, cmpInt);
    agregarTextoCache(&simple, 2, "B");
    agregarTextoCache(&simple, 1, "A");
    agregarTextoCache(&simple, 3, "C");
    agregarTextoCache(&doble, 2, "B");
    agregarTextoCache(&doble, 1, "A");
    agregarTextoCache(&doble, 3, "C");
    anexarInt(obt, "simple=", alturaIndiceCache(&simple));
    anexarInt(obt, "doble=", alturaIndiceCache(&doble));
    vaciarCache(&simple);
    vaciarCache(&doble);
    verificarTexto("simple=-1 doble=2", obt, "CP-003-21", "altura del indice: agregar 2, 1, 3");
}

static void cp22(void)
{
    tCache cache;
    char obt[TAM_TEXTO] = "";
    int ret;

    ret = crearCacheSimple(&cache, 0, cmpInt);
    anexar(obt, nombreRetorno(ret));
    if(ret == EXITO)
        vaciarCache(&cache);
    ret = crearCacheDoble(&cache, 3, NULL);
    anexar(obt, nombreRetorno(ret));
    if(ret == EXITO)
        vaciarCache(&cache);
    verificarTexto("ERROR_PARAMETRO ERROR_PARAMETRO", obt, "CP-003-22",
                   "crear simple cap 0 y doble con cmp NULL");
}

static void vaciarYRecrear(int (*crear)(tCache*, unsigned, tCmp), const char *id,
                           const char *descripcion)
{
    tCache cache;
    char obt[TAM_TEXTO] = "";
    int claves[] = {1, 2, 3};

    crear(&cache, 3, cmpInt);
    agregarTextoCache(&cache, 1, "A");
    agregarTextoCache(&cache, 2, "B");
    agregarTextoCache(&cache, 3, "C");
    vaciarCache(&cache);
    anexar(obt, nombreRetorno(crear(&cache, 2, cmpInt)));
    agregarTextoCache(&cache, 1, "A");
    agregarTextoCache(&cache, 2, "B");
    agregarTextoCache(&cache, 3, "C");
    presenciaCache(&cache, claves, 3, obt);
    vaciarCache(&cache);
    verificarTexto("EXITO 1- 2+B 3+C", obt, id, descripcion);
}

static void cp23(void)
{
    vaciarYRecrear(crearCacheSimple, "CP-003-23", "vaciar y volver a crear cap 2 (simple)");
    vaciarYRecrear(crearCacheDoble, "CP-003-23", "vaciar y volver a crear cap 2 (doble)");
}

// CP-003-24: esta funcion recibe cualquier tCache y no pregunta cual es
static void usarCualquiera(tCache *pCache, char *texto)
{
    char valor[TAM_BUFFER + 1];
    int k;

    for(k = 1; k <= 3; k++)
        agregarTextoCache(pCache, k, "x");
    for(k = 1; k <= 3; k++)
        anexar(texto, nombreRetorno(obtenerCache(pCache, &k, sizeof(k), valor, TAM_BUFFER)));
}

static void cp24(void)
{
    tCache cache;
    char obt[TAM_TEXTO] = "";

    crearSinCache(&cache);
    usarCualquiera(&cache, obt);
    vaciarCache(&cache);
    anexar(obt, "|");
    crearCacheSimple(&cache, 3, cmpInt);
    usarCualquiera(&cache, obt);
    vaciarCache(&cache);
    anexar(obt, "|");
    crearCacheDoble(&cache, 3, cmpInt);
    usarCualquiera(&cache, obt);
    vaciarCache(&cache);
    verificarTexto("NO_ENCONTRADO NO_ENCONTRADO NO_ENCONTRADO | EXITO EXITO EXITO | EXITO EXITO EXITO",
                   obt, "CP-003-24", "la misma funcion con las tres variantes");
}

// ------------------------------------------------------- funcion de la spec

int pruebas003(void)
{
    int fallasAntes = cantidadFallas();

    printf("== Spec 003: arbol indice e interfaz comun\n");

    // Arbol (T3 a T7)
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

    // Interfaz (T8 y T9)
    cp17();
    cp18();
    cp19();
    cp20();
    cp21();
    cp22();
    cp23();
    cp24();

    return cantidadFallas() - fallasAntes;
}
