#ifndef ISHAREDMEMORYCONTAINER_H
#define ISHAREDMEMORYCONTAINER_H


#include "isharedmemory.h"

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

Version 0.0.1
************************************************************************************************/
#include <vector>

class ISharedMemoryContainer
{
public:
     ISharedMemoryContainer();

     void removeAll();
     void closeAll();
     void removeFromName(const char *theName, int offset =-1);
     void insert(ISharedMemory* theShareMemory);
     ISharedMemory* getFromName(const char *theName, int offset=-1);
     ISharedMemory* at(int index);
     int getCount();
     char *getList();
protected:
     std::vector<ISharedMemory*> _allShareMemorys;
     int getNewID();
};


#endif // ISHAREDMEMORYCONTAINER_H
