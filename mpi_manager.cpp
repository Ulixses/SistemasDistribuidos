#include "mpi_manager.h"
#include "mpi.h"
#include "utils.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

std::vector<MPI_Comm*> MPI_Manager::comms;
bool MPI_Manager::init =false;
std::map<std::string, int> MPI_Manager::remotefile;
std::map<std::string, int> MPI_Manager::pruebaclase;


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
            sql::SQLString dummy;
            query = con->createStatement();
            res = query->executeQuery("select * from services");
            while (res->next())
            {
                dummy = res->getString("service");
                if(dummy.compare("rpc_pruebaclase") == 0) //rpc_pruebaclase
                {
                    ip = res->getString("ip");
                    pruebaclase.insert(std::pair<std::string, int>(ip,0));
                    std::cout << dummy << ":" << ip << std::endl;
                }
                else if(dummy.compare("rpc_remotefile") == 0) //rpc_remotefile
                {
                    ip = res->getString("ip");
                    remotefile.insert(std::pair<std::string, int>(ip,0));
                    std::cout << dummy << ":" << ip << std::endl;

                }
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

MPI_Comm* MPI_Manager::Instanciate(char *processName)
{
    MPI_Comm* newComm= new MPI_Comm[1];
    MPI_Info info;
    MPI_Info_create(&info);
    if(strcmp(processName, "rpc_remotefile") == 0)
    {
        MPI_Info_set(info,"host",getBestRemotefile().c_str());
    }
    else if(strcmp(processName, "rpc_pruebaclase") == 0)
    {            
        MPI_Info_set(info,"host",getBestPruebaclase().c_str());
    }        
    else
        MPI_Info_set(info,"host","localhost");

    MPI_Comm_spawn(processName,MPI_ARGV_NULL,1,info,0,
                   MPI_COMM_SELF,newComm,MPI_ERRCODES_IGNORE);
    comms.push_back(newComm);
    return newComm;
}

std::string MPI_Manager::getBestPruebaclase()
{
    int usos = INT32_MAX;
    std::string ip;
    for (auto i : pruebaclase)
    {
        if(i.second < usos)
        {
            usos = i.second;
            ip = i.first;
        }
    }
    pruebaclase[ip]++;
    return ip;
}

std::string MPI_Manager::getBestRemotefile()
{
    int usos = INT32_MAX;
    std::string ip;
    for (auto i : remotefile)
    {
        if(i.second < usos)
        {
            usos = i.second;
            ip = i.first;
        }
    }
    remotefile[ip]++;
    return ip;
}












