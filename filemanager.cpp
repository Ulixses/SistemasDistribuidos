#include "filemanager.h"


FileManager::FileManager(string path)
{
    this->dirPath=path;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != nullptr) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != nullptr) {
          if(ent->d_type==DT_REG) //Store only regular files
          {
           string* f=new string(ent->d_name);
           this->files[*f]=f;
          }
      }
      closedir (dir);
    } else {
      /* could not open directory */
        string* f=new string("ERROR: No existe el fichero o directorio");
        this->files[*f]=f;
        std::cout<<"ERROR: No existe el fichero o directorio\n";
    }
}


vector<string*>* FileManager::listFiles()
{
    vector<string*>* flist=new vector<string*>();
    for(map<string,string*>::iterator i=files.begin();i!= files.end();++i)
    {
        flist->push_back(new string(i->first));
    }
    return flist;
}


void FileManager::freeListedFiles(vector<string*>* fileList)
{
    for(vector<string*>::iterator i=fileList->begin();i!= fileList->end();++i)
    {
        delete *i;
    }
    delete fileList;
}


void FileManager::readFile(char* fileName, char* &data, unsigned long int & dataLength)
{
    string path=this->dirPath+"/"+string(fileName);
    FILE* f=fopen(path.c_str(),"r");

    fseek(f, 0L, SEEK_END);
    dataLength= ftell(f);
    fseek(f, 0L, SEEK_SET);
    data=new char[dataLength];

    fread(data,dataLength,1,f);
    // dataLength=dataLength+1;
    // data[dataLength]='\0';
    fclose(f);
}


void FileManager::writeFile(char* fileName, char* data, unsigned long dataLength)
{
    string path=this->dirPath+"/"+string(fileName);
    FILE* f=fopen(path.c_str(),"w");
    fwrite(data,dataLength,1,f);
    fclose(f);
//añadir a la lista el nuevo fichero, si no existe ya
    if(files.find(string(fileName))==files.end())
        files[ string(fileName)]=new string(fileName);

}
