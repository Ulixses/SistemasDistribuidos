#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "filemanager_imp.h"
#define DEBUG 0

int main (int argc,char** argv)
{
    filemanager_imp* fmanager=new filemanager_imp();

    #if DEBUG == 1
    volatile bool salir = false;
    while (!salir)
    {
        ;
    }
    #endif

    fmanager->run();
    delete fmanager;
    return 0;
}
