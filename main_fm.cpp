#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "filemanager_stub.h"
#define DEBUG 0
using namespace std;

int main (int argc,char** argv)
{
    cout << "Pre Debug\n";
    #if DEBUG == 1
    volatile bool salir = false;
    while (!salir)
    {
        ;
    }
    #endif
    cout << "Inicio\n";
    filemanager_stub* fm=new filemanager_stub("10.0.2.23");
    vector<string*>* vfiles=fm->listFiles();
    cout<<"Lista de ficheros en el directorio de prueba:\n";
    for(unsigned int i=0;i<vfiles->size();++i)
    {
        cout<<"Fichero: "<<vfiles->at(i)->c_str()<<endl;
    }

    cout<<"Leyendo el primer fichero del directorio de prueba:\n";
    char* data=nullptr;
    unsigned long int fileLen=0;
    fm->readFile(&(*(vfiles->at(0)))[0],data,fileLen);
    cout<<"Contenido del primer directorio:" << data<< "\n";

    cout<<"Escribiendo el primer fichero del directorio de prueba:\n";
    fm->writeFile("f4.txt",data,fileLen);

    cout<<"Liberando lista de ficheros:\n";
    fm->freeListedFiles(vfiles);
    cout<<"Liberando datos de fichero leído:\n";
    vfiles=fm->listFiles();
    cout<<"Lista de ficheros en el directorio de prueba:\n";
    for(unsigned int i=0;i<vfiles->size();++i)
    {
        cout<<"Fichero: "<<vfiles->at(i)->c_str()<<endl;
    }
    cout<<"Liberando lista de ficheros:\n";
    fm->freeListedFiles(vfiles);
    cout<<"Liberando datos de fichero leído:\n";
    delete[] data;
    delete fm;
    MPI_Manager::Finalize();
    return 0;
}
