#ifndef REMOTEFILE_IMP_H
#define REMOTEFILE_IMP_H
#include "mpi_manager.h"
#include "remotefile.h"

class remoteFile_imp
{

    remoteFile* rfile=nullptr;
    MPI_Comm * parent;
public:
    remoteFile_imp();
    void run();
    ~remoteFile_imp();
};

#endif // REMOTEFILE_IMP_H
