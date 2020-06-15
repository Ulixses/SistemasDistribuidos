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

#include "pruebaclase_stub.h"
#include "mpi_manager.h"
#include "remotefile_stub.h"

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
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
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
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement  *prep_stmt;
        sql::PreparedStatement  *hasher;
        sql::ResultSet *res;

        //Connect
        driver = get_driver_instance();
        //Other pc IP
        con = driver->connect("tcp://10.0.2.13:3306", "root", "uliware");
        if(!con->isValid())
        {
            std::cout << "Error al conectar";
            return false;
        }
        //Select database
        con->setSchema("sistemas");

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
<h2>La suma es: %s</h2><br>\n\
<h2>El contenido del fichero es: %s</h2><br>\n\
</body>\n\
</html>";

    char suma[] = "Proceso no realizado";
    std::string archivo = "Proceso no realizado";
    if(pruebaclase)
    {
        pruebaClase_stub* pclase=new pruebaClase_stub();
        int result=pclase->suma(1,2);
        sprintf(suma, "%d", result);
        delete pclase;
    }
    if(remoteFile)
    {
        remoteFile_stub* file=new remoteFile_stub();
        unsigned long int bufflen;
        char * buff = NULL;
        file->readfile("prueba.txt",&buff,&bufflen);
        archivo = buff;
        if(buff != NULL)
            delete[] buff;
        delete file;
    }
    int fileSize = strlen(suma) + archivo.size() + strlen(resultadoTemplate) + 1;
    char * content = new char[fileSize];
    sprintf(content, resultadoTemplate, suma, archivo.c_str());
    sendVirtualFile(newsock_fd, "resultado.html", content);

}