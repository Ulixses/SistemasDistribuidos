#ifndef MPI_MANAGER_H
#define MPI_MANAGER_H
#include <vector>
#include <map>
#include "mpi.h"

class MPI_Manager
{
    static bool init;
    static std::vector<MPI_Comm*> comms;
    
public:
    static std::map<std::string, bool> remotefile;
    static std::map<std::string, bool> pruebaclase;
    static std::map<std::string, bool> filemanager;

    MPI_Manager();
    static void Init();
    static void Finalize();
    static MPI_Comm* Instanciate(char* processName, char* host = NULL);

};

#endif // MPI_MANAGER_H
