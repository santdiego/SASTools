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

#include "Cevent.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

Cevent::Cevent(char *Name){
    _id=-1;
    setName(Name);
}

Cevent::~Cevent(){

}
bool Cevent::create(){
    #ifdef _WIN32
    hEvent=CreateEventA(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        TRUE,              // initial state is signaled
        cpName				// object name
    );
    if (hEvent == NULL){
        printf("CreateEvent failed (%d)\n", GetLastError());
        return false;
    }
    #endif
    return true;
}

bool Cevent::close(){
    set(); //unlock any WaitForSingleObject avoid freeze
#ifdef _WIN32
    if(CloseHandle(hEvent)!=0)
        return true;
#endif
    return false;
}

bool Cevent::set(){
#ifdef _WIN32
    if (SetEvent(hEvent)){
        return true;
    }else{
        printf("SetEvent failed (%d)\n", GetLastError());
        return false;
    }
#endif
    return false;
}

bool Cevent::reset(){
#ifdef _WIN32
    if (ResetEvent(hEvent)){
        return true;
    }else{
        printf("ResetEvent failed (%d)\n", GetLastError());
        return false;
    }
#endif
  return false;
}

bool Cevent::waitForEvent(unsigned long time_ms){
    //printf("\n Event name: %s",cpName);
#ifdef _WIN32
    unsigned long state= WaitForSingleObject(hEvent,time_ms);
    switch (state)
    {
        // Event object was signaled
        case WAIT_OBJECT_0:
            return true;
        // An error occurred
        default:
            return false;
    }
#endif
    return false;
}

void Cevent::setID(int newId){
    _id=newId;
}

int  Cevent::getID(){
        return(_id);
}

char* Cevent::getName(){
    return cpName;
}

void Cevent::setName(char *name){
    if(name!=nullptr && (strlen(name)<MAX_PATH)){
        strcpy(cpName,name);
    }
}
