#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "pruebaclase_imp.h"
#define DEBUG 0

int main (int argc,char** argv)
{
    pruebaClase_imp* pclase=new pruebaClase_imp();

    #if DEBUG == 1
    volatile bool salir = false;
    while (!salir)
    {
        ;
    }
    #endif

    pclase->run();

     delete pclase;
    return 0;
}
