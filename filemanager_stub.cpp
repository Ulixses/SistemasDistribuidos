#include "filemanager_stub.h"

filemanager_stub::filemanager_stub(char * host)
{
    MPI_Manager::Init();
    //en remote se adapta al host
    ip = host;
    this->comm=MPI_Manager::Instanciate("rpc_filemanager", host);
}

void filemanager_stub::freeListedFiles(std::vector<std::string*>* fileList)
{
    for(std::vector<std::string*>::iterator i=fileList->begin();i!= fileList->end();++i)
    {
        delete *i;
    }
    delete fileList;
}

std::vector<std::string*>* filemanager_stub::listFiles()
{
    //enviar tipo de operacion
    MPI_Status status;
    int op=OP_LISTFILES;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
    //Recibir datos
    long bufflen;
    MPI_Recv(&bufflen,1,MPI_LONG,0,0,*(this->comm),&status);
    char *buff=new char[bufflen];
    MPI_Recv(buff,bufflen,MPI_CHAR,0,0,*(this->comm),&status);
    //Dar formato a los datos
    std::string str_buff;
    for (int i = 0; i < bufflen; i++) str_buff += buff[i];
    std::vector<std::string*>* result = new std::vector<std::string*>;
    //Transformar al tipo de return
    while (str_buff.size() > 0)
    {
        int pos = str_buff.find('\0');
        std::string *new_file = new std::string(str_buff.substr(0,pos));
        str_buff.erase(0,pos+1);
        result->push_back(new_file);
    }
    
    delete[] buff;
    return result;
}

void filemanager_stub::readFile(char* filename, char* &buff, unsigned long int & bufflen)
{
    buff=NULL;
    bufflen=0;
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
    MPI_Recv(&bufflen,1,MPI_LONG,0,0,*(this->comm),&status);
    //reservar buffer
    buff=new char[bufflen + 1];
    //recibir buffer
    MPI_Recv(buff,bufflen,MPI_CHAR,0,0,*(this->comm),&status);
    buff[bufflen] = '\0';
    bufflen++;
}

void filemanager_stub::writeFile(char* filename, char* buff, unsigned long int bufflen)
{
//enviar tipo de operacion
    MPI_Status status;
    int op=OP_WRITEFILE;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
//enviar parametros
    unsigned long int filenameLen=strlen(filename)+1;
    MPI_Send(&filenameLen,1,MPI_LONG,0,0,*(this->comm));
    MPI_Send(filename,(int)filenameLen,MPI_CHAR,0,0,*(this->comm));

    MPI_Send(&bufflen,1,MPI_LONG,0,0,*(this->comm));
    MPI_Send(buff,(int)bufflen,MPI_CHAR,0,0,*(this->comm));
}

filemanager_stub::~filemanager_stub()
{
    int op=OP_EXIT;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
}
