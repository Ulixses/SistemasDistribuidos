#include "httpserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"
#include <iostream>
#include <string.h>

//using namespace std;

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "mpi_manager.h"

httpServer::httpServer(unsigned short port)
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
    printf("Error creating socket\n");
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    int option = 1;
    setsockopt(sock_fd,SOL_SOCKET,
               (SO_REUSEPORT | SO_REUSEADDR),
               &option,sizeof(option));

    if (bind(sock_fd,(struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          printf("ERROR on binding");


    listen(sock_fd,5);

    buildMimetypeTable();

    //Añadidido por Alejandro

    this->generateServices();
    this->generateDependencies();
}

std::string httpServer::getmimeType(char* file)
{
    std::string strfile=std::string(file);
    std::string ext="";
    std::string result="application/octet-stream";

    if(strfile[strfile.length()-4]=='.')
    {
        ext=std::string(&strfile[strfile.length()-3]);
    }
    else
        if(strfile[strfile.length()-5]=='.')
        {
            ext=std::string(&strfile[strfile.length()-4]);
        }


    std::map<std::string,std::string>::iterator mimetype= this->mimeTypes.find(ext);
    if(mimetype!=mimeTypes.end())
    {
        result= mimetype->second;
    }

    return result;
}

void httpServer::buildMimetypeTable()
{
    this->mimeTypes["aac"]="audio/aac";
    this->mimeTypes["abw"]="application/x-abiword";
    this->mimeTypes["arc"]="application/octet-stream";
    this->mimeTypes["avi"]="video/x-msvideo";
    this->mimeTypes["azw"]="application/vnd.amazon.ebook";
    this->mimeTypes["bin"]="application/octet-stream";
    this->mimeTypes["bz"]="application/x-bzip";
    this->mimeTypes["bz2"]="application/x-bzip2";
    this->mimeTypes["csh"]="application/x-csh";
    this->mimeTypes["css"]="text/css";
    this->mimeTypes["csv"]="text/csv";
    this->mimeTypes["doc"]="application/msword";
    this->mimeTypes["epub"]="application/epub+zip";
    this->mimeTypes["gif"]="image/gif";
    this->mimeTypes["htm"]="text/html";
    this->mimeTypes["html"]="text/html";
    this->mimeTypes["ico"]="image/x-icon";
    this->mimeTypes["ics"]="text/calendar";
    this->mimeTypes["jar"]="application/java-archive";
    this->mimeTypes["jpeg"]="image/jpeg";
    this->mimeTypes["jpg"]="image/jpeg";
    this->mimeTypes["js"]="application/javascript";
    this->mimeTypes["json"]="application/json";
    this->mimeTypes["mid"]="audio/midi";
    this->mimeTypes["midi"]="audio/midi";
    this->mimeTypes["mpeg"]="video/mpeg";
    this->mimeTypes["mpkg"]="application/vnd.apple.installer+xml";
    this->mimeTypes["odp"]="application/vnd.oasis.opendocument.presentation";
    this->mimeTypes["ods"]="application/vnd.oasis.opendocument.spreadsheet";
    this->mimeTypes["odt"]="application/vnd.oasis.opendocument.text";
    this->mimeTypes["oga"]="audio/ogg";
    this->mimeTypes["ogv"]="video/ogg";
    this->mimeTypes["ogx"]="application/ogg";
    this->mimeTypes["pdf"]="application/pdf";
    this->mimeTypes["png"]="image/png";
    this->mimeTypes["ppt"]="application/vnd.ms-powerpoint";
    this->mimeTypes["rar"]="application/x-rar-compressed";
    this->mimeTypes["rtf"]="application/rtf";
    this->mimeTypes["sh"]="application/x-sh";
    this->mimeTypes["svg"]="image/svg+xml";
    this->mimeTypes["swf"]="application/x-shockwave-flash";
    this->mimeTypes["tar"]="application/x-tar";
    this->mimeTypes["tif"]="image/tiff";
    this->mimeTypes["tiff"]="image/tiff";
    this->mimeTypes["ttf"]="font/ttf";
    this->mimeTypes["vsd"]="application/vnd.visio";
    this->mimeTypes["wav"]="audio/x-wav";
    this->mimeTypes["weba"]="audio/webm";
    this->mimeTypes["webm"]="video/webm";
    this->mimeTypes["webp"]="image/webp";
    this->mimeTypes["woff"]="font/woff";
    this->mimeTypes["woff2"]="font/woff2";
    this->mimeTypes["xhtml"]="application/xhtml+xml";
    this->mimeTypes["xls"]="application/vnd.ms-excel";
    this->mimeTypes["xml"]="application/xml";
    this->mimeTypes["xul"]="application/vnd.mozilla.xul+xml";
    this->mimeTypes["zip"]="application/zip";
    this->mimeTypes["3gp"]="video/3gpp";
    this->mimeTypes["3g2"]="video/3gpp2";
    this->mimeTypes["7z"]="application/x-7z-compressed";

}

int httpServer::waitForConnections()
{
    //Esta funcion bloquea el proceso, encuentra la forma de que no pase
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    //Codigo para comprobar los tiempos de los procesos
    //y evitar que accept() bloque el programa
    fd_set set;
    struct timeval tv;//2 segundos, 0 milisegundos
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    FD_ZERO(&set);
    FD_SET(sock_fd, &set);
    int rv = -1;
    //Bucle cada tv segundos que comprueba los tiempos de los procesos
    while (rv != 1 || FD_ISSET(sock_fd, &set) == 0)
    {
        FD_SET(sock_fd, &set);
        rv = select(sock_fd+1, &set, NULL, NULL, &tv);
        //std::cout << rv << " : " << FD_ISSET(sock_fd, &set) << std::endl;
        checkTimes();
    }
    
    int newsock_fd = accept(sock_fd,
                            (struct sockaddr *) &cli_addr,
                            &clilen);
    return newsock_fd;
}

void httpServer::sendContent(int newsock_fd,char* httpHeader,unsigned long int headerLen, char* content, unsigned long int contentLen)
{
    char* msg=new char[headerLen+contentLen+2];
    memcpy(msg,httpHeader,headerLen);
    msg[headerLen]='\r';
    msg[headerLen+1]='\n';
    memcpy(&(msg[headerLen]),content,contentLen);

    send(newsock_fd, msg,headerLen+contentLen+2,0);
}

void httpServer::sendFile(int newsock_fd,char* file)
{

    std::string mimetype=getmimeType(file);
    std::string filepath=this->files_path+std::string(file);
    char* fileContent=NULL;
    unsigned long int filelen=0;
    char* httpHeader;
    unsigned long int headerLen;

    readFile(&filepath[0],&fileContent,&filelen);
    createHeader(&httpHeader,&headerLen,
                 "200 OK",
                 &(mimetype[0]),
                 filelen);
    sendContent(newsock_fd,httpHeader,headerLen,fileContent,filelen);
}

void httpServer::sendVirtualFile(int newsock_fd,char* file, char *content, std::string fileType = "html")
{

    std::string mimetype = this->mimeTypes[fileType];
    char* fileContent=NULL;
    unsigned long int filelen= strlen(content);
    char* httpHeader;
    unsigned long int headerLen;

    createHeader(&httpHeader,&headerLen,
                 "200 OK",
                 &(mimetype[0]),
                 filelen);
    sendContent(newsock_fd,httpHeader,headerLen,content,filelen);
}

int httpServer::getHTTPParameter(std::vector<std::vector<std::string*>*> *lines,std::string parameter)
{

    for(unsigned long int i=0;i<lines->size();i++)
    {
        std::vector<std::string*> *v=(*lines)[i];

        if(((*v)[0])->compare(parameter)==0)
        {
            return atoi((*v)[1]->c_str());
        }
    }
    return 0;
}

void httpServer::resolveRequests(int newsock_fd)
{

    std::vector<std::vector<std::string*>*> lines;
    readLines(newsock_fd,&lines);
    printLines(&lines);
    std::cout<<"\n";
    std::vector<std::string*> *v=(lines)[0];
    httpRequest_t req=getRequestType(v);
        switch(req)
        {
         case GET:
            {

                std::string *s2=(*v)[1];
                if(s2->compare("/")==0)
                {
                    sendFile(newsock_fd,"/index.html");
                }
                else
                {
                    sendFile(newsock_fd,&((*s2)[0]));            
                }                
            }
            break;
        case POST:
           {
                std::vector<std::string*> postLine;
                int length=getHTTPParameter(&lines,"Content-Length:");
                readPostLine(newsock_fd,&postLine,length);

                std::string *s2=(*v)[1];
                if(s2->compare("/login.php")==0)
                {
                    char* user= getFromPost(postLine,"uname");
                    char* pass= getFromPost(postLine,"psw");

                    std::cout<<"user: "<<user<<"\n";
                    std::cout<<"pass: "<<user<<"\n";
                    if(validatePassword(user,pass))
                    {
                        sendFile(newsock_fd,"/services.html");
                    }
                    delete[] user;
                    delete[] pass;
                }

                if(s2->compare("/services.php")==0)
                {
                    servicesPost(newsock_fd, postLine);
                }

           }
           break;
        default:
            break;
        }
    close(newsock_fd);
    deleteLines(&lines);
}

bool httpServer::validatePassword(char* username,char* password){
    try {
        sql::Connection *con;
        sql::PreparedStatement  *prep_stmt;
        sql::PreparedStatement  *hasher;
        sql::ResultSet *res;

        con = getMySql();
        if(con == NULL)
        {
            return false;
        }

        //Secure user password
        hasher = con->prepareStatement("SELECT SHA1(?);");
        hasher->setString(1, password);//User password
        res = hasher->executeQuery();
        res->next();
        sql::SQLString userHash = res->getString(1);
        delete res;

        //Search user
        prep_stmt = con->prepareStatement("select * from users where user=?");
        prep_stmt->setString(1, username);//User username
        res = prep_stmt->executeQuery();

        if(res->next())
        {
            std::cout << "Usuario correcto\n";
            sql::SQLString dbHash = res->getString("passwd");
            if(dbHash == userHash)
            {
                std::cout << "Contraseña correcta\n";
                return true;
            }
            else
            {
                std::cout << "Contraseña incorrecta\n";
                return false;
            }
        }
        else
        {
            std::cout << "No existe el usuario\n";
            return false;
        }

        delete res;
        delete prep_stmt;
        delete con;

    }catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }


    return false;
}

void httpServer::closeServer()
{
    
    close(this->sock_fd);
}

void httpServer::servicesPost(int newsock_fd, std::vector<std::string*> &postLine)
{
    //Leer las checkbox del usuario
    char * pruebaclaseBox = getFromPost(postLine,"pruebaclase");
    char * remoteFileBox = getFromPost(postLine,"remoteFile");
    bool pruebaclase = false;
    bool remoteFile = false;

    if( pruebaclaseBox != NULL)
        pruebaclase = strcmp(pruebaclaseBox,"on") == 0;
    if( remoteFileBox != NULL)
        remoteFile = strcmp(remoteFileBox,"on") == 0;

    char resultadoTemplate[] ="\
<html>\n\
<body>\n\
%s\n\
%s\n\
</body>\n\
</html>";


    char template_suma[] = "<h2>La suma de la ip %s es: %d</h2><hr>\n";
    std::string suma= "";

    char template_archivo[] = "<h2>El contenido del fichero de la ip %s es: %s</h2><hr>\n";
    std::string archivo = "";
    if(pruebaclase)
    {
        char * ip;
        char dummy[100];
        for(int i = 0; i < MPI_Manager::pruebaclase.size(); i++)
        {
            std::string postName = "pc" + std::to_string(i);
            ip = getFromPost(postLine,postName);
            if(ip != NULL)
            {
                int result = this->pruebaclaseService(postLine, std::string(ip));
                sprintf(dummy, template_suma,  ip, result);
                suma += dummy;
            }
        }
        
    }
    if(suma.size() == 0)
        suma = "<h2>La suma es: Proceso no realizado</h2><br>\n";
    if(remoteFile)
    {
        char * ip;
        char dummy[300];
        for(int i = 0; i < MPI_Manager::pruebaclase.size(); i++)
        {  
            std::string postName = "rf" + std::to_string(i);
            ip = getFromPost(postLine,postName);
            if(ip != NULL)
            {
                std::string result = this->remotefileService(postLine, std::string(ip));
                sprintf(dummy, template_archivo,  ip, result.c_str());
                archivo += dummy;
            }
        }
    }
    if(archivo.size()==0)
        archivo= "<h2>El contenido del fichero es: Proceso no realizado</h2><hr>\n";


    int fileSize = suma.size() + archivo.size() + strlen(resultadoTemplate) + 1;
    char * content = new char[fileSize];
    sprintf(content, resultadoTemplate, suma.c_str(), archivo.c_str());
    sendVirtualFile(newsock_fd, "resultado.html", content);

}

void httpServer::checkTimes()
{
    //std::cout << "HE COMPROBADO LOS TIEMPOS\n";
    
    int tiempo = 60; //Tiempo en segundos para apagar los procesos
    for(auto i : pclase)
        if(difftime(time(NULL),i.second) > tiempo)
        {
            std::cout << "HE BORRADO REMOTEFILE\n";
            delete i.first;
            pclase.erase(i.first);
        }
    for(auto i : file)
        if(difftime(time(NULL),i.second) > tiempo)
        {
            std::cout << "HE BORRADO PCLASE\n";
            delete i.first;
            file.erase(i.first);
        }
}

void httpServer::generateServices()
{    
    std::string filepath=this->files_path+std::string("/services_template.html");
    char* fileContent=NULL;
    unsigned long int filelen=0;
    readFile(&filepath[0],&fileContent,&filelen);

    //<option value="10.0.2.23">10.0.2.23</option>
    //<input type="checkbox" name='pc1'>10.0.2.23</option>

    std::string ips_file = "";
    std::string ips_prueba = "";
    char option_template_prueba[] = "<hr>\n<input type='checkbox' name='pc%d' value='%s' >%s</input>\n";
    char option_template_file[] = "<hr>\n<input type='checkbox' name='rf%d' value='%s' >%s</input>\n";
    char dummy[100];
    int n = 0;
    for (auto i : MPI_Manager::pruebaclase)
    {
        sprintf(dummy, option_template_prueba, n, i.first.c_str(), i.first.c_str());
        ips_prueba += dummy;
        n++;
    }
    ips_prueba += "<hr>\n";
    n=0;
    for (auto i : MPI_Manager::remotefile)
    {
        sprintf(dummy, option_template_file, n, i.first.c_str(), i.first.c_str());
        ips_file += dummy;
        n++;
    }
    ips_file += "<hr>\n";
    char * new_services = new char[filelen + ips_file.size() + ips_prueba.size() + 1];
    sprintf(new_services, fileContent, ips_prueba.c_str(), ips_file.c_str());

    filepath=this->files_path+std::string("/services.html");
    FILE * service = fopen(filepath.c_str(),"w");
    if(service == NULL)
        std::cout << "Error al abrir services.html\n";
    else
    {
        fputs(new_services, service);
        fclose(service);
    }
    delete[] new_services;
}

int httpServer::pruebaclaseService(std::vector<std::string*> &postLine , std::string ip)
{
    int result = 0;
    int n1 = std::stoi(getFromPost(postLine,"n1"));
    int n2 = std::stoi(getFromPost(postLine,"n2"));
    //std::string ip = getFromPost(postLine,"ip_prueba");

    std::string cmdEnd = "ulises@" + ip + ":/home/ulises/SistemasDistribuidos/build";
    std::string cmdStart = "scp ";

    //Compruebo si esta copiado
    if(!MPI_Manager::pruebaclase[ip])
    {
        //Copiar archivo y marcar como copiado
        MPI_Manager::pruebaclase[ip] = true;
        std::string update = "UPDATE services SET copiado=1 WHERE ip='" + ip;
        update += "' and service='rpc_pruebaclase'";
        updateCommand(update);
        for(auto i: this->dependencies)
        {
            if(i.first.compare("rpc_pruebaclase") == 0)
                cmdStart += i.second + " ";
        }
        std::string command = cmdStart + cmdEnd;
        FILE * scp = popen(command.c_str(),"w");
        pclose(scp);
    }
    for(auto i : pclase)
    {
        if(i.first->ip == ip)
        {
            return i.first->suma(n1,n2);
        }
    }

    pruebaClase_stub * dummy;
    dummy=new pruebaClase_stub((char *)ip.c_str());
    result=dummy->suma(n1,n2);
    pclase.insert(std::pair<pruebaClase_stub*, time_t>(dummy,time(NULL)));

    return result;
}

std::string httpServer::remotefileService(std::vector<std::string*> &postLine, std::string ip)
{
    //std::string ip = getFromPost(postLine,"ip_file");

    std::string cmdEnd = "ulises@" + ip + ":/home/ulises/SistemasDistribuidos/build";
    std::string cmdStart = "scp ";

    //std::cout << command << "\n";
    //Compruebo si esta copiado
    if(!MPI_Manager::remotefile[ip])
    {
        //Copiar archivo y marcar como copiado
        MPI_Manager::remotefile[ip] = true;
        std::string cmd = "UPDATE services SET copiado=1 WHERE ip='" + ip;
        cmd += "' and service='rpc_remotefile'";
        updateCommand(cmd);
        for(auto i: this->dependencies)
        {
            if(i.first.compare("rpc_remotefile") == 0)
                cmdStart += i.second + " ";
        }
        std::string command = cmdStart + cmdEnd;
        FILE * scp = popen(command.c_str(),"w");
        pclose(scp);
    }


    for(auto i : file)
    {
        if(i.first->ip == ip)
        {
            unsigned long int bufflen;
            char * buff = NULL;
            std::string result;
            i.first->readfile("prueba.txt",&buff,&bufflen);
            if(buff == NULL)
                result = "No existe ese archivo.";
            else
            {
                result = buff;
                delete[] buff;
            }
            return result;
        }
    }

    remoteFile_stub * dummy =new remoteFile_stub((char*)ip.c_str());
    unsigned long int bufflen;
    char * buff = NULL;
    dummy->readfile("prueba.txt",&buff,&bufflen);
    std::string result = buff;
    if(buff != NULL)
        delete[] buff;
    file.insert(std::pair<remoteFile_stub*, time_t>(dummy,time(NULL)));

    return result;
}

void httpServer::generateDependencies()
{
    try
    {
        sql::Connection *con;
        //Acceder a la base de datos y obtener todas las ips de cada proceso
        con = getMySql();
        if(con == NULL)
        {
            std::cout << "No se han podido obtener las dependencias de la base de datos\n";
            return;
        }
        sql::Statement  *query;
        sql::ResultSet *res;
        std::string service;
        std::string file;
        query = con->createStatement();
        res = query->executeQuery("select * from dependencies");
        while (res->next())
        {
            service = res->getString("service");
            file = res->getString("file");
            this->dependencies.emplace_back(service, file);
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
}