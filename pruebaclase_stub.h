#ifndef PRUEBACLASE_STUB_H
#define PRUEBACLASE_STUB_H
#include "mpi_manager.h"

#define OP_EXIT     0
#define OP_HOLAMUNDO 1
#define OP_SUMA      2


class pruebaClase_stub
{
    MPI_Comm* comm;
public:
    std::string ip;
    pruebaClase_stub(char * host);
    void holamundo();
    int suma(int a , int b);
    ~pruebaClase_stub();
};

#endif // PRUEBACLASE_STUB_H
