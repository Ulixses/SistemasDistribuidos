#ifndef MPI_MANAGER_H
#define MPI_MANAGER_H
#include <vector>
#include <map>
#include "mpi.h"

class MPI_Manager
{
    static bool init;
    static std::vector<MPI_Comm*> comms;
    static std::map<std::string, int> remotefile;
    static std::map<std::string, int> pruebaclase;
public:
    MPI_Manager();
    static void Init();
    static void Finalize();
    static MPI_Comm* Instanciate(char* processName);
    static std::string getBestPruebaclase();
    static std::string getBestRemotefile();

};

#endif // MPI_MANAGER_H
