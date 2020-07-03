#include "mpi_manager.h"
#include "mpi.h"
#include "utils.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

std::vector<MPI_Comm*> MPI_Manager::comms;
bool MPI_Manager::init =false;
std::map<std::string, bool> MPI_Manager::remotefile;
std::map<std::string, bool> MPI_Manager::pruebaclase;


MPI_Manager::MPI_Manager()
{
}

void MPI_Manager::Init()
{
    if(!MPI_Manager::init)
    {

        int argc=0;
        char** argv=nullptr;
        MPI_Init(&argc,&argv);
        MPI_Manager::init=true;
        #ifdef ULI //Definido desde el archivo cmake, linea 41.
            std::cout << "Soy master\n";
        try
        {
            sql::Connection *con;
            //Acceder a la base de datos y obtener todas las ips de cada proceso
            con = getMySql();
            if(con == NULL)
            {
                std::cout << "No se han podido obtener las ips de la base de datos\n";
                return;
            }
            sql::Statement  *query;
            sql::ResultSet *res;
            std::string ip;
            bool copy = false;
            sql::SQLString dummy;
            query = con->createStatement();
            res = query->executeQuery("select * from services");
            while (res->next())
            {
                dummy = res->getString("service");
                ip = res->getString("ip");
                copy = res->getBoolean("copiado");
                if(dummy.compare("rpc_pruebaclase") == 0) //rpc_pruebaclase
                {
                    pruebaclase.insert(std::pair<std::string, bool>(ip,copy));
                }
                else if(dummy.compare("rpc_remotefile") == 0) //rpc_remotefile
                {
                    remotefile.insert(std::pair<std::string, bool>(ip,copy));
                }
                std::cout << dummy << ":" << ip << " -- "<< copy << std::endl;

            }
            delete query;
            delete res;
            delete con;
        }
        catch (sql::SQLException &e){
            std::cout << "# ERR: SQLException in " << __FILE__;
            std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
            std::cout << "# ERR: " << e.what();
            std::cout << " (MySQL error code: " << e.getErrorCode();
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }
        #endif

    }
        
}


void MPI_Manager::Finalize()
{

    if(MPI_Manager::init)
    {
        MPI_Finalize();
        MPI_Manager::init=false;
    }
   }

MPI_Comm* MPI_Manager::Instanciate(char *processName, char* host)
{
    MPI_Comm* newComm= new MPI_Comm[1];
    MPI_Info info;
    MPI_Info_create(&info);
    if(strcmp(processName, "rpc_remotefile") == 0 || 
    strcmp(processName, "rpc_pruebaclase") == 0)
    {
        if(host == NULL)
            MPI_Info_set(info,"host","localhost");
        else
            MPI_Info_set(info,"host",host);
    }        
    else
        MPI_Info_set(info,"host","localhost");

    MPI_Comm_spawn(processName,MPI_ARGV_NULL,1,info,0,
                   MPI_COMM_SELF,newComm,MPI_ERRCODES_IGNORE);
    comms.push_back(newComm);
    return newComm;
}

