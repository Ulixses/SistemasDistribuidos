#include "remotefile_imp.h"
#include "remotefile_stub.h"
#include "mpi.h"


remoteFile_imp::remoteFile_imp()
{
    MPI_Manager::Init();
    this->parent=new MPI_Comm[1];
    MPI_Comm_get_parent(this->parent);
    this->rfile=new remoteFile();
}

void remoteFile_imp::run()
{
    bool salir=false;
    //mientras no salir
    while(!salir){
         //esperar tipo de operacion
        int op=-1;
        MPI_Status status;
        MPI_Recv(&op,1,MPI_INT,0,0,*(this->parent),&status);
         //dependiendiendo de operacion
        switch(op)
        {
            case OP_EXIT:
            {
                salir=true;
             }break;
            case OP_READFILE:
            {

                //recibo parametros
                unsigned long int filenameLen=0;
                MPI_Recv(&filenameLen,1,MPI_LONG,0,0,*(this->parent),&status);
                char* filename=new char[filenameLen];
                MPI_Recv(filename,filenameLen,MPI_CHAR,0,0,*(this->parent),&status);
                //ejecuto operacion
                char* buff=NULL;
                unsigned long int bufflen=0;
                rfile->readfile(filename,&buff,&bufflen);

                //envio resultado
                MPI_Send(&bufflen,1,MPI_LONG,0,0,*(this->parent));
                MPI_Send(buff,bufflen,MPI_CHAR,0,0,*(this->parent));

                delete[] buff;
                delete[] filename;
            }break;

            default:
            {
            std::cout<<"ERROR SLAVE OPERACION NO SOPORTADA "<< __LINE__<<  __FILE__<<"\n";

            }break;
            }
    }
}

remoteFile_imp::~remoteFile_imp()
{
    MPI_Manager::Finalize();
    delete rfile;
    delete[] this->parent;
}




