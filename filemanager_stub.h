#ifndef FILEMANAGER_STUB_H
#define FILEMANAGER_STUB_H
#include "mpi_manager.h"
#include <string>

#define OP_EXIT     0
#define OP_READFILE 1
#define OP_WRITEFILE 2
#define OP_LISTFILES 3

class filemanager_stub
{
    MPI_Comm* comm;
    std::string dirPath;
public:
    std::string ip;
    filemanager_stub(char * host);
    std::vector<std::string*>* listFiles();
    void freeListedFiles(std::vector<std::string*>* fileList);
    void readFile(char* fileName, char* &data, unsigned long int & dataLength);
    void writeFile(char* fileName, char* data, unsigned long int dataLength);

    ~filemanager_stub();
};



#endif // FILEMANAGER_STUB_H
