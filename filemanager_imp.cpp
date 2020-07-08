#include "filemanager_imp.h"
#include "filemanager_stub.h"
#include "mpi.h"

filemanager_imp::filemanager_imp()
{
    MPI_Manager::Init();
    this->parent=new MPI_Comm[1];
    MPI_Comm_get_parent(this->parent);
    this->fmanager=new FileManager("./dirprueba");
}

void filemanager_imp::run()

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
            //AÑADIR FUNCIONES DE FILEMANAGER
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
                fmanager->readFile(filename,buff,bufflen);

                //envio resultado
                MPI_Send(&bufflen,1,MPI_LONG,0,0,*(this->parent));
                MPI_Send(buff,bufflen,MPI_CHAR,0,0,*(this->parent));

                delete[] buff;
                delete[] filename;
            }break;
            case OP_WRITEFILE:
            {
                //recibo parametros
                unsigned long int filenameLen=0;
                MPI_Recv(&filenameLen,1,MPI_LONG,0,0,*(this->parent),&status);
                char* filename=new char[filenameLen];
                MPI_Recv(filename,filenameLen,MPI_CHAR,0,0,*(this->parent),&status);

                unsigned long int dataLen=0;
                MPI_Recv(&dataLen,1,MPI_LONG,0,0,*(this->parent),&status);
                char* data=new char[dataLen];
                MPI_Recv(data,dataLen,MPI_CHAR,0,0,*(this->parent),&status);

                fmanager->writeFile(filename, data, dataLen);
            }break;
            case OP_LISTFILES:
            {
                char * all_files;
                vector<string*>* dummy_files = fmanager->listFiles();
                string dummy = this->toList(dummy_files);
                long bufflen = dummy.size();
                MPI_Send(&bufflen,1,MPI_LONG,0,0,*(this->parent));
                MPI_Send(dummy.c_str(),bufflen,MPI_CHAR,0,0,*(this->parent));

            }break;

            default:
            {
            std::cout<<"ERROR SLAVE OPERACION NO SOPORTADA "<< __LINE__<<  __FILE__<<"\n";
            }break;
            }
    }
}

string filemanager_imp::toList(vector<string*>* all)
{
    string list;
    vector<string*>* flist=new vector<string*>();
    for(int i = 0; i < all->size(); i++)
    {
        list += all->at(i)->c_str();
        list += '\0';
    }
    return list;
}


filemanager_imp::~filemanager_imp()
{
    MPI_Manager::Finalize();
    delete fmanager;
    delete[] this->parent;
}
