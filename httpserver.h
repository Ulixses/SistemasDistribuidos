#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <mpi.h>

#include "remotefile_stub.h"
#include "pruebaclase_stub.h"
#include "filemanager_stub.h"
#include <ctime>


/**
 * @brief Clase que implementa un servidor HTTP sencillo
 * con soporte para sesión basada en cookies
 *
 * Sólo implementa los comandos "GET" y "POST"
 * Sólo sirve páginas web en formato HTML, CSS y ficheros
 * Los ficheros deben estar alojados en el direcotrio "html_dir"
 * junto con el ejecutable
 *
 */
class httpServer
{
    /**
     * @brief sock_fd Descriptor del puerto 8080 abierto por el servidor
     */
    int sock_fd;
    /**
     * @brief sessionID Identificador del usuario que hizo login. Se usa como "token"
     * que se guardará como cookie en el cliente y se recibirá con cada nueva comunicación
     */
    int sessionID;
    /**
     * @brief files_path Directorio donde se guardan los archivos que sirve el
     * servidor http
     */
    std::string files_path="../html_dir";
    /**
     * @brief mimeTypes Lista de tipos de ficheros conocidos por el servidor http. Cualquier
     * extensión de fichero no listada se enviará como fichero de tipo "application/octet-stream",
     * que será interpretado por el cliente como una descarga de archivos binarios.
     */
    std::map<std::string,std::string> mimeTypes;

    //Variables para reutilizar procesos y ver su tiempo de vida
    std::map<pruebaClase_stub*, time_t> pclase;
    std::map<remoteFile_stub*, time_t> file;
    std::map<filemanager_stub*, time_t> manager;
    std::vector<std::pair<std::string, std::string>> dependencies;


public:
    //
    //Tipos de "Request" soportados por el protocolo HTTP
    //
    //Sólo se han implementado "GET" y "POST"

    typedef enum httpRequest_t
    {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE
    }httpRequest_t;

    /**
     * @brief httpServer Constructor de la clase. Abrirá un servidor http que esperará
     * conexiones en "localhost:8080"
     *
     * @param port Puerto en el que esperará conexiones
     */
    httpServer(unsigned short port);
    /**
     * @brief waitForConnections Una vez creado el servidor, este método esperará
     * conexiones por parte de clientes web
     *
     * @return Devuelve el puerto de comunicaciones en el que se aceptó la conexión
     * con el cliente, liberando el puerto 8080 para nuevas conexiones
     *
     */
    int  waitForConnections();
    /**
     * @brief resolveRequests Este método lee los mensajes HTTP enviados por el cliente web,
     * y los trata de manera a corde con el tipo de mensaje. Sólo soporta dos tipos de mensaje:
     *  GET: En este caso servirá la página web pedida por el usuario
     *  POST: En este caso recibe las opciones/parámetros introducidos en los formularios html,
     *       y los tratará a corde con la situación
     *
     * @param newsock_fd Socket en el que se reliza la comunicación con el cliente. Una vez
     * recibido el mensaje, se cierra la conexión.
     */
    void resolveRequests(int newsock_fd);
    /**
     * @brief closeServer Método para cerrar el puerto 8080 de forma que se pueda reutilizar
     * más adelante en nuevas ejecuciones del servidor.
     */
    void closeServer();

    /**
     * @brief sendContent Método que envía un mensaje HTTP a través de un puerto. En general
     * enviará la respuesta a un mensaje de tipo GET recibido previamente
     *
     * @param newsock_fd Socket por el que se se realiza el envío del mensaje al cliente
     * @param httpHeader Cabecera HTTP inicializada con los valores necesarios para el mensaje
     * @param headerLen Longitud en bytes de la cabecera
     * @param content Contenido del mensaje (página html pedida, fichero de descarga...)
     * @param contentLen Longitud del contenido, en bytes
     */
    void sendContent(int newsock_fd,char* httpHeader,unsigned long int headerLen, char* content, unsigned long int contentLen);

    /**
     * @brief sendFile Método que realiza el envío de un archivo pedido a través de un mensaje
     * HTTP de tipo GET, recibido anteriormente
     *
     * @param newsock_fd Socket del cliente
     * @param file Nombre del fichero pedido. Debe encontrarse en el directorio "html_dir"
     */
    void sendFile(int newsock_fd,char* file);

    /**
     * @brief getmimeType Enfunción de la extensión del archivo pedido, se necesita traducir
     *  esa extensión a un tipo "mime" para poder crear la cabecera HTTP a corde
     *
     * @param file Nombre del fichero pedido
     * @return Tipo "mime" que se relaciona con la extensión del fichero
     */
    std::string getmimeType(char* file);
    /**
     * @brief buildMimetypeTable Constructor de la tabla de búsqueda de tipos mime. Es una
     * lista parcial de todos los tipos mime identificados por el protocolo HTTP
     */
    void buildMimetypeTable();

    /**
     * @brief getHTTPParameter
     * @param lines
     * @param parameter
     * @return
     */
    int getHTTPParameter(std::vector<std::vector<std::string*>*> *lines,std::string parameter);

    /**
     * @brief validatePassword Función que valida el usuario y la contraseña proporcionadas por parámetros
     *                  DEBE IMPLEMENTARLA EL ALUMNO
     * @param username Nombre de usuario
     * @param password Contraseña
     * @return
     */
    bool validatePassword(char* username,char* password);

    /**
     * @brief servicesPost Se encarga de manejar las peticiones post a la url services.php
     * 
     * @param newsock_fd Socket en el que se reliza la comunicación con el cliente. Una vez
     * recibido el mensaje, se cierra la conexión.
     * @param postLine La informacion enviada con el parametro POST.
     */
    void servicesPost(int newsock_fd, std::vector<std::string*> &postLine);

    void serviceStore(int newsock_fd, std::vector<std::string*> &postLine, int contentLenght);

    void serviceDownload(int newsock_fd, std::vector<std::string*> &postLine);


    void sendVirtualFile(int newsock_fd, char * file, char * content, std::string fileType);

    void checkTimes();
    
    void generateServices();

    void generateDependencies();

    int pruebaclaseService(std::vector<std::string*> &postLine, std::string ip);

    std::string remotefileService(std::vector<std::string*> &postLine, std::string ip);
    
    std::string filemanagerService(std::vector<std::string*> &postLine, std::string ip);
    void fileDownload(int newsock_fd, std::string * url);            

};


#endif // HTTPSERVER_H
