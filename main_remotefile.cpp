#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "remotefile_imp.h"
#define DEBUG 0

int main (int argc,char** argv)
{
    remoteFile_imp* rfile=new remoteFile_imp();

    #if DEBUG == 1
    volatile bool salir = false;
    while (!salir)
    {
        ;
    }
    #endif

    rfile->run();

     delete rfile;
    return 0;
}
