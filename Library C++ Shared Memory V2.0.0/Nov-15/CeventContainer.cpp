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
#include "CeventContainer.h"
#include <string.h>
#include <stdio.h>

CEventContainer::CEventContainer()
{
}

CEventContainer::~CEventContainer()
{
    closeAll();
}

void CEventContainer::closeAll()
{
    for (int i=0;i<getCount();i++){
        _allEvents[i]->close();
        delete _allEvents[i];
    }
    _allEvents.clear();
}

void CEventContainer::removeFromID(int theID)
{
    for (int i=0;i<getCount();i++)
    {
        if (_allEvents[i]->getID()==theID)
        {
            _allEvents[i]->close();
            delete _allEvents[i];
            _allEvents.erase(_allEvents.begin()+i);
            break;
        }
    }
}

int CEventContainer::insert(Cevent* theEvent)
{
    int newID=0;
    while (getFromID(newID)!=NULL)
        newID++;
    _allEvents.push_back(theEvent);
    theEvent->setID(newID);
    return(newID);
}

Cevent* CEventContainer::getFromID(int theID)
{
    for (int i=0;i<getCount();i++)
    {
        if (_allEvents[i]->getID()==theID)
            return(_allEvents[i]);
    }
    return(NULL);
}

Cevent* CEventContainer::getFromIndex(int ind)
{
    if ( (ind<0)||(ind>=getCount()) )
        return(NULL);
    return(_allEvents[ind]);
}

Cevent* CEventContainer::getFromName(char *theName)
{
    for (int i=0;i<getCount();i++)
    {
        if(strcmp(_allEvents[i]->getName(),theName)==0)
            return(_allEvents[i]);
    }
    return(NULL);
}

int CEventContainer::getCount()
{
    return(int(_allEvents.size()));
}

char* CEventContainer::getList()
{
    int count=getCount();
    char *stringOut=new char[40*count];
    for (int i=0;i<count;i++){
        Cevent* theEvent=getFromIndex(i);
        sprintf(stringOut,"%s%i %s\n",stringOut,theEvent->getID(),theEvent->getName());
    }
    return stringOut;
}

void CEventContainer::unlockAll()
{
    for (int i=0;i<getCount();i++){
        if(_allEvents[i]->waitForEvent(0)==false)
            _allEvents[i]->set();
    }
}



