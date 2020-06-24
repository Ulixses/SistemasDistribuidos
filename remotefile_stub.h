#ifndef REMOTEFILE_STUB_H
#define REMOTEFILE_STUB_H
#include "mpi_manager.h"

#define OP_EXIT     0
#define OP_READFILE 1

class remoteFile_stub
{
     MPI_Comm* comm;
public:
    remoteFile_stub();
    void readfile(char* filename, char** buff, unsigned long int *bufflen);
    ~remoteFile_stub();
};

#endif // REMOTEFILE_STUB_H
