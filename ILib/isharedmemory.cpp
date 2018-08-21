#include "isharedmemory.h"
#include <iostream>
#include  <sys/types.h>
#ifdef _WIN32
#include <process.h>
#endif
using namespace std;

BaseSharedMemory::BaseSharedMemory(){
    pData=nullptr;
    hShare=nullptr;
    _size=0;
    _isattached=false;

}
BaseSharedMemory::~BaseSharedMemory(){
    cout<<"~BaseSharedMemory() name:"<<_name<<endl;
    if(_isattached)
        close();
}
SharedMemoryError BaseSharedMemory::close(){
    //ssPrintf("BaseSharedMemory::close() %s \n",_name);
    SharedMemoryError ret =NoError;
    if (!_isattached){
        //cout<<"ISharedMemory::detach() !_is_attached: NAME:"<<_name<<endl;
        return NotFound;
    }
#ifdef _WIN32
    try{
        if((pData != nullptr)) {
            if( FlushViewOfFile(pData, _size) == 0){
                ret= UnknownError;
            }
            if( UnmapViewOfFile(pData) == 0){
                ret= UnknownError;
            }
            _isattached=false;
            pData=nullptr;
        }
        //        if(hShare!=nullptr)
        //            if( CloseHandle(hShare) == 0)
        //                ret= UnknownError;
    }
    catch (exception& e){
        cout << e.what() << '\n';
    }


#endif
#if defined (__linux) || defined (__APPLE__)
    if (shm_unlink(name) != 0) {
        perror("In shm_unlink()");
        return false;
    }
#endif /* __linux || __APPLE__ */

    hShare=nullptr;
    _isattached=false;
    return ret;
}

SharedMemoryError BaseSharedMemory::open(const char *shmName, int shmSize, AccessMode mode = ReadWrite){
    if (strlen(shmName)<MINLENGTH){
        return KeyError;
    }

    if(shmSize<=0){
        return InvalidSize;
    }

#ifdef _WIN32
    DWORD access;

    switch(mode){
    case ReadOnly:
        access =PAGE_READONLY;
        break;
    case ReadWrite:
        access =PAGE_READWRITE;
        break;
    default:
        access =PAGE_READWRITE;
    }

    hShare = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, access, 0,shmSize, shmName);
    if(hShare == nullptr){
        return PermissionDenied;
    }else{
        pData = MapViewOfFile(hShare,FILE_MAP_ALL_ACCESS,0, 0, shmSize);
    }
    if(pData == nullptr){
        return UnknownError;
    }

#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
    /* creating the shared memory object    --  shm_open()  */
    shmfd = shm_open(cpName, O_CREAT|O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0) {
        perror("In shm_open()");
        return false;
    }
    //printf(stderr, "Created shared memory object %s\n", shmName);

    /* adjusting mapped file size (make room for the whole segment to map)      --  ftruncate() */
    ftruncate(shmfd, shmSize);
    /* requesting the shared segment    --  mmap() */
    pData = mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (pData == nullptr) {
        perror("In mmap()");
        return false;
    }
    //printf(stderr, "Shared memory segment allocated correctly (%d bytes).\n", _size);
#endif /* __linux || __APPLE__ */
    //ALL OK
    strcpy(_name,shmName);
    _size=shmSize;
    _isattached=true;
    if(GetLastError()==ERROR_ALREADY_EXISTS){
        return AlreadyExists;
    }
    else{
        memset(pData,0,shmSize);
    }
    return NoError;
}

void* BaseSharedMemory::data() {
    if (!_isattached){
        //_error=NotFound;
        return nullptr;
    }
    return pData;
}
/////////////////////////ISharedMemory/////////////////////////////////////////
int ISharedMemory::_instance = 0;



ISharedMemory::ISharedMemory() :BaseSharedMemory(){
    ghSemaphore=nullptr;
    _pid=_getpid();
    _instance++;
    if(!loadTable())
        exit(1);
    memOffset=0;
    _size=0;
}

ISharedMemory::ISharedMemory(const char* name, int pid) {
    ghSemaphore=nullptr;
    //cout<<"ISharedMemory"<<endl;
	if (pid == -1)
        _pid=_getpid();
    else
        _pid=pid;
    _instance++;
    setKey(name);
    if(!loadTable())
        cerr<<"Could Not load Table"<<endl;
    memOffset=0;
    _size=0;
}
bool ISharedMemory::loadTable(){
    //cout<<"loadTable"<<endl;
    _error=shmTable.open(GLOBALTABLENAME,sizeof(IShTable),ReadWrite);
    switch(_error){
    case AlreadyExists:
        break;
    case NoError:
        break;
    default:
        cerr<<"Error "<< _error<<", loading ITable "<<endl;
        return false;
    }
    ptrShmTable=static_cast<IShTable*>(shmTable.data());
    if(ptrShmTable==nullptr){
        cout<<_name<<endl;
        return false;
    }
    if(findISharedMemory(GLOBALTABLENAME)!=0){ //No hay ISharedMemory Cargados
        strcpy(ptrShmTable->name[0],GLOBALTABLENAME);
        ptrShmTable->size[0]=sizeof(IShTable);
        ptrShmTable->pid[0][0]=_pid;
        ptrShmTable->num=1;
        for (int i=1;i<MAXISHM;i++){ //clean table
            for(int j=0;j<MAXPROCESSATTACHED;j++)
                ptrShmTable->pid[i][j]=-1;
            for(int j=0;j<MAXPROCESSESWAITING;j++)
                ptrShmTable->lock[i][j]=-1;
            ptrShmTable->size[i]=-1;
        }
    }
    return true;
}
void ISharedMemory::setSize(int size){
    _size=size;
}

ISharedMemory::~ISharedMemory() {
    cout<<"ISharedMemory::~ISharedMemory()"<<endl;
    if(isAttached()){
        detach();
    }
    _instance--;
}

int ISharedMemory::findISharedMemory(const char *lookFor){
    //cout<<"findISharedMemory"<<endl;
    if(ptrShmTable==nullptr)
       return -1;
    for (int i=0;i<MAXISHM;i++){
        if(strcmp(ptrShmTable->name[i],lookFor)==0){
            return i;
        }
    }
    return -1;
}

bool ISharedMemory::create(int size, AccessMode mode) {
    //cout<<"create"<<endl;
    bool ret=false;
    if(ptrShmTable==nullptr){
        _error=TableError;
        return false;
    }
    if(findISharedMemory(_name)>-1){
        return attach(mode);
    }
    _isattached=false;
    if (size<=0){
        _error= InvalidSize;
        return false;
    }
    //ATENCION USANDO LA LIBRERIA SIEMPRE SE CREA LA SHM COMO NO NATIVA
    _error=open(_name,size,mode);
    switch(_error){
    case NoError:
    { //Actualizar tabla
        _isattached=true;
        int i =1;
        for(i=1;i<MAXISHM;i++)
            if(ptrShmTable->size[i]<0)
                break;
        tableIndex=i;
        strcpy(ptrShmTable->name[i],_name);
        ptrShmTable->size[i]=size;
        ptrShmTable->pid[i][0]=_pid;
        for(int j=0;j<MAXPROCESSESWAITING;j++)
            ptrShmTable->lock[i][j]=-1;
        ptrShmTable->num++;
        ret= true;
        break;
    }
    case AlreadyExists:
    { //La memoria no es del tipo ISharedMemory pero fue creada : La agrego a la tabla
        _isattached=true;
        int i =1;
        for(i=1;i<MAXISHM;i++)
            if(ptrShmTable->size[i]<0)
                break;
        tableIndex=i;
        strcpy(ptrShmTable->name[i],_name);
        ptrShmTable->size[i]=size;
        int j=0;
        for(j=0;j<MAXPROCESSATTACHED;j++)
            if(ptrShmTable->pid[tableIndex][j]==_pid)
                break;
        ptrShmTable->pid[i][j]=_pid;
        for(j=0;j<MAXPROCESSESWAITING;j++)
            ptrShmTable->lock[i][j]=-1;
        ptrShmTable->num++;
        ret= false;
        break;
    }
    default:
    {
        return false;
    }
    }

    if(!loadSemaphore())
        ret=false;
    return ret;
}

bool ISharedMemory::attach(AccessMode mode) {
    //cout<<"attach"<<endl;
    _isattached=false;
    if(strlen(_name)<MINLENGTH){
        //cerr<<"Shared Memory key not valid"<<endl;
        _error=KeyError;
        return false;
    }
    tableIndex=findISharedMemory(_name);
    if(tableIndex>0){
        _size=ptrShmTable->size[tableIndex];
        if(_size>0){
            _error=open(_name,_size,mode);
            switch(_error){
            case NoError :
                _isattached=true;
                loadSemaphore();
                return true;
            case AlreadyExists:
                _isattached=true;
                loadSemaphore();
                _error=NoError;
                return true;
            default:
                return false;
            }
        }
        else{
            //cerr<<"Invalid size"<<endl;
            _error= InvalidSize;
            return false;
        }
    }
    else{
        //cerr<<"Memory not found"<<endl;
        _error= NotFound;
        return false;
    }
}

bool ISharedMemory::detach() {
    cout<<"ISharedMemory::detach() Index:"<<tableIndex<<"name: "<<ptrShmTable->name[tableIndex]<<endl;
    if(!_isattached)
        return false;

    if(ghSemaphore!=nullptr){
        ::CloseHandle(ghSemaphore);
    }
    if(hShare!=nullptr){
        ::CloseHandle(hShare);
    }

    bool last=true;
    for(int j=0;j<MAXPROCESSATTACHED;j++){
        if(ptrShmTable->pid[tableIndex][j]==_pid)
            ptrShmTable->pid[tableIndex][j]=-1;
        if(ptrShmTable->pid[tableIndex][j]>0){
            last=false;
            break;
        }
    }
    if(last){
        cout<<"erase Table: Index:%i , name:%s "<<tableIndex<<ptrShmTable->name[tableIndex]<<endl;
        strcpy(ptrShmTable->name[tableIndex],"none");
        ptrShmTable->size[tableIndex]=-1;
        for (int j=0;j<MAXPROCESSATTACHED;j++)
            ptrShmTable->pid[tableIndex][j]=-1;
        for(int j=0;j<MAXPROCESSESWAITING;j++)
            ptrShmTable->lock[tableIndex][j]=-1;
        ptrShmTable->num--;
    }

    close();
    return true;
}


const void* ISharedMemory::constData() const {
    if (!_isattached){
        //_error=NotFound;
        return nullptr;
    }
    return pData;
}

void* ISharedMemory::data() {
    if (!_isattached){
        _error=NotFound;
        return nullptr;
    }
    return (static_cast<char*>(pData)+memOffset);
}

bool ISharedMemory::setOffset(int offset){
    if(offset<_size){
        memOffset=offset;
        return true;
    }
    else
        return false;
}
int ISharedMemory::getOffset(){
    return memOffset;
}



SharedMemoryError ISharedMemory::error() const {
    return _error;
}

std::string ISharedMemory::errorString() const {
    string the_error;
    switch(_error){
    case NoError:
        the_error= string("NoError : No error occurred.");
        break;
    case PermissionDenied:
        the_error=string("PermissionDenied: The operation failed because the caller didn't have the required permissions.");
        break;
    case InvalidSize:
        the_error=string("InvalidSize: A create operation failed because the requested size was invalid.");
        break;
    case KeyError:
        the_error=string("KeyError: The operation failed because of an invalid key.");
        break;
    case AlreadyExists:
        the_error=string("AlreadyExists: A create() operation failed because a shared memory segment with the specified key already existed.");
        break;
    case NotFound:
        the_error=string("NotFound: An attach() failed because a shared memory segment with the specified key could not be found.");
        break;
    case LockError:
        the_error=string("LockError:The attempt to lock() the shared memory segment failed because create() or attach() failed and returned false.");
        break;
    case OutOfResources:
        the_error=string("OutOfResources: A create() operation failed because there was not enough memory available to fill the request");
        break;
    case UnknownError:
        the_error=string("UnknownError: Something else happened and it was bad.");
        break;
    case TableError:
        the_error=string("ShmTable could not be found");
        break;
    }
    return the_error;
}

bool ISharedMemory::isAttached() const {
    return _isattached;
}

bool ISharedMemory::lock() {
    //cout<<"lock: "<<_name<<endl;
    if (!_isattached){
        _error=NotFound;
        return false;
    }

#if defined(__linux) || defined(__APPLE__)
    for(int i=0;i<MAXPROCESSESWAITING;i++){
        if(ptrShmTable->lock[tableIndex][i]<0){
            ptrShmTable->lock[tableIndex][i]=_pid;
            break;
        }
    }

    while(1){

        if(ptrShmTable->lock[tableIndex][0]==_pid)
            break;
        if(ptrShmTable->lock[tableIndex][0]<0)
            break;
        for(int i=0;i<MAXPROCESSESWAITING;i++)
            cout<<"j"<<i<<"value"<<ptrShmTable->lock[tableIndex][i]<<endl;
    }
    //FALTA UN SLEEP FLEXIBLE EN us
    return true;

#endif /* __linux || __APPLE__ */

#ifdef _WIN32


    dwWaitResult = WaitForSingleObject(ghSemaphore,1L);
    switch (dwWaitResult){

    case WAIT_OBJECT_0: // The semaphore object was signaled.
        return true;
    case WAIT_TIMEOUT:
        cerr<<"wait timed out"<<endl;// The semaphore was nonsignaled, so a time-out occurred.
        _error=LockError;
        return false;
    }
    return false;
#endif
    /* _WIN32 */
}

bool ISharedMemory::unlock() {
    //cout<<"unlock: "<<_name<<endl;
    if (!_isattached){
        _error=NotFound;
        return false;
    }
#if defined(__linux) || defined(__APPLE__)
    if(ptrShmTable->lock[tableIndex][0]!=_pid){
        cerr<<"ISharedMemory::unlock(): Unknown error"<<endl;
        //exit(1);
        return false;
    }
    for(int j=0;j<MAXPROCESSESWAITING-1;j++){
        ptrShmTable->lock[tableIndex][j]=ptrShmTable->lock[tableIndex][j+1];
    }
    ptrShmTable->lock[tableIndex][MAXPROCESSESWAITING-1]=-1;
    return true;
#endif /* __linux || __APPLE__ */
    // Release the semaphore when task is finished
#ifdef _WIN32
    if (ghSemaphore!=nullptr){
        if (!ReleaseSemaphore(ghSemaphore,1,nullptr))
        {
            cerr<<"ReleaseSemaphore :"<<_name<<". Error: "<<GetLastError()<<endl;
            _error=LockError;
            loadSemaphore(); //try to reload semaphore
            return false;
        }
    }
    return true;
#endif
    /* _WIN32 */
}

const char* ISharedMemory::key() const {
    return _name;
}


void ISharedMemory::setKey(const char* key) {
    strcpy(_name,key);
}

int ISharedMemory::totalSize() const{
    if (!_isattached){
        //_error=NotFound;
        return 0;
    }
    return _size;
}
int ISharedMemory::size() const {
    if (!_isattached){
        //_error=NotFound;
        return 0;
    }
    return (_size-memOffset);
}

bool ISharedMemory::loadSemaphore(){
    if(ghSemaphore!=nullptr)
        CloseHandle(ghSemaphore);
    string semName(_name);
    semName+="_sem";
    ghSemaphore = CreateSemaphore(
                nullptr,           // default security attributes
                MAX_SEM_COUNT,  // initial count
                MAX_SEM_COUNT,  // maximum count
                semName.c_str());          // unnamed semaphore
    if(ghSemaphore == nullptr){
        _error=UnknownError;
        return false;
    }
    return true;
}
/*******************PRIVATE SECTION *******************/
