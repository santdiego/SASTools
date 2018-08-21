#ifndef ISHAREDMEMORY_H
#define ISHAREDMEMORY_H



#pragma once
#ifdef UNICODE
#undef UNICODE
#endif
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#if defined(__linux) || defined(__APPLE__)

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
#define TYPES 8
#endif /* __linux || __APPLE__ */

#define MAXLENGTH 40
#define MINLENGTH 3
#define MAXISHM 400
#define MAXPROCESSESWAITING 10
#define MAXPROCESSATTACHED 40
#define GLOBALTABLENAME "IShmTable"
#define MAX_SEM_COUNT 10

enum AccessMode {ReadOnly, ReadWrite };
enum SharedMemoryError{ NoError, PermissionDenied, InvalidSize, KeyError, AlreadyExists, NotFound,LockError,OutOfResources,UnknownError,TableError };
enum ShmState { BUSY, FREE, NON_SYNC };

//struct memoryInformation{
//    ShmState _state;  // apunta hacia el final del arreglo de datos en memoria (pData[size]+1)
//                  // cumple la funcion de semaforo con otros procesos
//    int _dataSize;   //tamaño de datos de la memoria compartida
//};

struct IShTable{ //Tabla en memoria compartida que se crea cada vez que una ISharedMemory es instanciada
    char name[MAXISHM][MAXLENGTH];
    int size[MAXISHM];
    int pid[MAXISHM][MAXPROCESSATTACHED]; //process attached
    int lock[MAXISHM][MAXPROCESSESWAITING];// Semaforo
    int  num; //Cantidad de memorias compartidas de tipo ISharedMemory
};

class BaseSharedMemory{
public:
    BaseSharedMemory();
    ~BaseSharedMemory();
    SharedMemoryError open(const char *shmName, int size, AccessMode mode); //Equaly for attach or create
    SharedMemoryError close();
    void *data();
protected:
    void *pData;
    char _name[MAXLENGTH];
    int _size;
    bool _isattached;
  #ifdef _WIN32
    HANDLE hShare;  // system Handle of share memory
  #endif            /* _WIN32 */
  #if defined(__linux) || defined(__APPLE__)
    int shmfd;
  #endif /* __linux || __APPLE__ */
};

class ISharedMemory :public BaseSharedMemory {
 public:
  ISharedMemory();
  ISharedMemory(const char * name, int pid=-1);
  ~ISharedMemory();
  bool attach(AccessMode mode = ReadWrite);
  const void *constData() const;
  bool create(int size, AccessMode mode = ReadWrite);
  void *data();
  bool detach();
  SharedMemoryError error() const;
  std::string errorString() const;
  bool isAttached() const;
  const char *key() const;
  bool lock();
  void setKey(const char *key);
  int totalSize() const;
  int size() const; //returns the size of the writable memory (total size - offset)
  void setSize(int);
  bool unlock();
  bool setOffset(int offset);
  int getOffset();
 private:
  bool loadSemaphore();
  bool loadTable();
  IShTable *ptrShmTable;
  BaseSharedMemory shmTable;
  SharedMemoryError _error;
  int findISharedMemory(const char*);
  int _pid;
  int tableIndex;
  int memOffset; //le indica a la memoria compartida a partir de cual byte tiene que leer/escribir
  static int _instance;
  HANDLE ghSemaphore;
  DWORD dwWaitResult;
};

#endif  // ISHAREDMEMORY_H

