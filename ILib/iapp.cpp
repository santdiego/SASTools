#include "iapp.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <istream>
#include <thread>
using namespace std;

IApp::IApp(){
    memset(&_ctrInfo,0,sizeof(appControl));
    _createOk=false;
    _configLoaded=false;
    //    DEBUG cout<<"Constructor base IApp()"<<endl;
}

std::string IApp::getInputName(){
    return std::string(_ctrInfo.shmInputName);
}
std::string IApp::getOutputName(){
    return std::string(_ctrInfo.shmOutputName);
}
std::string IApp::getControlName(){
    return std::string(_ctrInfo.shmControlName);
}

IApp::~IApp(){

    if(vecTimer.size()>0){
        for(unsigned i=0;i<vecTimer.size();i++){
            vecTimer[i]->stop();
            delete vecTimer[i];
        }
    }
    detach();
    //   DEBUG cout<<"Destructor base IApp::~IApp()"<<endl;
}
bool IApp::isLoaded(){
    return _createOk;
}

int IApp::sizeInput(){
    return shmInput.size();
}
int IApp::sizeOutput(){
    return shmOutput.size();
}

bool IApp::lockInput(){

    return shmInput.lock();

}
bool IApp::lockOutput(){

    return shmOutput.lock();

}
bool IApp::lockControl(){
    return shmControl.lock();

}
bool IApp::unlockInput(){

    return shmInput.unlock();


}
bool IApp::unlockOutput(){

    return shmOutput.unlock();

}
bool IApp::unlockControl(){
    return shmControl.unlock();

}

appControl* IApp::ctrData(){

    return static_cast<appControl*>(shmControl.data());
    //memcpy(&_ctrInfo,controData,sizeof(appControl));

}

void* IApp::inputData(){

    return shmInput.data();
}

void* IApp::outputData(){
    return shmOutput.data();
}

string IApp::getError(){
    return _error;
}

bool IApp::setInput(const char *inputName, int inputSize){
    shmInput.setKey(inputName);
    if (!shmInput.create(inputSize+_ctrInfo.shmInputOffset)){
        _error=shmInput.errorString();
        cerr<<"ERROR: "<<_error<<"IN: IApp::shmInput(const char* outputName, int outputSize)"<<endl;
        if(!shmInput.attach()){
            cerr<<"ERROR: "<<_error<<"IN: IApp::setOutput(const char* outputName, int outputSize)"<<endl;
            _error=shmOutput.errorString();
            return false;
        }
    }
    return true;
}

bool IApp::setOutput(const char* outputName, int outputSize){
    shmOutput.setKey(outputName);
    if (!shmOutput.create(outputSize+_ctrInfo.shmOutputOffset)){
        _error=shmOutput.errorString();
        cerr<<"ERROR: "<<_error<<"IN: IApp::setOutput(const char* outputName, int outputSize)"<<endl;
        if(!shmOutput.attach()){
            cerr<<"ERROR: "<<_error<<"IN: IApp::setOutput(const char* outputName, int outputSize)"<<endl;
            _error=shmOutput.errorString();
            return false;
        }
    }
    return true;
}

bool IApp::create(){
   // cout<<"create()"<<endl;

    _createOk=false;
    if(!_configLoaded){
        _error="create(): Config Page Not Loaded";
        return false;
    }

    //    cout<<"IApp::create(appControl& data)"<<data.shmControlName<<endl;
    shmControl.setKey(_ctrInfo.shmControlName);
    if (shmControl.create(static_cast<int>(sizeof(appControl))+_ctrInfo.shmControlOffset)){
        if(shmControl.lock()){
            memcpy(shmControl.data(),&_ctrInfo,sizeof(appControl));
            shmControl.unlock();
            _createOk=true;
            setState(st_LOAD);
        }else{
            _createOk=false;
            cerr<<"Memory Could not be locked"<<endl;
        }
    }
    else{
        _error=shmControl.errorString(); //Could not create or already exist
        cerr<<"ERROR: '"<<_error<<"' IN: IApp::create(appControl& data)"<<endl;
        if(shmControl.attach()){
            if(!shmControl.lock())
                return false;
            memcpy(&_ctrInfo,shmControl.data(),sizeof(_ctrInfo)); //Si la memoria existe cargo su contenido
            shmControl.unlock();
            _createOk=true;
        }
        else{
            _createOk=false;
            _error=shmControl.errorString();
            cerr<<"ERROR: "<<_error<<" IN: IApp::create(appControl& data)"<<endl;
        }
    }
    if(_createOk){
        if(_ctrInfo.shmInputSize>0){
            if(!setInput(_ctrInfo.shmInputName,_ctrInfo.shmInputSize)){
                _createOk=false;
                cerr<<"ERROR: "<<_error<<" IN: IApp::create(appControl& data)"<<endl;
            }
        }
        if(_ctrInfo.shmOutputSize>0){
            if(!setOutput(_ctrInfo.shmOutputName,_ctrInfo.shmOutputSize)){
                _createOk=false;
                cerr<<"ERROR: "<<_error<<" IN: IApp::create(appControl& data)"<<endl;
            }
        }
    }

    if(_createOk)
        setState(st_LOAD);
    shmControl.setOffset(_ctrInfo.shmControlOffset);
    shmInput.setOffset(_ctrInfo.shmInputOffset);
    shmOutput.setOffset(_ctrInfo.shmOutputOffset);
    return _createOk;
}


State IApp::getState() {
    State tmp;
    if(lockControl()){
        tmp=ctrData()->appState;
        unlockControl();
        return tmp;
    }
    return st_ERROR;
}

void IApp::setState( State st){
    if(lockControl()){
        if(shmControl.isAttached()){
            ctrData()->appState = st;
        }
        unlockControl();
    }
}

bool IApp::attach(const string &ctrName){
    //cout<<"IApp::attach"<<endl;

    appControl *data;
    _createOk=false;
    //    cout<<"IApp::create(appControl& data)"<<data.shmControlName<<endl;
    shmControl.setKey(ctrName.c_str());
    shmControl.setSize(sizeof(appControl));
    bool res=shmControl.attach();
    if (res){
        _createOk=true;
        _configLoaded=true;
    }
    else{
        _createOk=false;
        _error=shmControl.errorString();
        //cerr<<"ERROR: Could not load Memory "<<ctrName<<". Memory was not created"<<endl;
        //cerr<<"ERROR detail:"<<_error<<" IN: IApp::create(appControl& data)"<<endl;
    }
    if(_createOk){
        shmControl.lock();
        data=ctrData();
        if(data->shmInputSize>0){
            if(!setInput(data->shmInputName,data->shmInputSize)){
                _createOk=false;
                cerr<<"ERROR: "<<_error<<" IN: IApp::create(appControl& data)"<<endl;
            }
        }
        if(data->shmOutputSize>0){
            if(!setOutput(data->shmOutputName,data->shmOutputSize)){
                _createOk=false;
                cerr<<"ERROR: "<<_error<<" IN: IApp::create(appControl& data)"<<endl;
            }
        }
        memcpy(&_ctrInfo,data,sizeof(appControl));
        shmControl.unlock();
    }
    shmControl.setOffset(_ctrInfo.shmControlOffset);
    shmInput.setOffset(_ctrInfo.shmInputOffset);
    shmOutput.setOffset(_ctrInfo.shmOutputOffset);
    return _createOk;
}

bool IApp::loadConfig(const appControl &conf){

    memcpy(&_ctrInfo,&conf,sizeof(appControl));
    _configLoaded=true;

    return true;

}

bool IApp::loadConfigFromFile(const string &fileName){
    //load File
    string line;
    ifstream file(fileName);
    if(!file.is_open()){
        //cerr << "ERROR: File not found" << endl;
        return false;
    }
    while(getline(file,line)){
        str2conf(_ctrInfo,line);
    }
    _configLoaded=true;
    return true;
}
void IApp::str2conf(appControl &conf,const string &line){
    //cout<<line<<endl;
    istringstream iss(line);
    string aux_s;
    int aux_i;
    iss>>aux_s;
    if(aux_s=="appState"){
        iss>>aux_s;
        if(aux_s=="st_NOTLOADED"){
            conf.appState=st_NOTLOADED;
        }
        else if(aux_s=="st_LOAD"){
            conf.appState=st_LOAD;
        }
        else if(aux_s=="st_START"){
            conf.appState=st_START;
        }
        else if(aux_s=="st_PAUSE"){
            conf.appState=st_PAUSE;
        }
        else if(aux_s=="st_STOP"){
            conf.appState=st_STOP;
        }
        else if(aux_s=="st_CLOSE"){
            conf.appState=st_CLOSE;
        }
    }
    else if(aux_s=="period_us"){
        iss>>aux_i;
        conf.period_us=aux_i;
    }
    else if(aux_s=="shmControlName"){
        iss>>aux_s;
        if(aux_s.length()>MINLENGTH)
            strcpy(conf.shmControlName,aux_s.c_str());
    }
    else if(aux_s=="shmInputName"){
        iss>>aux_s;
        if(aux_s.length()>MINLENGTH)
            strcpy(conf.shmInputName,aux_s.c_str());
    }
    else if(aux_s=="shmOutputName"){
        iss>>aux_s;
        if(aux_s.length()>MINLENGTH)
            strcpy(conf.shmOutputName,aux_s.c_str());
    }
    else if(aux_s=="shmControlOffset"){
        iss>>aux_i;
        if(aux_i>0)
            conf.shmControlOffset=aux_i;
    }
    else if(aux_s=="shmInputOffset"){
        iss>>aux_i;
        if(aux_i>0)
            conf.shmInputOffset=aux_i;
    }
    else if(aux_s=="shmOutputOffset"){
        iss>>aux_i;
        if(aux_i>0)
            conf.shmOutputOffset=aux_i;
    }
    else if(aux_s=="shmInputSize"){
        iss>>aux_i;
        conf.shmInputSize=aux_i;
    }
    else if(aux_s=="shmOutputSize"){
        iss>>aux_i;
        conf.shmOutputSize=aux_i;
    }
    else if(aux_s=="error"){
        iss>>aux_i;
        conf.error=aux_i;
    }
    else if(aux_s=="nParams"){
        iss>>aux_i;
        conf.nParams=aux_i;
    }
    else if(aux_s=="-P"){
        getline(iss,aux_s); //extraer el resto del string
        aux_s+="\n";
        strcat(conf.Params,aux_s.c_str());
    }
}

bool IApp::detach(){
    bool a=false,b=false,c=false;
    if(shmInput.isAttached())
        a=shmInput.detach();
    if(shmOutput.isAttached())
        b=shmOutput.detach();
    if(shmControl.isAttached())
        c=shmControl.detach();
    if(a&b&c)
        return true;
    return false;
}


int	IApp::createTask(std::function<void(void*)> _fcall, void * _inarg){
    int id=static_cast<int>(vecTimer.size());
    ITimer *_newTimer=new ITimer();
    _newTimer->onTimeout(_fcall,_inarg);
    vecTimer.push_back(_newTimer);
    return id;
}

void IApp::startTask(int id,int _period_us){
    if(id>=0){
        if(_period_us>0)
            vecTimer[id]->setIntervalus(_period_us);
        else
            vecTimer[id]->setSingleShot(true);
        vecTimer[id]->start();
    }
    else{
        for (unsigned i=0;i<vecTimer.size();i++){
            if(_period_us>0)
                vecTimer[i]->setIntervalus(_period_us);
            else
                vecTimer[i]->setSingleShot(true);
            vecTimer[i]->start();
        }
    }
}

void IApp::stopTask(int id){
    if(id>=0)
        vecTimer[id]->stop();
    else{
        for (unsigned i=0;i<vecTimer.size();i++)
            vecTimer[i]->stop();
    }
}
bool IApp::isTaskActive(int id){
    if(id>=0){
        return vecTimer[id]->isActive();
    }
    else{
        for(unsigned i=0;i<vecTimer.size();i++){
            if(vecTimer[i]->isActive()==true)
                return true;
        }
    }
    return false;
}

void IApp::exec(){
    bool play=false;
    State st;
    milliseconds intervalPeriodMillis(1);
    while(1){
        st=getState();
        switch(st){
        case st_START:
            if (play==false){
                cout<<"START Command"<<endl;
                startTask();//task 1 se ejecuta cada 100ms
                play=true;
            }
            break;
        case st_STOP:
            if(play==true){
                stopTask();
                play =false;
                cout<<"STOP Command"<<endl;
            }
            break;
        case st_CLOSE:
            cout<<"CLOSE Command"<<endl;
            cout<<"closing.."<<endl;
            return;
        default:
            break;
        }
        std::this_thread::sleep_for(intervalPeriodMillis);
    }
}
int IApp::period(){
    int period;
    lockControl();
    period=ctrData()->period_us;
    unlockControl();
    return period;
}
