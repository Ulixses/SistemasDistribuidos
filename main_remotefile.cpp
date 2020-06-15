#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "remotefile_imp.h"

int main (int argc,char** argv)
{
    remoteFile_imp* rfile=new remoteFile_imp();
    rfile->run();

     delete rfile;
    return 0;
}
