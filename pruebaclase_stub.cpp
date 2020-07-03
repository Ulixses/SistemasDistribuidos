#include "pruebaclase_stub.h"
#include "mpi.h"
#include <iostream>
//#include <opemnpi/mpi.h>
#include "mpi_manager.h"

pruebaClase_stub::pruebaClase_stub(char * host)
{
    MPI_Manager::Init();
    //en remote se adapta al host
    ip = host;
    this->comm=MPI_Manager::Instanciate("rpc_pruebaclase", host);
}

void pruebaClase_stub::holamundo()
{
//enviar tipo operacion holamundo
    int op=OP_HOLAMUNDO;
    MPI_Send(&op, 1,MPI_INT,0,0,*(this->comm));
 //recibo ack
    int ack=0;
    MPI_Status status;
    MPI_Recv(&ack,1,MPI_INT,0,0,*(this->comm),&status);
    if(ack!=1) std::cout<<"ERROR MASTER HOLAMUNDO "<< __LINE__<<  __FILE__<<"\n";


}
int pruebaClase_stub::suma(int a , int b)
{
//enviar tipo operacion suma
    int result=0;
    MPI_Status status;
    int op=OP_SUMA;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
 //enviar parametros
    int* data=new int[2];
    data[0]=a;
    data[1]=b;
    MPI_Send(data,2,MPI_INT,0,0,*(this->comm));
 //esperar resultado
    MPI_Recv(&result,1,MPI_INT,0,0,*(this->comm),&status);

    delete[] data;

    return result;
}

pruebaClase_stub::~pruebaClase_stub()
{
    int op=OP_EXIT;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
    
}









