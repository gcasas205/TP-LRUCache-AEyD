#ifndef METRICAS_H_INCLUDED
#define METRICAS_H_INCLUDED

#include <time.h>

typedef struct
{
    long aciertos;
    long fallos;
    long lecturas;
    long posteos;
    long usuarios;
    double segundos;
} tMetricas;

void iniciarMetricas(tMetricas *pMetricas);
void registrarAcierto(tMetricas *pMetricas);
void registrarFallo(tMetricas *pMetricas);
void registrarLectura(tMetricas *pMetricas);
void registrarUsuario(tMetricas *pMetricas);
void registrarPosteo(tMetricas *pMetricas);
void fijarTiempo(tMetricas *pMetricas, clock_t inicio, clock_t fin);
double promedioPorPosteo(const tMetricas *pMetricas);
double promedioPorUsuario(const tMetricas *pMetricas);

#endif // METRICAS_H_INCLUDED
