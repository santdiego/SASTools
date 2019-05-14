#ifndef CEVENTCONTAINER_H
#define CEVENTCONTAINER_H

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
************************************************************************************************/

#pragma once

#ifndef NULL
#define NULL nullptr
#endif

#include <vector>
#include "Cevent.h"

class CEventContainer
{

public:
     CEventContainer();
     virtual ~CEventContainer();

     void closeAll();
     void removeFromID(int theID);
     int insert(Cevent* theShareMemory);
     Cevent* getFromID(int theID);
     Cevent* getFromName(char *theName);
     Cevent* getFromIndex(int ind);
     int getCount();
     char *getList();
     void unlockAll();
protected:
     std::vector<Cevent*> _allEvents;
};


#endif // CEVENTCONTAINER_H
