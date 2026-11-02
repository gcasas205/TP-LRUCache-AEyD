#include "metricas.h"

void iniciarMetricas(tMetricas *pMetricas)
{
    pMetricas->aciertos = 0;
    pMetricas->fallos = 0;
    pMetricas->lecturas = 0;
    pMetricas->posteos = 0;
    pMetricas->usuarios = 0;
    pMetricas->segundos = 0.0;
}

void registrarAcierto(tMetricas *pMetricas)
{
    pMetricas->aciertos++;
}

void registrarFallo(tMetricas *pMetricas)
{
    pMetricas->fallos++;
}

void registrarLectura(tMetricas *pMetricas)
{
    pMetricas->lecturas++;
}

void registrarUsuario(tMetricas *pMetricas)
{
    pMetricas->usuarios++;
}

void registrarPosteo(tMetricas *pMetricas)
{
    pMetricas->posteos++;
}

void fijarTiempo(tMetricas *pMetricas, clock_t inicio, clock_t fin)
{
    pMetricas->segundos = (double)(fin - inicio) / CLOCKS_PER_SEC;
}

double promedioPorPosteo(const tMetricas *pMetricas)
{
    if(!pMetricas->posteos)
        return 0.0;

    return pMetricas->segundos / (double)pMetricas->posteos;
}

double promedioPorUsuario(const tMetricas *pMetricas)
{
    if(!pMetricas->usuarios)
        return 0.0;

    return pMetricas->segundos / (double)pMetricas->usuarios;
}
