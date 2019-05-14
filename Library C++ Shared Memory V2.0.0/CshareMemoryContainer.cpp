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

Version 2.0.1
************************************************************************************************/

#include "CshareMemoryContainer.h"
#include <stdio.h>
#include <string.h>

CShareMemoryContainer::CShareMemoryContainer()
{
}

CShareMemoryContainer::~CShareMemoryContainer()
{
    //removeAll();
}

void CShareMemoryContainer::closeAll()
{
    for (int i=0;i<getCount();i++){
        if(_allShareMemorys[i]->closeShareMemory()==false){
            printf("ERROR in 'CShareMemoryContainer::closeAll()': THE SHARE MEMORY WASN'T CLOSE \n");
        }
    }
}
void CShareMemoryContainer::removeAll()
{
    closeAll();
    for (int i=0;i<getCount();i++)
        delete _allShareMemorys[i];
    _allShareMemorys.clear();

}

void CShareMemoryContainer::removeFromID(int theID)
{
    for (int i=0;i<getCount();i++)
    {
        if (_allShareMemorys[i]->getID()==theID)
        {
            if(_allShareMemorys[i]->closeShareMemory()==false){
                printf("ERROR in 'CShareMemoryContainer::removeFromID': THE SHARE MEMORY WASN'T CLOSE \n");
            }
            delete _allShareMemorys[i];
            _allShareMemorys.erase(_allShareMemorys.begin()+i);
            break;
        }
    }
}
void CShareMemoryContainer::removeFromName(char *theName)
{
    
    for (int i=0;i<getCount();i++)
    {
        if(strcmp(_allShareMemorys[i]->getName(),theName)==0)
        {
            if(_allShareMemorys[i]->closeShareMemory()==false){
                printf("ERROR in 'CShareMemoryContainer::removeFromID': THE SHARE MEMORY WASN'T CLOSE \n");
            }
            delete _allShareMemorys[i];
            _allShareMemorys.erase(_allShareMemorys.begin()+i);
            break;
        }
    }
}
int CShareMemoryContainer::getNewID(){
    int newID=0;
    while (getFromID(newID)!=NULL)
        newID++;
    return(newID);
}

int CShareMemoryContainer::insert(CshareMemory* theShareMemory)
{   //This automatically assign ID
    int ID = getNewID();
    _allShareMemorys.push_back(theShareMemory);
    theShareMemory->setID(getNewID());
    return ID;
}

bool CShareMemoryContainer::insert(CshareMemory* theShareMemory, int theID)
{   //With this function you can assignd custom ID
    if (getFromID(theID)==NULL){
        _allShareMemorys.push_back(theShareMemory);
        theShareMemory->setID(theID);
        return true;
    }else
        printf(" ERROR in 'CShareMemoryContainer::insert(CshareMemory* theShareMemory, int theID)' : Id not valid \n");
    return false;
}


CshareMemory* CShareMemoryContainer::getFromID(int theID)
{
    for (int i=0;i<getCount();i++)
    {
        if (_allShareMemorys[i]->getID()==theID)
            return(_allShareMemorys[i]);
    }
    return(NULL);
}

CshareMemory* CShareMemoryContainer::getFromIndex(int ind)
{
    if ( (ind<0)||(ind>=getCount()) )
        return(NULL);
    return(_allShareMemorys[ind]);
}

CshareMemory* CShareMemoryContainer::getFromName(char *theName)
{
    for (int i=0;i<getCount();i++)
    {
        if(strcmp(_allShareMemorys[i]->getName(),theName)==0)
            return(_allShareMemorys[i]);
    }
    return(NULL);
}

int CShareMemoryContainer::getCount()
{
    return(int(_allShareMemorys.size()));
}

char* CShareMemoryContainer::getList()
{
    int count=getCount();
    char *stringOut=new char[40*count];
    stringOut[0]=0;
    for (int i=0;i<count;i++){
        CshareMemory* theShareMemory=getFromIndex(i);
        sprintf(stringOut,"%s %i %s %i\n",stringOut,theShareMemory->getID(),theShareMemory->getName(),theShareMemory->getSize());
    }
    return stringOut;
}


