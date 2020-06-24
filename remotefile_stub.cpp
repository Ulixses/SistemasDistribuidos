#include "remotefile_stub.h"
#include "mpi_manager.h"
#include <string.h>

remoteFile_stub::remoteFile_stub()
{
    MPI_Manager::Init();
    //en remote se adapta al host
    this->comm=MPI_Manager::Instanciate("rpc_remotefile");
}

void remoteFile_stub::readfile(char *filename, char **buff, unsigned long *bufflen)
{
    *buff=NULL;
    *bufflen=0;
//enviar tipo de operacion
    MPI_Status status;
    int op=OP_READFILE;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
//enviar parametros
    unsigned long int filenameLen=strlen(filename)+1;
    MPI_Send(&filenameLen,1,MPI_LONG,0,0,*(this->comm));
    MPI_Send(filename,(int)filenameLen,MPI_CHAR,0,0,*(this->comm));
//esperar resultado
    //recibir tamaño
    MPI_Recv(bufflen,1,MPI_LONG,0,0,*(this->comm),&status);
    //reservar buffer
    *buff=new char[*bufflen];
    //recibir buffer
    MPI_Recv(*buff,*bufflen,MPI_CHAR,0,0,*(this->comm),&status);
}

remoteFile_stub::~remoteFile_stub()
{
    int op=OP_EXIT;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
    
}







