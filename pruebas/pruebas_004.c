
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "comun.h"
#include "posteo.h"
#include "usuario.h"
#include "generador.h"
#include "metricas.h"
#include "simulador.h"
#include "cache.h"
#include "verificar.h"
#include "pruebas.h"

#define TAM_TEXTO 256
#define USUARIOS_LOTE 200
#define G_CONCENTRADO 300
#define USUARIOS_GRANDE 50000

#define RUTA_POSTEOS_GRANDE "bin/cp004_posteos50000.dat"
#define RUTA_POSTEOS_MINI "bin/cp004_posteos.dat"
#define RUTA_MINI "bin/cp004_mini.dat"
#define RUTA_MINI_FUERA "bin/cp004_mini_fuera.dat"
#define RUTA_A "bin/cp004_a.dat"
#define RUTA_B "bin/cp004_b.dat"
#define RUTA_C "bin/cp004_c.dat"
#define RUTA_CONCENTRADO "bin/cp004_concentrado.dat"
#define RUTA_DISPERSO "bin/cp004_disperso.dat"
#define RUTA_ORDENADO "bin/cp004_ordenado.dat"
#define RUTA_ORDENADO_GRANDE "bin/cp004_ordenado50000.dat"
#define RUTA_CHICO "bin/cp004_chico.dat"
#define RUTA_NO_EXISTE "bin/cp004_no_existe.dat"

static void anexar(char *texto, const char *agregado)
{
    if(*texto)
        strcat(texto, " ");
    strcat(texto, agregado);
}

static void anexarLong(char *texto, const char *rotulo, long valor)
{
    char item[48];

    sprintf(item, "%s%ld", rotulo, valor);
    anexar(texto, item);
}

static int existeArchivo(const char *ruta)
{
    FILE *fp = fopen(ruta, "rb");

    if(!fp)
        return 0;
    fclose(fp);
    return 1;
}

static long tamArchivo(const char *ruta)
{
    FILE *fp = fopen(ruta, "rb");
    long tam;

    if(!fp)
        return -1L;
    fseek(fp, 0L, SEEK_END);
    tam = ftell(fp);
    fclose(fp);
    return tam;
}

static int archivosIguales(const char *rutaA, const char *rutaB)
{
    FILE *fa = fopen(rutaA, "rb");
    FILE *fb = fopen(rutaB, "rb");
    int ca;
    int cb;
    int iguales = 1;

    if(!fa || !fb)
    {
        if(fa)
            fclose(fa);
        if(fb)
            fclose(fb);
        return 0;
    }
    do
    {
        ca = fgetc(fa);
        cb = fgetc(fb);
        if(ca != cb)
            iguales = 0;
    } while(iguales && ca != EOF);
    fclose(fa);
    fclose(fb);
    return iguales;
}

typedef struct
{
    long usuarios;
    long nombresMal;
    long fueraDeRango;
    long mayorQue;
} tResumenLote;

static int resumirLote(const char *ruta, int limite, tResumenLote *res)
{
    FILE *fp = fopen(ruta, "rb");
    tUsuario usr;
    int i;

    memset(res, 0, sizeof(*res));
    if(!fp)
        return ERROR_ARCHIVO;
    while(leerUsuario(fp, &usr) == EXITO)
    {
        res->usuarios++;
        if(!memchr(usr.usuario, '\0', TAM_NOMBRE_USUARIO))
            res->nombresMal++;
        for(i = 0; i < CANT_FEED; i++)
        {
            if(usr.feed[i] < 0 || usr.feed[i] >= CANT_POSTEOS)
                res->fueraDeRango++;
            if(usr.feed[i] >= limite)
                res->mayorQue++;
        }
    }
    fclose(fp);
    return EXITO;
}

static int posteosGrandeOk = 0;

static void cp01(void)
{
    FILE *fp;
    tPosteo posteo;
    char obt[TAM_TEXTO] = "";
    int ids[] = {0, 25000, 49999};
    int i;
    int bien = 0;
    int ret;

    ret = generarPosteos(RUTA_POSTEOS_GRANDE, CANT_POSTEOS);
    anexar(obt, nombreRetorno(ret));
    anexar(obt, tamArchivo(RUTA_POSTEOS_GRANDE) == (long)CANT_POSTEOS * (long)sizeof(tPosteo) ?
           "tam=ok" : "tam=mal");
    if((fp = fopen(RUTA_POSTEOS_GRANDE, "rb")) != NULL)
    {
        for(i = 0; i < 3; i++)
            if(leerPosteoPorId(fp, ids[i], &posteo) == EXITO && posteo.id == ids[i] &&
               memchr(posteo.texto, '\0', TAM_TEXTO_POSTEO))
                bien++;
        fclose(fp);
    }
    anexarLong(obt, "registros_ok=", bien);
    posteosGrandeOk = (ret == EXITO);
    verificarTexto("EXITO tam=ok registros_ok=3", obt, "CP-004-01",
                   "50000 posteos: tamanio, ids y texto con '\\0'");
}

static void cp02(void)
{
    char obt[TAM_TEXTO] = "";

    srand(123);
    generarLote(RUTA_A, USUARIOS_LOTE, feedDisperso, NULL);
    srand(123);
    generarLote(RUTA_B, USUARIOS_LOTE, feedDisperso, NULL);
    srand(124);
    generarLote(RUTA_C, USUARIOS_LOTE, feedDisperso, NULL);
    anexar(obt, archivosIguales(RUTA_A, RUTA_B) ? "a=b" : "a!=b");
    anexar(obt, existeArchivo(RUTA_C) && !archivosIguales(RUTA_A, RUTA_C) ? "c!=a" : "c=a");
    verificarTexto("a=b c!=a", obt, "CP-004-02", "misma semilla: mismo lote; otra semilla: distinto");
}

static void generarLotesChicos(void)
{
    int grupo = G_CONCENTRADO;
    int usuarios = USUARIOS_LOTE;

    srand(123);
    generarLote(RUTA_CONCENTRADO, USUARIOS_LOTE, feedConcentrado, &grupo);
    generarLote(RUTA_DISPERSO, USUARIOS_LOTE, feedDisperso, NULL);
    generarLote(RUTA_ORDENADO, USUARIOS_LOTE, feedOrdenado, &usuarios);
}

static void cp03(void)
{
    const char *rutas[] = {RUTA_CONCENTRADO, RUTA_DISPERSO, RUTA_ORDENADO};
    tResumenLote res;
    char obt[TAM_TEXTO] = "";
    char item[64];
    int i;

    for(i = 0; i < 3; i++)
    {
        resumirLote(rutas[i], CANT_POSTEOS, &res);
        sprintf(item, "[%ld %ld %ld]", res.usuarios, res.nombresMal, res.fueraDeRango);
        anexar(obt, item);
    }
    verificarTexto("[200 0 0] [200 0 0] [200 0 0]", obt, "CP-004-03",
                   "tres lotes: 200 usuarios, nombres con '\\0', ids en rango");
}

static void cp04(void)
{
    tResumenLote res;
    char obt[TAM_TEXTO] = "";

    resumirLote(RUTA_CONCENTRADO, G_CONCENTRADO, &res);
    anexarLong(obt, "usuarios=", res.usuarios);
    anexarLong(obt, "ids>=300:", res.mayorQue);
    verificarTexto("usuarios=200 ids>=300:0", obt, "CP-004-04", "concentrado G=300: todos los ids < 300");
}

static void cp05(void)
{
    static char visto[CANT_POSTEOS];
    FILE *fp;
    tUsuario usr;
    char obt[TAM_TEXTO] = "";
    int decimas[10];
    long distintos = 0;
    long total = 0;
    int vacias = 0;
    int i;

    memset(visto, 0, sizeof(visto));
    memset(decimas, 0, sizeof(decimas));
    if((fp = fopen(RUTA_DISPERSO, "rb")) != NULL)
    {
        while(leerUsuario(fp, &usr) == EXITO)
            for(i = 0; i < CANT_FEED; i++)
                if(usr.feed[i] >= 0 && usr.feed[i] < CANT_POSTEOS)
                {
                    total++;
                    if(!visto[usr.feed[i]])
                    {
                        visto[usr.feed[i]] = 1;
                        distintos++;
                    }
                    decimas[usr.feed[i] / (CANT_POSTEOS / 10)]++;
                }
        fclose(fp);
    }
    for(i = 0; i < 10; i++)
        if(!decimas[i])
            vacias++;
    anexarLong(obt, "total=", total);
    anexar(obt, distintos >= 3600 ? "distintos>=3600" : "distintos<3600");
    anexarLong(obt, "decimas_vacias=", vacias);
    anexarLong(obt, "ultima_decima=", decimas[9] > 0);
    verificarTexto("total=4000 distintos>=3600 decimas_vacias=0 ultima_decima=1", obt, "CP-004-05",
                   "disperso: ids distintos y en todo el rango");
}

static void cp06(void)
{
    FILE *fp;
    tUsuario usr;
    char obt[TAM_TEXTO] = "";
    long cant = 0;
    long saltosMal = 0;
    int primero = -1;
    int anterior = -1;
    int i;

    if((fp = fopen(RUTA_ORDENADO, "rb")) != NULL)
    {
        while(leerUsuario(fp, &usr) == EXITO)
            for(i = 0; i < CANT_FEED; i++)
            {
                if(!cant)
                    primero = usr.feed[i];
                else if(usr.feed[i] - anterior != 12 && usr.feed[i] - anterior != 13)
                    saltosMal++;
                anterior = usr.feed[i];
                cant++;
            }
        fclose(fp);
    }
    anexarLong(obt, "pedidos=", cant);
    anexarLong(obt, "primero=", primero);
    anexarLong(obt, "ultimo=", anterior);
    anexarLong(obt, "saltos_mal=", saltosMal);
    verificarTexto("pedidos=4000 primero=0 ultimo=49987 saltos_mal=0", obt, "CP-004-06",
                   "ordenado de 200: proporcional, saltos de 12 o 13");
}

static void cp07(void)
{
    char obt[TAM_TEXTO] = "";

    remove(RUTA_CHICO);
    anexar(obt, nombreRetorno(generarLote(RUTA_CHICO, 150, feedDisperso, NULL)));
    anexar(obt, existeArchivo(RUTA_CHICO) ? "archivo=existe" : "archivo=no");
    verificarTexto("ERROR_PARAMETRO archivo=no", obt, "CP-004-07", "lote de 150 usuarios: no se crea");
}

static void cp09(void)
{
    FILE *fp;
    tPosteo posteo;
    char obt[TAM_TEXTO] = "";

    if(!posteosGrandeOk || (fp = fopen(RUTA_POSTEOS_GRANDE, "rb")) == NULL)
    {
        verificarTexto("EXITO id=49999 NO_ENCONTRADO NO_ENCONTRADO", "sin archivo de CP-004-01",
                       "CP-004-09", "leerPosteoPorId: ultimo, uno mas y -1");
        return;
    }
    posteo.id = -7;
    anexar(obt, nombreRetorno(leerPosteoPorId(fp, 49999, &posteo)));
    anexarLong(obt, "id=", posteo.id);
    anexar(obt, nombreRetorno(leerPosteoPorId(fp, 50000, &posteo)));
    anexar(obt, nombreRetorno(leerPosteoPorId(fp, -1, &posteo)));
    fclose(fp);
    verificarTexto("EXITO id=49999 NO_ENCONTRADO NO_ENCONTRADO", obt, "CP-004-09",
                   "leerPosteoPorId: ultimo, uno mas y -1");
}

static void cp10(void)
{
    char obt[TAM_TEXTO] = "";
    int pares[4][2];
    int i;
    int r;

    pares[0][0] = 3;
    pares[0][1] = 7;
    pares[1][0] = 7;
    pares[1][1] = 7;
    pares[2][0] = 7;
    pares[2][1] = 3;
    pares[3][0] = INT_MIN;
    pares[3][1] = 1;
    for(i = 0; i < 4; i++)
    {
        r = cmpIdPosteo(&pares[i][0], &pares[i][1]);
        anexar(obt, r < 0 ? "neg" : (r > 0 ? "pos" : "0"));
    }
    verificarTexto("neg 0 pos neg", obt, "CP-004-10", "cmpIdPosteo sin desborde");
}

static void cp27(void)
{
    FILE *fp;
    tPosteo directo;
    tPosteo conSeek;
    char obt[TAM_TEXTO] = "";
    int ids[] = {0, 25000, 49999};
    int i;
    int iguales = 0;

    if(!posteosGrandeOk || (fp = fopen(RUTA_POSTEOS_GRANDE, "rb")) == NULL)
    {
        verificarTexto("iguales=3 NO_ENCONTRADO", "sin archivo de CP-004-01", "CP-004-27",
                       "leerPosteoConSeek lee lo mismo que el directo");
        return;
    }
    for(i = 0; i < 3; i++)
    {
        memset(&directo, 0, sizeof(directo));
        memset(&conSeek, 0, sizeof(conSeek));
        if(leerPosteoConSeek(fp, ids[i], &conSeek) == EXITO &&
           leerPosteoPorId(fp, ids[i], &directo) == EXITO &&
           memcmp(&directo, &conSeek, sizeof(tPosteo)) == 0)
            iguales++;
    }
    anexarLong(obt, "iguales=", iguales);
    anexar(obt, nombreRetorno(leerPosteoConSeek(fp, 50000, &conSeek)));
    fclose(fp);
    verificarTexto("iguales=3 NO_ENCONTRADO", obt, "CP-004-27", "leerPosteoConSeek lee lo mismo que el directo");
}

static void cp28(void)
{
    FILE *fp;
    tUsuario usr;
    char obt[TAM_TEXTO] = "";
    int usuarios = USUARIOS_GRANDE;
    long cant = 0;
    long decrece = 0;
    long rachasMal = 0;
    long racha = 0;
    int primero = -1;
    int anterior = -1;
    int i;

    generarLote(RUTA_ORDENADO_GRANDE, USUARIOS_GRANDE, feedOrdenado, &usuarios);
    if((fp = fopen(RUTA_ORDENADO_GRANDE, "rb")) != NULL)
    {
        while(leerUsuario(fp, &usr) == EXITO)
            for(i = 0; i < CANT_FEED; i++)
            {
                if(!cant)
                {
                    primero = usr.feed[i];
                    racha = 1;
                }
                else if(usr.feed[i] == anterior)
                    racha++;
                else
                {
                    if(usr.feed[i] < anterior)
                        decrece++;
                    if(racha != CANT_FEED)
                        rachasMal++;
                    racha = 1;
                }
                anterior = usr.feed[i];
                cant++;
            }
        fclose(fp);
        if(cant && racha != CANT_FEED)
            rachasMal++;
    }
    remove(RUTA_ORDENADO_GRANDE);
    anexarLong(obt, "pedidos=", cant);
    anexarLong(obt, "primero=", primero);
    anexarLong(obt, "ultimo=", anterior);
    anexarLong(obt, "decrece=", decrece);
    anexarLong(obt, "rachas_mal=", rachasMal);
    verificarTexto("pedidos=1000000 primero=0 ultimo=49999 decrece=0 rachas_mal=0", obt, "CP-004-28",
                   "ordenado de 50000 usuarios: cada id 20 veces seguidas, sin desborde");
}

static void cp11(void)
{
    tMetricas m;
    char obt[TAM_TEXTO];

    iniciarMetricas(&m);
    sprintf(obt, "%ld %ld %ld %ld %ld %.1f %.1f %.1f", m.aciertos, m.fallos, m.lecturas, m.posteos,
            m.usuarios, m.segundos, promedioPorPosteo(&m), promedioPorUsuario(&m));
    verificarTexto("0 0 0 0 0 0.0 0.0 0.0", obt, "CP-004-11", "metricas en cero, promedios sin dividir por 0");
}

static void cp12(void)
{
    tMetricas m;
    char obt[TAM_TEXTO];
    int i;

    iniciarMetricas(&m);
    for(i = 0; i < 4000; i++)
        registrarPosteo(&m);
    for(i = 0; i < 200; i++)
        registrarUsuario(&m);
    fijarTiempo(&m, (clock_t)0, (clock_t)(2 * CLOCKS_PER_SEC));
    sprintf(obt, "%.4f %.2f", promedioPorPosteo(&m), promedioPorUsuario(&m));
    verificarTexto("0.0005 0.01", obt, "CP-004-12", "promedios a partir del total");
}

static int escribirMini(const char *ruta, int idFuera)
{
    FILE *fp = fopen(ruta, "wb");
    tUsuario usr;
    int inicio[] = {0, 0, 20};
    int u;
    int i;

    if(!fp)
        return ERROR_ARCHIVO;
    for(u = 0; u < 3; u++)
    {
        memset(&usr, 0, sizeof(usr));
        sprintf(usr.usuario, "mini%c", 'A' + u);
        for(i = 0; i < CANT_FEED; i++)
            usr.feed[i] = inicio[u] + i;
        if(u == 0 && idFuera >= 0)
            usr.feed[CANT_FEED - 1] = idFuera;
        escribirUsuario(fp, &usr);
    }
    fclose(fp);
    return EXITO;
}

static int miniListo = 0;

static void prepararMini(void)
{
    miniListo = generarPosteos(RUTA_POSTEOS_MINI, 50) == EXITO &&
                escribirMini(RUTA_MINI, -1) == EXITO &&
                escribirMini(RUTA_MINI_FUERA, 9999) == EXITO;
}

static int correrMini(char variante, unsigned capacidad, tLectorPosteo lector, const char *rutaLote,
                      tMetricas *m, int *altura)
{
    tCache cache;
    int ret;

    iniciarMetricas(m);
    *altura = -1;
    if(variante == 'S')
        ret = crearCacheSimple(&cache, capacidad, cmpIdPosteo);
    else if(variante == 'D')
        ret = crearCacheDoble(&cache, capacidad, cmpIdPosteo);
    else
    {
        crearSinCache(&cache);
        ret = EXITO;
    }
    if(ret != EXITO)
        return ret;
    ret = simular(RUTA_POSTEOS_MINI, rutaLote, &cache, lector, m);
    *altura = alturaIndiceCache(&cache);
    vaciarCache(&cache);
    return ret;
}

static void textoMetricas(int ret, const tMetricas *m, char *texto)
{
    sprintf(texto, "%s p=%ld u=%ld a=%ld f=%ld l=%ld", nombreRetorno(ret), m->posteos, m->usuarios,
            m->aciertos, m->fallos, m->lecturas);
}

static void casoMini(char variante, unsigned capacidad, tLectorPosteo lector, const char *rutaLote,
                     const char *esperado, const char *id, const char *descripcion)
{
    tMetricas m;
    char obt[TAM_TEXTO];
    int altura;
    int ret;

    if(!miniListo)
    {
        verificarTexto(esperado, "no se pudo preparar el lote mini (generarPosteos)", id, descripcion);
        return;
    }
    ret = correrMini(variante, capacidad, lector, rutaLote, &m, &altura);
    textoMetricas(ret, &m, obt);
    verificarTexto(esperado, obt, id, descripcion);
}

static void cp16(void)
{
    casoMini('D', 20, leerPosteoPorId, RUTA_MINI, "EXITO p=60 u=3 a=20 f=40 l=40", "CP-004-16",
             "Parte 2 cap 20: igual que la Parte 1 (CP-004-14)");
    casoMini('D', 10, leerPosteoPorId, RUTA_MINI, "EXITO p=60 u=3 a=0 f=60 l=60", "CP-004-16",
             "Parte 2 cap 10: igual que la Parte 1 (CP-004-15)");
}

static void cp17(void)
{
    tMetricas m;
    char obt[TAM_TEXTO] = "";
    int altura = -1;

    if(miniListo)
        correrMini('D', 20, leerPosteoPorId, RUTA_MINI, &m, &altura);
    anexarLong(obt, "altura=", altura);
    verificarTexto("altura=20", obt, "CP-004-17", "Parte 2 cap 20: el indice queda como cadena");
}

static void cp18(void)
{
    tMetricas m1;
    tMetricas m2;
    char t1[TAM_TEXTO];
    char t2[TAM_TEXTO];
    char obt[2 * TAM_TEXTO + 8];
    int altura;
    int r1;
    int r2;

    if(!miniListo)
    {
        verificarTexto("iguales", "no se pudo preparar el lote mini", "CP-004-18",
                       "Parte 1 cap 20 dos veces: mismos numeros");
        return;
    }
    r1 = correrMini('S', 20, leerPosteoPorId, RUTA_MINI, &m1, &altura);
    r2 = correrMini('S', 20, leerPosteoPorId, RUTA_MINI, &m2, &altura);
    textoMetricas(r1, &m1, t1);
    textoMetricas(r2, &m2, t2);
    if(strcmp(t1, t2) == 0 && m1.posteos == 60)
        strcpy(obt, "iguales");
    else
        sprintf(obt, "%s / %s", t1, t2);
    verificarTexto("iguales", obt, "CP-004-18", "Parte 1 cap 20 dos veces: mismos numeros");
}

static void cp19(void)
{
    tMetricas m;
    char obt[TAM_TEXTO] = "";
    double esperadoPosteo;
    double esperadoUsuario;
    int altura;

    if(miniListo)
        correrMini('S', 20, leerPosteoPorId, RUTA_MINI, &m, &altura);
    else
        iniciarMetricas(&m);
    m.segundos = 0.6;
    esperadoPosteo = m.posteos ? m.segundos / 60.0 : -1.0;
    esperadoUsuario = m.usuarios ? m.segundos / 3.0 : -1.0;
    anexar(obt, promedioPorPosteo(&m) == esperadoPosteo ? "posteo=total/60" : "posteo=mal");
    anexar(obt, promedioPorUsuario(&m) == esperadoUsuario ? "usuario=total/3" : "usuario=mal");
    verificarTexto("posteo=total/60 usuario=total/3", obt, "CP-004-19",
                   "promedios de una corrida: total / posteos y total / usuarios");
}

static void cp20(void)
{
    tCache cache;
    tMetricas m;

    crearSinCache(&cache);
    verificarTexto("ERROR_ARCHIVO",
                   nombreRetorno(simular(RUTA_NO_EXISTE, RUTA_MINI, &cache, leerPosteoPorId, &m)),
                   "CP-004-20", "simular sin archivo de posteos");
    vaciarCache(&cache);
}

int pruebas004(void)
{
    int fallasAntes = cantidadFallas();

    printf("== Spec 004: generador y simulacion\n");

    cp10();
    cp11();
    cp12();

    cp01();
    cp09();
    cp27();
    cp02();
    generarLotesChicos();
    cp03();
    cp04();
    cp05();
    cp06();
    cp07();
    cp28();

    prepararMini();
    casoMini('N', 0, leerPosteoPorId, RUTA_MINI, "EXITO p=60 u=3 a=0 f=60 l=60", "CP-004-13",
             "sin cache: todo es fallo y lectura (el calentamiento no suma)");
    casoMini('S', 20, leerPosteoPorId, RUTA_MINI, "EXITO p=60 u=3 a=20 f=40 l=40", "CP-004-14",
             "Parte 1 cap 20: B encuentra lo que trajo A");
    casoMini('S', 10, leerPosteoPorId, RUTA_MINI, "EXITO p=60 u=3 a=0 f=60 l=60", "CP-004-15",
             "Parte 1 cap 10: el feed es mas largo que la cache");
    cp16();
    cp17();
    cp18();
    cp19();
    casoMini('S', 20, leerPosteoConSeek, RUTA_MINI, "EXITO p=60 u=3 a=20 f=40 l=40", "CP-004-29",
             "Parte 1 cap 20 con seek: mismos numeros que el directo");
    cp20();
    casoMini('S', 20, leerPosteoPorId, RUTA_MINI_FUERA, "EXITO p=60 u=3 a=19 f=41 l=41", "CP-004-26",
             "id 9999 fuera de rango: fallo y lectura, no entra en la cache");

    return cantidadFallas() - fallasAntes;
}
