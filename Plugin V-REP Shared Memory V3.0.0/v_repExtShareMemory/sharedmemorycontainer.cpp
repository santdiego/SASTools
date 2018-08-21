#include "sharedmemorycontainer.h"

SharedMemoryContainer::SharedMemoryContainer()
{
}


void SharedMemoryContainer::closeAll()
{
    for (size_t i=0;i<_allShareMemorys.size();i++){
        if(_allShareMemorys[i].ptr_shm->detach()==false){
            printf("ERROR in 'QSharedMemoryContainer::closeAll()': THE SHARE MEMORY WASN'T CLOSE \n");
        }
    }
}
void SharedMemoryContainer::removeAll()
{
    closeAll();
    for (size_t i=0;i<_allShareMemorys.size();i++)
        delete _allShareMemorys[i].ptr_shm;
    _allShareMemorys.clear();

}

void SharedMemoryContainer::removeFromID(int theID)
{
    QString name=getNames(theID);
    if(name!=QString("Error")){
        for (size_t i=0;i<_allShareMemorys.size();i++)
        {
            if (QString::compare(_allShareMemorys[i].ptr_shm->nativeKey(),name)==0)
            {
                if(_allShareMemorys[i].ptr_shm->isAttached()==true){
                    if(_allShareMemorys[i].ptr_shm->detach()==false){
                        printf(_allShareMemorys[i].ptr_shm->errorString().toLatin1().data());
                    }
                }
                delete _allShareMemorys[i].ptr_shm;
                _allShareMemorys.erase(_allShareMemorys.begin()+i);
                break;
            }
        }
    }
}

int SharedMemoryContainer::insert(QSharedMemory* theShareMemory)
{   //Look for the name, if it is not registered, add it.
    //return -1 means error
    int ID=-1;
    SHM_descriptor aux;
    for(size_t i=0;i<_names.size();i++){
        if(QString::compare(theShareMemory->nativeKey(),_names[i])==0){ //The name exists, give it the ID
            ID=(int)i;
        }
    }
    if(ID==-1){
        _names.push_back(theShareMemory->nativeKey());
        ID=(int)_names.size()-1;
    }
    aux.ID=ID;
    aux.ptr_shm=theShareMemory;
    _allShareMemorys.push_back(aux);
    return ID;
}

QSharedMemory* SharedMemoryContainer::getFromID(int theID)
{
    if (theID<_names.size()){
        for (size_t i=0;i<_allShareMemorys.size();i++)
        {
            if (_allShareMemorys[i].ID==theID){
                return(_allShareMemorys[i].ptr_shm);
            }
        }
    }
    else printf("theID does not exist");
    return(NULL);


}


QSharedMemory* SharedMemoryContainer::getFromName(QString theName)
{
    for (size_t i=0;i<_allShareMemorys.size();i++)
    {
        if(QString::compare(_allShareMemorys[i].ptr_shm->nativeKey(),theName)==0)
            return(_allShareMemorys[i].ptr_shm);
    }
    return(NULL);
}

int SharedMemoryContainer::getNameID(QString theName){
    for(size_t i=0;i<_names.size();i++){
        if(QString::compare(_names[i],theName)==0)
            return (int)i;
    }
    return -1;
}

int SharedMemoryContainer::getSize(){
    return (int)_allShareMemorys.size();
}

char* SharedMemoryContainer::getList()
{
    size_t count=_allShareMemorys.size();
    char *stringOut=new char[40*count];
    stringOut[0]=0;
    for (size_t i=0;i<count;i++){
        //QSharedMemory* theShareMemory=getFromIndex(i);
        //sprintf(stringOut,"%s %i %s %i\n",stringOut,theShareMemory->getID(),theShareMemory->getName(),theShareMemory->getSize());
    }
    return stringOut;
}

QString SharedMemoryContainer::getNames(int names_index){
    if(names_index<_names.size())
        return _names[names_index];
    else return (QString("Error"));
}
