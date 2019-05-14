#include "xxxtosharedmemory.h"

XXXtoSharedMemory::XXXtoSharedMemory(QObject *parent,QString toDevKey, QString fromDevKey) : QObject(parent)

{
    //toDevKey: data from other device to this
    //fromDevkey: data from this device to another
    inbox= new QSharedMemory(toDevKey);
    outbox= new QSharedMemory(fromDevKey);
    if(!inbox->attach(QSharedMemory::ReadOnly)){
        //ERROR
    }
    if(!outbox->attach(QSharedMemory::ReadWrite)){
            //ERROR
    }
}
