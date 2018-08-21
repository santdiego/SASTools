// Copyright 2006-2017 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.4.0 rev. 1 on April 5th 2017

#include "v_repExtShareMemory.h"
#include "stackArray.h"
#include "stackMap.h"
#include "v_repLib.h"
#include <iostream>
#include "sharedmemorycontainer.h"

#ifdef _WIN32
    #ifdef QT_COMPIL
        #include <direct.h>
    #else
        #include <shlwapi.h>
        #pragma comment(lib, "Shlwapi.lib")
    #endif
#else
    #include <unistd.h>
#endif

#ifdef __APPLE__
#define _stricmp strcmp
#endif

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)    CONCAT(x,y,z)

#define PLUGIN_VERSION 4 // 2 since version 3.2.1, 3 since V3.3.1, 4 since V3.4.0

LIBRARY vrepLib; // the V-REP library that we will dynamically load and bind


/********************************* SHARED MEMORY SPECIFIC FUNCITONS***********************/
SharedMemoryContainer *shm_Container;


void simExtShareMemoryOpen(SLuaCallBack* p)
{
    // LUA prototype: "number result,number ID=simExtOpenShareMemory(string Name,int size)"
    int result=-1; // -1 means error
    int ID;

    if (p->inputArgCount>0){ // Ok, we have at least 1 input arguments : One for Memory Name
        if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_string && strlen(p->inputChar)>2){ // Ok, we have and string and it has at least 3 characters
            if(shm_Container->getFromName(p->inputChar)==NULL){//Check if the share memory already exist
                if(p->inputArgCount>1 && p->inputArgTypeAndSize[1*2+0]>0){ //the size of the memory was specified
                    QSharedMemory *newShareMemory=new QSharedMemory; //Create New Share Memory with custom size
                    newShareMemory->setNativeKey(p->inputChar);
                    if (!newShareMemory->create(p->inputInt[0]))
                    {
                        if (newShareMemory->isAttached()==true)
                            newShareMemory->detach();
                        if(newShareMemory->attach()==true){
                            result=1;
                        }else {
                            simSetLastError("simExtShareMemoryOpen",newShareMemory->errorString().toLatin1().data()); // output an error
                            delete newShareMemory;
                        }

                    }else result=1;
                    if (result=1)
                        ID=shm_Container->insert(newShareMemory); //Store and Asign an ID
                }else{//the size of the memory wasn't specified
                    QSharedMemory *newShareMemory=new QSharedMemory(); //Create New Share Memory with default size
                    if (!newShareMemory->create(1000)){
                        if (newShareMemory->isAttached()==true)
                            newShareMemory->detach();
                        if(newShareMemory->attach()==true){
                            ID=shm_Container->insert(newShareMemory); //Store and Asign an ID
                            result=1;
                        }else simSetLastError("simExtShareMemoryOpen","Undable to Open Share Memory with default size"); // output an error
                    }

                }
            }else{

                ID=shm_Container->getNameID(p->inputChar);
                result=0; //It means that any error occurred, but the result is valid.
            }
        }else simSetLastError("simExtShareMemoryOpen","Wrong argument type/size."); // output an error
    }else simSetLastError("simExtShareMemoryOpen","Not enough arguments."); // output an error

    // Now we prepare the return value(s):

    p->outputArgCount=2; // 2 return values (function succeeded)

    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    p->outputInt=(simInt*)simCreateBuffer(2*sizeof(int)); // 2 integer return value

    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;		// The first return value is an int
    p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
    p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_int;		// The second return value is a int
    p->outputArgTypeAndSize[2*1+1]=1;					// Not used (table size if the return value was a table)

    p->outputInt[0]=result;
    if (result!=-1)
        p->outputInt[1]=ID;
    else
        p->outputInt[1]=-1;

}

void simExtShareMemoryClose(SLuaCallBack* p){ // Callback of the Lua simExtCloseShareMemory command

    // LUA prototype: "number result=simExtShareMemoryClose(number ID)"
    //				  "number result=simExtShareMemoryClose()"

    int result=-1; // -1 means error
    if (p->inputArgCount>0){ //The Id from memory to close
        if ( (p->inputArgTypeAndSize[0]==(sim_lua_arg_int))){ //Find the share memory already exist
            if(shm_Container->getFromID(p->inputInt[0])!=NULL){
                shm_Container->removeFromID(p->inputInt[0]);
                result=1;
            }else simSetLastError("simExtShareMemoryClose","Shared Memory does not exist or is already closed"); // output an error
        }else simSetLastError("simExtShareMemoryClose","Wrong argument type/size."); // output an error
    }else{ //no entry specified, close all shared memories
        shm_Container->removeAll();
        result=1;
    }

    // Now we prepare the return value(s):

    p->outputArgCount=1; // 2 return values (function succeeded)

    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int)); // 2 integer return value

    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;		// The first return value is an int
    p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)

    p->outputInt[0]=result;
}

void simExtShareMemoryRead(SLuaCallBack* p){
    //LUA prototyoe: "number result,charbuff data=simExtShareMemoryRead(int ID)"
    int result=-1; // -1 means error
    int size=0;
    //Prepare the return value(s):
    p->outputArgCount=2; // 2 return values (function succeeded)
    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;
    p->outputArgTypeAndSize[2*0+1]=1;
    p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_charbuff;
    p->outputArgTypeAndSize[2*1+1]=0;
    p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int));

    if (p->inputArgCount>0){
        if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int){
            QSharedMemory *theMemory=shm_Container->getFromID(p->inputInt[0]);
            if(theMemory!=NULL){ //Check if the share memory exist
                size=theMemory->size();
                p->outputArgTypeAndSize[2*1+1]=size;
                p->outputCharBuff=(simChar*)simCreateBuffer(size);
                //theMemory->lock();
                memcpy(p->outputCharBuff, theMemory->constData(),size );
                result=1;
               // theMemory->unlock();
                //if (theMemory->error()==QSharedMemory::NoError){
                //    result=1;
                //}else simSetLastError("simExtShareMemoryRead","Error reading data"); // output an error
            }else simSetLastError("simExtShareMemoryRead","Shared Memory wasn't found"); // output an error
        }else simSetLastError("simExtShareMemoryRead","Wrong argument type/size."); // output an error
    }else simSetLastError("simExtShareMemoryRead","Not enough arguments."); // output an error

    p->outputInt[0]=result;
}

void simExtShareMemoryWrite(SLuaCallBack* p){ // Callback of the Lua simExtWriteShareMemory command

    //LUA prototype : "number result=simExtShareMemoryWrite(int Id,charbuff data)"
    int result=-1; // -1 means error
    int size;
    if (p->inputArgCount>1){ // Ok, we have at least 2 input arguments : One for Memory Id and One for Memory data
        if ((p->inputArgTypeAndSize[0*2+0]==(sim_lua_arg_int))&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_charbuff))
        { //Check if the share memory already exist
            size=p->inputArgTypeAndSize[1*2+1];
            QSharedMemory *theMemory=shm_Container->getFromID(p->inputInt[0]);

            if(theMemory!=NULL){
                //Uncoment for use non native shared memory
                if(size>0 && size<=theMemory->size()){
                    //theMemory->lock();
                    memcpy(theMemory->data(),p->inputCharBuff,size);
                    result=1;
                    //theMemory->unlock();
                    //if(theMemory->error()==QSharedMemory::NoError)
                    //    result=1;
                    //else simSetLastError("simExtShareMemoryWrite","Shared memory was not written."); // output an error
                }else simSetLastError("simExtShareMemoryWrite","The data size is larger than the memory size or null"); // output an error
            }else simSetLastError("simExtShareMemoryWrite","Shared Memory wasn't found"); // output an error
        }else simSetLastError("simExtShareMemoryWrite","Wrong argument type/size."); // output an error
    }else simSetLastError("simExtShareMemoryWrite","Not enough arguments."); // output an error

    // Now we prepare the return value(s):
    p->outputArgCount=1; // 1 return values (function succeeded)
    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
    p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int)); // 2 integer return value
    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;		// The first return value is an int
    p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
    p->outputInt[0]=result;
}

void simExtShareMemoryGetId(SLuaCallBack* p)
{
    //LUA prototype : "number result,number ID=simExtShareMemoryGetId(string Name)"

    int result=-1; // -1 means error
    int ID;
    if (p->inputArgCount>0){
        if ( (p->inputArgTypeAndSize[0]==(sim_lua_arg_string))){ //The memory Id
            QSharedMemory* theShareMemory=shm_Container->getFromName(p->inputChar);
            if(theShareMemory!=NULL){//Check if the share memory already exist
                ID=shm_Container->getNameID(QString(p->inputChar)); //Store and Asign an ID
                result=1;
            }else simSetLastError("simExtShareMemoryGetId","Shared Memory was not found"); // output an error
        }else simSetLastError("simExtShareMemoryGetId","Wrong argument type/size."); // output an error
    }else simSetLastError("simExtShareMemoryGetId","Not enough arguments."); // output an error

    // Now we prepare the return value(s):

    p->outputArgCount=2; // 2 return values (function succeeded)

    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    p->outputInt=(simInt*)simCreateBuffer(2*sizeof(int)); // 2 integer return value

    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;		// The first return value is an int
    p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
    p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_int;		// The second return value is a int
    p->outputArgTypeAndSize[2*1+1]=1;					// Not used (table size if the return value was a table)

    p->outputInt[0]=result;
    if (result!=-1)
        p->outputInt[1]=ID;
    else
        p->outputInt[1]=-1;
}

void simExtShareMemoryGetName(SLuaCallBack* p){

    //LUA prototyoe: "number result,string name=simExtShareMemoryGetName()"
    int result=-1; // -1 means error
    char *name=NULL;
    if (p->inputArgCount>0)	{ // Ok, we have at least 1 Memory Id
        if ( (p->inputArgTypeAndSize[0]==(sim_lua_arg_int))){
            QSharedMemory* theShareMemory=shm_Container->getFromID(p->inputInt[0]);
            if(theShareMemory!=NULL){//Check if the share memory already exist
                name=theShareMemory->nativeKey().toLatin1().data(); //convert QString to QByteArray and get *char
            if(theShareMemory->error()==QSharedMemory::NoError)
                result=1;
            }else simSetLastError("simExtShareMemoryGetName","Shared Memory was not found"); // output an error
        }else simSetLastError("simExtShareMemoryGetName","Wrong argument type/size."); // output an error
    }else simSetLastError("simExtShareMemoryGetName","Not enough arguments."); // output an error

    // Now we prepare the return value(s):

    p->outputArgCount=2; // 2 return values (function succeeded)

    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    p->outputInt=(simInt*)simCreateBuffer(sizeof(int)); // integer return value
    if(name!=NULL)
        p->outputChar=(simChar*)simCreateBuffer((simInt)(strlen(name)+1)); // integer return value

    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;		// The first return value is an int
    p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)

    p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_string;	// The first return value is an int
    p->outputArgTypeAndSize[2*1+1]=1;					// Not used (table size if the return value was a table)

    p->outputInt[0]=result;
    if (result!=-1){
        strcpy(p->outputChar,name);
    }
}

void simExtShareMemoryList(SLuaCallBack* p){
    //LUA prototype: "table ID,table names,table sizes=simExtShareMemoryList()"
    //char *stringOut=new char[200];
    int count=shm_Container->getSize();
    QSharedMemory* theShareMemory;
    char *pt;
    p->outputArgCount=3; // 2 return values (function succeeded)
    p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int|sim_lua_arg_table;			// The first return value is an int
    p->outputArgTypeAndSize[2*0+1]=count;					// table size
    p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_string|sim_lua_arg_table;		// The second return value is a string
    p->outputArgTypeAndSize[2*1+1]=count;					// table size
    p->outputArgTypeAndSize[2*2+0]=sim_lua_arg_int|sim_lua_arg_table;			// The second return value is a int
    p->outputArgTypeAndSize[2*2+1]=count;					// table size

    p->outputInt =(simInt*) simCreateBuffer(2*count*sizeof(int)); // size for "Id" and "size" of memory
    p->outputChar=(simChar*)simCreateBuffer(30*count*sizeof(char)); // size for each name
    pt=p->outputChar;

    for(int i=0;i<count;i++){

        p->outputInt[i]=i;
        strcpy(pt,shm_Container->getNames(i).toLatin1().data());
        QString name=shm_Container->getNames(i);
        pt+=name.size();
        theShareMemory=shm_Container->getFromName(name);
        p->outputInt[i+count]=theShareMemory->size();
    }
}
/***************************************END SHARED MEMORY SECTION ***********************************************/






// This is the plugin start routine (called just once, just after the plugin was loaded):
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{
    // Dynamically load and bind V-REP functions:
    // 1. Figure out this plugin's directory:
    char curDirAndFile[1024];
#ifdef _WIN32
    #ifdef QT_COMPIL
        _getcwd(curDirAndFile, sizeof(curDirAndFile));
    #else
        GetModuleFileName(NULL,curDirAndFile,1023);
        PathRemoveFileSpec(curDirAndFile);
    #endif
#else
    getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif

    std::string currentDirAndPath(curDirAndFile);
    // 2. Append the V-REP library's name:
    std::string temp(currentDirAndPath);
#ifdef _WIN32
    temp+="\\v_rep.dll";
#elif defined (__linux)
    temp+="/libv_rep.so";
#elif defined (__APPLE__)
    temp+="/libv_rep.dylib";
#endif /* __linux || __APPLE__ */
    // 3. Load the V-REP library:
    vrepLib=loadVrepLibrary(temp.c_str());
    if (vrepLib==NULL)
    {
        std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'PluginSkeleton' plugin.\n";
        return(0); // Means error, V-REP will unload this plugin
    }
    if (getVrepProcAddresses(vrepLib)==0)
    {
        std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'PluginSkeleton' plugin.\n";
        unloadVrepLibrary(vrepLib);
        return(0); // Means error, V-REP will unload this plugin
    }

    // Check the version of V-REP:
    int vrepVer;
    simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
    if (vrepVer<30200) // if V-REP version is smaller than 3.02.00
    {
        std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'PluginSkeleton' plugin.\n";
        unloadVrepLibrary(vrepLib);
        return(0); // Means error, V-REP will unload this plugin
    }
    shm_Container=new SharedMemoryContainer;

    //Input Arguments:
    int inArgsNameSize[]={2,sim_lua_arg_string,sim_lua_arg_int}; // 2 input arguments: one string (The share memory name) AND one iteger (The share memory size)
    int inArgsId[]={1,sim_lua_arg_int};
    int inArgsName[]={1,sim_lua_arg_string};
    int inArgsIdData[]={2,sim_lua_arg_int,sim_lua_arg_charbuff};
    int inArgsIdTime[]={2,sim_lua_arg_int,sim_lua_arg_int};
    int inArgsNil[]={0,sim_lua_arg_nil};
    // Register Share Memory Lua commands:

    simRegisterCustomLuaFunction("simExtShareMemoryOpen","number result,number ID=simExtOpenShareMemory(string Name,int size)",inArgsNameSize,simExtShareMemoryOpen);
    simRegisterCustomLuaFunction("simExtShareMemoryClose","number result=simExtShareMemoryClose(number ID)",inArgsId,simExtShareMemoryClose);
    simRegisterCustomLuaFunction("simExtShareMemoryRead","number result,charbuff data=simExtShareMemoryRead(int ID)",inArgsId,simExtShareMemoryRead);
    simRegisterCustomLuaFunction("simExtShareMemoryGetName","number result,string name=simExtShareMemoryGetName(int ID)",inArgsId,simExtShareMemoryGetName);
    simRegisterCustomLuaFunction("simExtShareMemoryWrite","number result=simExtShareMemoryWrite(int Id,charbuff data)",inArgsIdData,simExtShareMemoryWrite);
    simRegisterCustomLuaFunction("simExtShareMemoryGetId","number result,number ID=simExtShareMemoryGetId(string Name)",inArgsName,simExtShareMemoryGetId);
    simRegisterCustomLuaFunction("simExtShareMemoryList","table ID,table names,table sizes=simExtShareMemoryList()",inArgsNil,simExtShareMemoryList);


    return(PLUGIN_VERSION); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
}

// This is the plugin end routine (called just once, when V-REP is ending, i.e. releasing this plugin):
VREP_DLLEXPORT void v_repEnd()
{
    // Here you could handle various clean-up tasks
    delete shm_Container;
    unloadVrepLibrary(vrepLib); // release the library
}

// This is the plugin messaging routine (i.e. V-REP calls this function very often, with various messages):
VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle
    // Keep following 5 lines at the beginning and unchanged:
    static bool refreshDlgFlag=true;
    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);
    void* retVal=NULL;

    // Here we can intercept many messages from V-REP (actually callbacks). Only the most important messages are listed here.
    // For a complete list of messages that you can intercept/react with, search for "sim_message_eventcallback"-type constants
    // in the V-REP user manual.

    if (message==sim_message_eventcallback_refreshdialogs)
        refreshDlgFlag=true; // V-REP dialogs were refreshed. Maybe a good idea to refresh this plugin's dialog too

    if (message==sim_message_eventcallback_menuitemselected)
    { // A custom menu bar entry was selected..
        // here you could make a plugin's main dialog visible/invisible
    }

    if (message==sim_message_eventcallback_instancepass)
    {   // This message is sent each time the scene was rendered (well, shortly after) (very often)
        // It is important to always correctly react to events in V-REP. This message is the most convenient way to do so:

        int flags=auxiliaryData[0];
        bool sceneContentChanged=((flags&(1+2+4+8+16+32+64+256))!=0); // object erased, created, model or scene loaded, und/redo called, instance switched, or object scaled since last sim_message_eventcallback_instancepass message
        bool instanceSwitched=((flags&64)!=0);

        if (instanceSwitched)
        {
            // React to an instance switch here!!
        }

        if (sceneContentChanged)
        { // we actualize plugin objects for changes in the scene

            //...

            refreshDlgFlag=true; // always a good idea to trigger a refresh of this plugin's dialog here
        }
    }

    if (message==sim_message_eventcallback_mainscriptabouttobecalled)
    { // The main script is about to be run (only called while a simulation is running (and not paused!))

    }

    if (message==sim_message_eventcallback_simulationabouttostart)
    { // Simulation is about to start

    }

    if (message==sim_message_eventcallback_simulationended)
    { // Simulation just ended

    }

    if (message==sim_message_eventcallback_moduleopen)
    { // A script called simOpenModule (by default the main script). Is only called during simulation.
        if ( (customData==NULL)||(_stricmp("PluginSkeleton",(char*)customData)==0) ) // is the command also meant for this plugin?
        {
            // we arrive here only at the beginning of a simulation
        }
    }

    if (message==sim_message_eventcallback_modulehandle)
    { // A script called simHandleModule (by default the main script). Is only called during simulation.
        if ( (customData==NULL)||(_stricmp("PluginSkeleton",(char*)customData)==0) ) // is the command also meant for this plugin?
        {
            // we arrive here only while a simulation is running
        }
    }

    if (message==sim_message_eventcallback_moduleclose)
    { // A script called simCloseModule (by default the main script). Is only called during simulation.
        if ( (customData==NULL)||(_stricmp("PluginSkeleton",(char*)customData)==0) ) // is the command also meant for this plugin?
        {
            // we arrive here only at the end of a simulation
        }
    }

    if (message==sim_message_eventcallback_instanceswitch)
    { // We switched to a different scene. Such a switch can only happen while simulation is not running

    }

    if (message==sim_message_eventcallback_broadcast)
    { // Here we have a plugin that is broadcasting data (the broadcaster will also receive this data!)

    }

    if (message==sim_message_eventcallback_scenesave)
    { // The scene is about to be saved. If required do some processing here (e.g. add custom scene data to be serialized with the scene)

    }

    // You can add many more messages to handle here

    if ((message==sim_message_eventcallback_guipass)&&refreshDlgFlag)
    { // handle refresh of the plugin's dialogs
        // ...
        refreshDlgFlag=false;
    }

    // Keep following unchanged:
    simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
    return(retVal);
}

