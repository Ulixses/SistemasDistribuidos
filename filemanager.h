#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <dirent.h>

using namespace std;

class FileManager
{
    string dirPath;
    map<string, string* > files;
public:
/**
 * @brief FileManager::FileManager Constructor de la clase FileManager. Recibe por parámetros el directorio
 * que usará esta clase para indexar, almacenar y leer ficheros. Se aconseja usar una ruta completa al directorio,
 * desde la raiz del sistema de ficheros.
 *
 * @param path Ruta al directorio que se desa usar
 */
    FileManager(string path);

/**
 * @brief FileManager::listFiles Sirve para acceder a la lista de ficheros almacenados en la ruta
 * que se usó en el contructor de la clase
 * @return Una copia de la lista de ficheros almacenada en "files". Esta copia hay que liberarla
 * después de haber sido usada. Para ello se ofrece la función "freeListedFiles"
 */   
    vector<string*>* listFiles();

/**
 * @brief FileManager::freeListedFiles Función de apoyo, libera una lista de ficheros devuelta por la función "listFiles"
 * @param fileList Puntero a una lista de strings devuelta por la función "listFiles"
 */
    void freeListedFiles(vector<string*>* fileList);

/**
 * @brief FileManager::readFile Dado el nombre de un fichero almacenado en el directorio que se usó en el contructor,
 * se inicializará la variable "data" con un puntero al contenido del fichero, y la variable dataLength con el
 * tamaño del mismo en bytes.
 *
 * @param fileName Nombre del fichero a leer
 * @param data Datos del fichero. Deberá de liberarse después de ser usado
 * @param dataLength Longitud del fichero en bytes
 */
    void readFile(char* fileName, char* &data, unsigned long int & dataLength);

/**
 * @brief FileManager::writeFile Dado un nuevo nombre de un fichero que se quiere almacenar  en el directorio que se usó en el contructor,
 * se escribirá el contenido del array de datos almacenado en "data", siendo dataLength el
 * tamaño del mismo en bytes. Sobreescribirá el fichero que hubiera en el directorio si tiene el mismo nombre.
 *
 * @param fileName Nombre del fichero a escribir
 * @param data Datos del fichero. Deberá de liberarse después de ser usado
 * @param dataLength Longitud del fichero en bytes
 */
    void writeFile(char* fileName, char* data, unsigned long int dataLength);
};

#endif // FILEMANAGER_H
