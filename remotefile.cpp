#include "remotefile.h"
#include <stdio.h>
#include <stdlib.h>


remoteFile::remoteFile()
{

}

void remoteFile::readfile(char* filename,char** buff, unsigned long int* bufflen)
{

    FILE* fin= fopen(filename,"r");
    if(fin==NULL)
    {
        *buff=NULL;
        *bufflen=0;
    }else
    {
        unsigned long int flen=0;
        fseek(fin,0,SEEK_END);//posicionarse al final del fichero
        flen=ftell(fin);//averiguar posicion
        fseek(fin,0,SEEK_SET);//posicionarse al inicio
        *buff=new char[flen];
        fread(*buff,1,flen,fin);
        *bufflen=flen+1;
        (*buff)[flen]='\0';
        fclose(fin);
    }
}






