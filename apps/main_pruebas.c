#include "pruebas.h"
#include "verificar.h"

int main()
{
    pruebas001();
    if(imprimirResumen() > 0)
        return 1;
    return 0;
}
