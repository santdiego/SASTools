#ifndef CSHAREMEMORYCONTAINER_H
#define CSHAREMEMORYCONTAINER_H

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
 *06/07/2015: Se modifica Mecanismo de Cierre de memoria compartida.
************************************************************************************************/
#pragma once

#include <vector>
#include "CshareMemory.h"

class CShareMemoryContainer
{

public:
     CShareMemoryContainer();
     virtual ~CShareMemoryContainer();

     void removeAll();
     void closeAll();
     void removeFromID(int theID);
     void removeFromName(char *theName);
     int insert(CshareMemory* theShareMemory);
     bool insert(CshareMemory* theShareMemory, int theID);
     CshareMemory* getFromID(int theID);
     CshareMemory* getFromName(char *theName);
     CshareMemory* getFromIndex(int ind);
     int getCount();
     char *getList();
protected:
     std::vector<CshareMemory*> _allShareMemorys;
     int getNewID();
};

#endif // CSHAREMEMORYCONTAINER_H
