#include "pruebas.h"
#include "verificar.h"

int main()
{
    pruebas001();
    pruebas002();
    pruebas003();
    if(imprimirResumen() > 0)
        return 1;
    return 0;
}
