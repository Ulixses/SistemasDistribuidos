#include <iostream>
#include <signal.h>
#define DEBUG 0
#include "httpserver.h"
#include "mpi_manager.h"
using namespace std;

httpServer* server=nullptr;
void sigClose(int dummy) {
    server->closeServer();
    delete server;
    exit(0);
}

int main()
{
    MPI_Manager::Init();

    #if DEBUG == 1
    volatile bool salir = false;
    while (!salir)
    {
        ;
    }
    #endif

    server =new httpServer(8080);
    signal(SIGINT, sigClose);

    while(1)
    {
        int newfd=server->waitForConnections();
        server->resolveRequests(newfd);
    }
}
