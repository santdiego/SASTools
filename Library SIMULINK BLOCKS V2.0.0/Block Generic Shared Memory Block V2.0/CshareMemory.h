#ifndef CSHAREMEMORY_H
#define CSHAREMEMORY_H

/**********************************************************************************************
Author : Ing. Diego Daniel Santiago.
diego.daniel.santiago@gmail.com
dsantiago@inaut.unsj.edu.ar
Facultad de Ingeniería
Universidad Nacional de San Juan
San Juan - Argentina

INAUT - Instituto de Automática
http://www.inaut.unsj.edu.ar/
CONICET - Consejo Nacional de Investigaciones Científicas y Técnicas.
http://www.conicet.gov.ar/

Version: 2.0.1

HISTORIAL DE CAMBIO:
-V2.0 18/11/2014 : Se agrego Compatibilidad Con Linux
      06/07/2015 : Se modifica Mecanismo de Cierre de memoria compartida.
                   Se modifican mensajes de error. Se elimina error "THE SHARE MEMORY WAS ALREADY CLOSE "
                   Se modifica constructor de la clase
                   Se unificaros los tipos de datos destinados a tamaño a "size_t"
************************************************************************************************/

#pragma once
#ifdef UNICODE
   #undef UNICODE
#endif
        
#ifdef _WIN32
    #include <Windows.h>
#endif /* _WIN32 */
        
        
#if defined (__linux) || defined (__APPLE__)

    #include <stdio.h>
    /* exit() etc */
    #include <unistd.h>
    /* shm_* stuff, and mmap() */
    #include <sys/mman.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    /* Posix IPC object name [system dependant] - see
    http://mij.oltrelinux.com/devel/unixprg/index2.html#ipc__posix_objects */

    /* how many types of messages we recognize (fantasy) */
    #define TYPES               8
    #endif /* __linux || __APPLE__ */

#define DEFAULT_SIZE 200000 //bytes


class CshareMemory
{
public:
    CshareMemory();
    CshareMemory(char *cpName);
    CshareMemory(char *cpName, size_t size);
    virtual ~CshareMemory();
    //Share memory functions
    bool openShareMemory();
    bool closeShareMemory();
    bool readShareMemory(void *pfData);
    bool readShareMemory(void *pfData, size_t size);//read custom_size<_size
    bool writeShareMemory(void *pfData);
    bool writeShareMemory(void *pfData, size_t size);//write custom_size<_size
    void setID(int newId);
    int getID();
    char* getName(); //Get the share memory name
    void setName(char *name);
    size_t getSize();
    void setSize(size_t size);

protected:
    void *pMemory;          //Pointer to Memory dir
    char cpName[40];		//Name of the Sare Memory
    size_t _size;           //Size of Share Memory
    int _id;                // Id of Share Memory
#ifdef _WIN32
        HANDLE hShare;		// system Handle of share memory
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
    int shmfd;
#endif /* __linux || __APPLE__ */

};
#endif
