#ifndef FILEMANAGER_IMP_H
#define FILEMANAGER_IMP_H
#include "mpi_manager.h"
#include "filemanager.h"

class filemanager_imp
{
    FileManager* fmanager = nullptr;
    MPI_Comm * parent;
public:
    filemanager_imp();
    void run();
    ~filemanager_imp();
    string toList(vector<string*>* all);
};

#endif // FILEMANAGER_IMP_H