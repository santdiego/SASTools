#include "isharedmemorycontainer.h"
#include <stdio.h>
#include <string.h>

ISharedMemoryContainer::ISharedMemoryContainer(){

}
void ISharedMemoryContainer::closeAll()
{
    for (int i=0;i<getCount();i++){
        if(_allShareMemorys[i]->detach()==false){
            printf("ERROR in 'ISharedMemoryContainer::closeAll()': THE SHARE MEMORY WASN'T CLOSE \n");
        }
    }
}
void ISharedMemoryContainer::removeAll()
{
    closeAll();
    for (int i=0;i<getCount();i++){
        _allShareMemorys[i]->close();
        delete _allShareMemorys[i];
    }
    _allShareMemorys.clear();

}

void ISharedMemoryContainer::removeFromName(const char *theName, int offset)
{

    for (int i=0;i<getCount();i++)
    {
        if(strcmp(_allShareMemorys[i]->key(),theName)==0)
        {  
            if(offset>=0){
                //ssPrintf("_allShareMemorys[i]->getOffset: %i\n",_allShareMemorys[i]->getOffset());
                //ssPrintf("offset %i\n",offset);

        		if(_allShareMemorys[i]->getOffset()==offset){
                    printf("_allShareMemorys[i]->detach()\n");
        		    if(_allShareMemorys[i]->detach()==false)
                        printf("ERROR in 'ISharedMemoryContainer::removeFromID': THE SHARE MEMORY WASN'T CLOSE \n");
                    _allShareMemorys[i]->close();
        		    delete _allShareMemorys[i];
        		    _allShareMemorys.erase(_allShareMemorys.begin()+i);
        		}
        	}
        	else{
        		if(_allShareMemorys[i]->getOffset()==offset){
        		    if(_allShareMemorys[i]->detach()==false)
                        printf("ERROR in 'ISharedMemoryContainer::removeFromID': THE SHARE MEMORY WASN'T CLOSE \n");
                    _allShareMemorys[i]->close();
        		    delete _allShareMemorys[i];
        		    _allShareMemorys.erase(_allShareMemorys.begin()+i);
        		}
        	}
            break;
        }
    }
}

ISharedMemory* ISharedMemoryContainer::at(int idx)
{
	if(idx<getCount()){
		return(_allShareMemorys[idx]);
	}
    return 0;
}

void ISharedMemoryContainer::insert(ISharedMemory* theShareMemory)
{
    _allShareMemorys.push_back(theShareMemory);
}

ISharedMemory* ISharedMemoryContainer::getFromName(const char *theName, int offset)
{
    for (int i=0;i<getCount();i++)
    {
        if(strcmp(_allShareMemorys[i]->key(),theName)==0){
        	if(offset>=0){
        		if(_allShareMemorys[i]->getOffset()==offset)
        			return(_allShareMemorys[i]);
        	}
        	else{
        		return(_allShareMemorys[i]);
        	}
        }
    }
    return(0);
}

int ISharedMemoryContainer::getCount()
{
    return(int(_allShareMemorys.size()));
}



