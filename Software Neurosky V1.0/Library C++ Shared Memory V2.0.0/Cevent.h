#ifndef CEVENT_H
#define CEVENT_H

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

#ifdef _WIN32
    #include <windows.h>
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    #include <sys/shm.h>
    #include <sys/types.h>
    #include <sys/ipc.h>
#define MAX_PATH 100
#endif /* __linux || __APPLE__ */


class Cevent
{
public:
    Cevent(char *cpName);
    virtual ~Cevent();
    //Events functions
    bool create();
    bool close();
    bool set();
    bool reset();
    bool waitForEvent(unsigned long time);
    void setID(int newId);
    int getID();
    char* getName();
    void setName(char *name);

protected:
    void* hEvent;	//Handle
    char cpName[40];	//Name
    int _id;		// Id
};

#endif // CEVENT_H
