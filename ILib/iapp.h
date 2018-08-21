#ifndef IAPP_H
#define IAPP_H
#include "isharedmemory.h"
#include "itimer.h"
#include <string>
#include <vector>


/* Clase base de una IApp. Pensada para ser utilizada mediante herencia
 *
*/

#define DEBUG //

//seridor: esta app
//cliente: quien ejecuta la app
enum State{st_NOTLOADED,st_LOAD,st_START,st_PAUSE,st_STOP,st_CLOSE,st_ERROR};
//st_NOTLOADED : La aplicacion aun no se inicializa
//st_LOAD :App en proceso de carga
//st_START : App iniciando un ciclo de trabajo
//st_UPDATE :Actualizacion de los estados del dispositivo (entrada de datos)
//st_OUTPUT :Actualizacion de salida del dispositivo (salida de datos)
//st_STOP   : App finalizo ciclo de trabajo
//st_CLOSE : La app fue cerrada

enum Error{OUTPUT_SIZE_ERROR,INPUT_SIZE_ERROR, NON_LISTED, NON_SUPPORTED, MEMORY_ALREADY_EXISTS, MEMORY_COULD_NOT_ACCESS, NOT_LOADED};


#define MAXUSER_PARAMS 400
#pragma pack(push)
#pragma pack(1)
struct appControl{
    appControl(){
        memset(this,0,sizeof(appControl));
    }
public:
    State appState; //descripcion del estado del programa (Servidor)
    int period_us; //para usar con timer periodico

    char shmControlName[MAXLENGTH];
    char shmInputName[MAXLENGTH]; //nombre de la memoria compartida que contiene los datos de entrada del dispositivo fisico (Cliente )
    char shmOutputName[MAXLENGTH]; //nombre de la memoria compartida que contiene los datos de salida del dispositivo fisico (Cliente )

    int shmControlOffset; //dereccion a partir de la obtenida en shmControlName en donde se encuentran los datos
    int shmInputOffset;  //dereccion a partir de la obtenida en shmInputName en donde se encuentran los datos
    int shmOutputOffset; //dereccion a partir de la obtenida en shmOutputName en donde se encuentran los datos

    int shmInputSize;  //Tamaño de datos requeridos en en shmInput (Cliente)
    int shmOutputSize;  //Tamaño de datos requeridos en shmOutpu (Cliente)

    int error;//descripcion del error (Servidor)
    int nParams; //cantidad de parametros enviados a la memoria compartida
    char Params[MAXUSER_PARAMS]; //Espacio de relleno que puede contener parametros adicionales enviados al programa
};
#pragma pack(pop)

class IApp
{
public:
    IApp();
    virtual ~IApp();
    //Las funciones virtuales deben ser obligatoriamente definidas por quien herede de la clase.
    bool attach(const std::string &ctrName);
    bool loadConfig(const appControl & conf);
    bool loadConfigFromFile(const std::string &fileName);
    bool create();
    bool detach();
    int	createTask(std::function<void(void*)> _fcall, void * _inarg=nullptr);
    void stopTask(int id=-1);
    void startTask(int id=-1,int _period_us=0);
    bool isTaskActive(int id);
    virtual void exec();
    appControl *ctrData(); //para escribir o leer los datos de control
    void *inputData();  //Para escribir o leer los datos de entrada al dispositivo
    void *outputData(); //para leer los datos de salida del dispositivo.
    std::string getError();
    std::string getInputName();
    std::string getOutputName();
    std::string getControlName();
    State getState();
    void setState(State );
    bool lockInput();
    bool lockOutput();
    bool lockControl();
    bool unlockInput();
    bool unlockOutput();
    bool unlockControl();
    bool isLoaded();
    int sizeInput();
    int sizeOutput();
    int period();

protected:
    ISharedMemory shmInput;
    ISharedMemory shmOutput;
    ISharedMemory shmControl;
    appControl _ctrInfo;
    std::string _error;
    bool setInput(const char *inputName, int inputSize);
    bool setOutput(const char *outputName, int outputSize);
    void str2conf(appControl &conf,const std::string &line);
    bool _createOk;
    bool _configLoaded;
    std::vector<ITimer*> vecTimer;
};

#endif // IAPP_H
