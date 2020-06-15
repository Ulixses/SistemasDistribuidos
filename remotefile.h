#ifndef REMOTEFILE_H
#define REMOTEFILE_H


class remoteFile
{
public:
    remoteFile();
    //abre rchivo "filename" y copia su contenido en "buff",
    //inicia "buffllen" a su tamaño
    void readfile(char* filename, char** buff, unsigned long int *bufflen);
};

#endif // REMOTEFILE_H
