// Pruebas de la spec 002 (LRU con lista doble e indice). Los IDs y valores son los de
// specs/002-lru-doble/casos-prueba.md. Por ahora solo las primitivas de la lista doble (D01 a D10);
// los casos de la cache se agregan cuando exista lru_doble.

#include <stdio.h>
#include <string.h>

#include "comun.h"
#include "lista_doble.h"
#include "verificar.h"
#include "pruebas.h"

#define TAM_TEXTO 256

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

    return cantidadFallas() - fallasAntes;
}
