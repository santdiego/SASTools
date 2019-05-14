/**********************************************************************************************
Author : Ing. Diego Daniel Santiago.
diego.daniel.santiago@gmail.com
dsantiago@inaut.unsj.edu.ar
Facultad de Ingeniera
Universidad Nacional de San Juan
San Juan - Argentina

INAUT - Instituto de Automatica
http://www.inaut.unsj.edu.ar/
CONICET - Consejo Nacional de Investigaciones Científicas y Técnicas.
http://www.conicet.gov.ar/
 *NOTA de version: 
 *25/11/2014    Lanzamiento version 1
 *25/02/2015    Se agrega soporte para "unsigned int"('I'), "unsigned char"('C')
 *26/02/2015    Se agrega Soporte para bloqes con 0 INPUTS, 0 OUTPUTS o ambos
 *27/02/2015    Se corrige bug en lectura de datos tipo float
 *10/04/2015    Compilado en Linux Mint 17
 *16/06/2015	Se corrige bug en cierre de memoria compartida.
 *              compilado en Windows.  
 *06/07/2015	Se actualiza a ShareMemoryLibrary V2.0.1 para
 *              permitir uso en varias simulaciones simultaneamente.
 *              Se unificaros los tipos de datos destinados a tama�o a "unsigned long"
 *06/07/2017  	Se probaron varios tipos de tiempo de muestreo, se determino que INHERITED_SAMPLE_TIME es el mas adecuado para la aplicacion
 *08/02/2018    Parche de bugs para grandes cantidades de datos.
************************************************************************************************/


//COMPILE CODE: mex -v COMPFLAGS='$COMPFLAGS -std=c++11' matlabGenericSHM4.cpp


//#define MATLAB_MEX_FILE

#define S_FUNCTION_NAME   matlabGenericSHM4 //Cambiar este nombre por el nombre de su s-Function
#define S_FUNCTION_LEVEL 2

/*La memoria compartida de entrada figura como "nombreMemoria_entrada_#"
 *La memoria compartida de salida  figura como "nombreMemoria_salida_#" 
 * donde # es el numero de robot o DEVICE especificado en los parametros
 * y "nombreMemoria" debe ser fijada por el programador en el archivosFunctionProtoShareMemory.h
 */

#define NUM_PARAMS 10 //Numero de parametros esperados
//Posicion de los parametros en el vector de parametros:
enum Params{DEV_INPUT_SHM_NAME=0,DEV_OUTPUT_SHM_NAME,INPUTS,OUTPUTS,INPUT_TYPES,OUTPUT_TYPES,TS,TRIGG,INOFFSET,OUTOFFSET};

#include "simstruc.h"
#ifdef _WIN32
#include "..\..\ITools\ILib\ilib.h"
#else
#include "../../ITools/ILib/modules/isharedmemory.h"
#endif
#include <iostream>
using namespace std;
using namespace ilib;
static ISharedMemoryContainer shareMemoryContainer;
/*====================*
 * S-function methods *
 *====================*/
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)



static void mdlCheckParameters(SimStruct *S)
{
    //char *input_buf = mxArrayToString(ssGetSFcnParam(S,0));
    //printf("%s",input_buf );
    static size_t nameLength = (mxGetM(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME)) * mxGetN(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME))) + 1;
    if(nameLength<2||nameLength>50)
    { 
        ssSetErrorStatus(S,"The device Output shared memory name assigned is not Valid \n");
        printf("Output name length= %i",nameLength);
        return;
    }
    nameLength = (mxGetM(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME)) * mxGetN(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME))) + 1;
    
    if(nameLength<2||nameLength>50)
    { 
        ssSetErrorStatus(S,"The device Input shared memory name assigned is not Valid\n");
        printf("Input name length= %i",nameLength);
        return;
    }

    //Parametro 4 : Tipo de INPUTS
	//Verificar que coincida con el numero de INPUTS
    size_t num_input =mxGetN(ssGetSFcnParam(S,INPUTS));
    size_t num_output=mxGetN(ssGetSFcnParam(S,OUTPUTS));
    
    if(num_input!=mxGetN(ssGetSFcnParam(S,INPUT_TYPES)))
    { 
        ssSetErrorStatus(S,"The input number and input type vectors do not match \n");
        return;
    }
    //Parametro 5 : Tipo de OUTPUTS
	//Verificar que coincida con el numero de OUTPUTS
    if(num_output!=mxGetN(ssGetSFcnParam(S,OUTPUT_TYPES)))
    { 
        ssSetErrorStatus(S,"Output number vectors and output type do not match \n");
        return;
    }
//     { SHELLEXECUTEINFO ExecuteInfo;
//     memset(&ExecuteInfo, 0, sizeof(ExecuteInfo));
//     
//     ExecuteInfo.cbSize       = sizeof(ExecuteInfo);
//     ExecuteInfo.fMask        = 0;                
//     ExecuteInfo.hwnd         = 0;                
//     ExecuteInfo.lpVerb       = "open";                      // Operation to perform
//     ExecuteInfo.lpFile       = "c:\\windows\\notepad.exe";  // Application name
//     ExecuteInfo.lpParameters = "c:\\example.txt";           // Additional parameters
//     ExecuteInfo.lpDirectory  = 0;                           // Default directory
//     ExecuteInfo.nShow        = SW_SHOW;
//     ExecuteInfo.hInstApp     = 0;
//     
//     if(ShellExecuteEx(&ExecuteInfo) == FALSE)
//       // Could not start application -> call 'GetLastError()'
// 
// 	return 0;}
    
}
#endif /* MDL_CHECK_PARAMETERS */

static void mdlInitializeSizes(SimStruct *S)
{
    
    //printf("mdlInitializeSizes \n");//DEBUG
    /* See sfuntmpl_doc.c for more details on the macros below */
    size_t num_input   = mxGetN(ssGetSFcnParam(S,INPUTS));
    size_t num_output  = mxGetN(ssGetSFcnParam(S,OUTPUTS));
    char *tipoE     = mxArrayToString(ssGetSFcnParam(S,INPUT_TYPES));
    char *tipoS     = mxArrayToString(ssGetSFcnParam(S,OUTPUT_TYPES));
    
    ssSetNumSFcnParams(S, NUM_PARAMS);  /* Number of expected parameters */ 
    #if defined(MATLAB_MEX_FILE)
    if(ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S))
    {
        mdlCheckParameters(S);
        if(ssGetErrorStatus(S) != NULL)
            return;
    }
    else
        return; /* The Simulink engine reports a mismatch error. */
    #endif

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
	int trigg_enable      =	(int)(mxGetPr(ssGetSFcnParam(S,TRIGG))[0]);
    if (!ssSetNumInputPorts(S,(int_T) (num_input+trigg_enable))){
            printf("Error: ssSetNumInputPorts(S,(int_T) (num_input+trigg_enable))");
            return;  
    }
    if(trigg_enable>0){
        size_t i=num_input+trigg_enable-1;
        ssSetInputPortDataType(S, i, SS_DOUBLE);
        ssSetInputPortWidth(S,i,1);
        ssSetInputPortRequiredContiguous(S, i, true);
        ssSetInputPortDirectFeedThrough(S, i, 1);
    }
    if(num_input!=0){
        for(size_t i=0;i<num_input;i++){
            ssSetInputPortWidth(S,i,(int)mxGetPr(ssGetSFcnParam(S,INPUTS))[i]);          
            switch(tipoE[i]){ //verifico el tipo de dato
                case 'f':
                    ssSetInputPortDataType(S, i, SS_SINGLE);
                    break;
                case 'd':
                    ssSetInputPortDataType(S, i, SS_DOUBLE);
                    break;
                case 'i':
                    ssSetInputPortDataType(S, i, SS_INT32);
                    break;
                case 'c':
                    ssSetInputPortDataType(S, i, SS_INT8);
                    break;
                case 'C':
                    ssSetInputPortDataType(S, i, SS_UINT8);
                    break;
                case 'I':
                    ssSetInputPortDataType(S, i, SS_UINT32);
                    break;
                default :
                    ssSetInputPortDataType(S, i, SS_DOUBLE);
                    break;             
            }
            ssSetInputPortRequiredContiguous(S, i, true);
            ssSetInputPortDirectFeedThrough(S, i, 1);
        }
    }
	/*--------------------Fin INPUTS--------------------------------*/
	if(num_output!=0){
        if (!ssSetNumOutputPorts(S,(int_T) num_output)) return;
        for(size_t i=0;i<num_output;i++){
            ssSetOutputPortWidth(S,i,(unsigned long)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i]);
            switch(tipoS[i]){ //verifico el tipo de dato
                case 'f':
                    ssSetOutputPortDataType(S, i, SS_SINGLE);
                    break;
                case 'd':
                    ssSetOutputPortDataType(S, i, SS_DOUBLE);
                    break;
                case 'i':
                    ssSetOutputPortDataType(S, i, SS_INT32);
                    break;
                case 'c':
                    ssSetOutputPortDataType(S, i, SS_INT8);
                    break;
                case 'C':
                    ssSetOutputPortDataType(S, i, SS_UINT8);
                    break;
                case 'I':
                    ssSetOutputPortDataType(S, i, SS_UINT32);
                    break;
                default :
                    ssSetOutputPortDataType(S, i, SS_DOUBLE);
                    break;             
            }

        }
    }
	/*----------------------Fin OUTPUTS--------------------------------*/
    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
    /* Specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
    ssSetOptions(S, 0);
    mxFree(tipoE);
    mxFree(tipoS);
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    //printf("mdlInitializeSampleTimes \n"); //DEBUG
    //ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	//ssSetSampleTime(S, 0, VARIABLE_SAMPLE_TIME); //no funciona
	time_T  ts      =	(time_T )(mxGetPr(ssGetSFcnParam(S,TS))[0]);
	//int_T   offset  =	(int_T )(mxGetPr(ssGetSFcnParam(S,TS))[1]);
	ssSetSampleTime(S, 0, ts );
	ssSetOffsetTime(S, 0, 0.0);
}


 
#define MDL_INITIALIZE_CONDITIONS   /* Change to #undef to remove function */
#if defined(MDL_INITIALIZE_CONDITIONS)
  /* Function: mdlInitializeConditions ========================================
   * Abstract:
   *    In this function, you should initialize the continuous and discrete
   *    states for your S-function block.  The initial states are placed
   *    in the state vector, ssGetContStates(S) or ssGetRealDiscStates(S).
   *    You can also perform any other initialization activities that your
   *    S-function may require. Note, this routine will be called at the
   *    start of simulation and if it is present in an enabled subsystem
   *    configured to reset states, it will be call when the enabled subsystem
   *    restarts execution to reset the states.
   */
  static void mdlInitializeConditions(SimStruct *S)
  {
  }
#endif /* MDL_INITIALIZE_CONDITIONS */



#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START) 
  /* Function: mdlStart =======================================================
   * Abstract:
   *    This function is called once at start of model execution. If you
   *    have states that should be initialized once, this is the place
   *    to do it.
   */ 
  static void mdlStart(SimStruct *S)
  {
    //printf("mdlStart \n");//DEBUG
    bool ret=false;
	size_t num_input	=   mxGetN(ssGetSFcnParam(S,INPUTS));
    size_t num_output  =   mxGetN(ssGetSFcnParam(S,OUTPUTS));
    char *devOutputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME));
    char *devInputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME));
    char *tipoE     = mxArrayToString(ssGetSFcnParam(S,INPUT_TYPES));
    char *tipoS     = mxArrayToString(ssGetSFcnParam(S,OUTPUT_TYPES));
    size_t shmInputOffset=	(size_t)(mxGetPr(ssGetSFcnParam(S,INOFFSET))[0]);
    size_t shmOutputOffset =(size_t)(mxGetPr(ssGetSFcnParam(S,OUTOFFSET))[0]);
    //Determinacion del tamaño de la memoria compartida de entrada:
    //printf("shmInputOffset :%i \n",shmInputOffset);
    //printf("shmOutputOffset :%i \n",shmOutputOffset);
    
    unsigned  devInputShm_size=0;
	unsigned  devOutputShm_size=0;
	unsigned mult;
   
    if(num_input!=0){
        for(unsigned i=0;i<num_input;i++){
            switch(tipoE[i]){ //verifico el tipo de dato
                case 'f':
                    mult=sizeof(float);
                    break;
                case 'd':
                    mult=sizeof(double);
                    break;
                case 'i':
                    mult=sizeof(int);
                    break;
                case 'c':        
                    mult=sizeof(char);
                    break;
                case 'C':        
                    mult=sizeof(unsigned char);
                    break;
                case 'I':        
                    mult=sizeof(unsigned int);
                    break;
                default :
                    mult=sizeof(double);
                    break;           
            }
            devInputShm_size+=mult*ssGetInputPortWidth(S,i);
        }
    //cout<<"to atach in"<<endl;

        //Se instancia e inicializan los punteros a la clase memoria   
        ISharedMemory *InShareMemory =new ISharedMemory(devInputShm_name);
        //Se abre la memoria compartida y se coloca el objeto en el contenedor para su administracion
        
        ret=InShareMemory->attach(); //Primero intento agregarme a una existente
        if(!ret){ //la memoria no existe
            //printf("Failed to attach, attempting to create devInputShm_name: %s \n",devInputShm_name);
            ret=InShareMemory->create(int(devInputShm_size+shmInputOffset));
        }
        else{
            //printf("%s attach(): OK \n",InShareMemory->key());
            if((shmInputOffset+devInputShm_size)>InShareMemory->size()){
                ssSetErrorStatus(S,"The declared Input size plus the offset exceeds the size of the existing memory \n");
                //printf("Input size= %d \n",devInputShm_size);
                //printf("Offset size= %d \n",shmInputOffset);
                //printf("Shared Memory size= %d \n",InShareMemory->size());
                //mdlTerminate(S);
                return;
            }
        }
        if(ret==true){
           InShareMemory->setOffset(int(shmInputOffset));
           shareMemoryContainer.insert(InShareMemory);
        }else{
            mexPrintf("ERROR %i: %s create()  \n",InShareMemory->error(),InShareMemory->key());
            ssSetErrorStatus(S,"ERROR InShareMemory->create(devInputShm_size) \n");
            //printf("Detail: %s \n",InShareMemory->error());
            //mdlTerminate(S);
            return;
        }
    }
    if(num_output!=0){
        for(int i=0;i<num_output;i++){
            switch(tipoS[i]){ //verifico el tipo de dato
                case 'f':
                    mult=sizeof(float);
                    break;
                case 'd':
                    mult=sizeof(double);
                    break;
                case 'i':
                    mult=sizeof(int);
                    break;
                case 'c':        
                    mult=sizeof(char);
                    break;
                case 'C':        
                    mult=sizeof( unsigned char);
                    break;
                case 'I':        
                    mult=sizeof( unsigned int);
                    break;
                default :
                    mult=sizeof(double);
                    break;           
            }
            devOutputShm_size+=mult*ssGetOutputPortWidth(S,i);
        }
     // cout<<"to create out"<<endl;
      
        ISharedMemory *OutShareMemory=new ISharedMemory(devOutputShm_name);
        
        ret=OutShareMemory->attach();
        if(!ret){
            ret=OutShareMemory->create(int(devOutputShm_size+shmOutputOffset));
        }
        else{
            //printf("%s attach(): OK \n", OutShareMemory->key());
            if((devOutputShm_size+shmOutputOffset)>OutShareMemory->size()){
                ssSetErrorStatus(S,"The declared Output size plus the offset exceeds the size of the existing memory \n");
                //printf("Output size= %d \n",devOutputShm_size);
                //printf("Offset size= %d \n",shmOutputOffset);
                //printf("Shared Memory size= %d \n",OutShareMemory->size());
                //mdlTerminate(S);
                return;
            }
        }
        if(ret==true){
            OutShareMemory->setOffset(int(shmOutputOffset));
            shareMemoryContainer.insert(OutShareMemory);
        }else{
            mexPrintf("%s create()\n",OutShareMemory->errorString().c_str());
            ssSetErrorStatus(S,"ERROR OutShareMemory->create()\n");
            //mdlTerminate(S);
            return;
        }
    }
    mxFree(devInputShm_name);
    mxFree(devOutputShm_name);
    mxFree(tipoE);
    mxFree(tipoS);    
  }
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. 
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
	
    //printf("mdlOutputs \n");//DEBUG
    size_t  num_output  = mxGetN(ssGetSFcnParam(S,OUTPUTS)); 
    //int shmInputOffset=	(int)(mxGetPr(ssGetSFcnParam(S,INOFFSET))[0]);
    int shmOutputOffset =(int)(mxGetPr(ssGetSFcnParam(S,OUTOFFSET))[0]);
    if(num_output>0){
        char    *devOutputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME));
        char    *tipo	= mxArrayToString(ssGetSFcnParam(S,OUTPUT_TYPES));
        char    *pDeviceOutput;  
        int     aux=0;
        unsigned long _size=0;
        //printf("shareMemoryContainer.getCount(): %i\n",shareMemoryContainer.getCount());
        ISharedMemory *theMemory = shareMemoryContainer.getFromName(devOutputShm_name);
        size_t devOutputShm_size = theMemory->size(); //LINEA DE PRUEBA
        //printf("devOutputShm_size : %i \n",devOutputShm_size); //DEBUG
        //Lectura de memoria compartida
        if (theMemory!=NULL){
            theMemory->lock();
            pDeviceOutput=(char*)theMemory->data();
            //Verificaciones
            //printf("theMemory->size(): %i \n",theMemory->size()); 
            //printf("tipo: %s \n",tipo);
            //printf("num_output: %i \n",num_output);
            //printf("shmOutputOffset: %i \n",shmOutputOffset);
            //printf("devOutputShm_name memOffset: %i \n",theMemory->getOffset());
            
            //Saque los datos del bloque
            for(unsigned i=0;i<num_output;i++){
                switch(tipo[i]){ //verifico el tipo de dato
                    case 'f'://float
                        _size=ssGetOutputPortWidth(S,i)*sizeof(float);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;
                    case 'd'://double
                        _size=ssGetOutputPortWidth(S,i)*sizeof(double);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;
                    case 'i'://int32
                        _size=ssGetOutputPortWidth(S,i)*sizeof(int);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;
                    case 'c'://char
                        _size=ssGetOutputPortWidth(S,i)*sizeof(char);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;
                   case 'C'://unsigned char
                        _size=ssGetOutputPortWidth(S,i)*sizeof(unsigned char);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;
                   case 'I'://unsigned int
                        _size=ssGetOutputPortWidth(S,i)*sizeof(unsigned int);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;
                    default : //double
                        _size=ssGetOutputPortWidth(S,i)*sizeof(double);                       
                        memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                        aux+=_size;
                        break;       
                }
            }
            theMemory->unlock();
        }else{
            ssSetErrorStatus(S,"shareMemoryContainer.getFromName(devInputShm_name) : MEMORY NOT FOUND \n");
        }   
        mxFree(tipo);
        mxFree(devOutputShm_name);
    }   
    
}


#define MDL_UPDATE  /* Change to #undef to remove function */
#if defined(MDL_UPDATE)
  /* Function: mdlUpdate ======================================================
   * Abstract:
   *    This function is called once for every major integration time step.
   *    Discrete states are typically updated here, but this function is useful
   *    for performing any tasks that should only take place once per
   *    integration step.
   */
  static void mdlUpdate(SimStruct *S, int_T tid)
  {
      //printf("mdlUpdate \n");//DEBUG
      size_t num_input = mxGetN(ssGetSFcnParam(S,INPUTS));
      unsigned trigg_enable      =	(unsigned)(mxGetPr(ssGetSFcnParam(S,TRIGG))[0]);
      size_t shmInputOffset=	size_t(mxGetPr(ssGetSFcnParam(S,INOFFSET))[0]);
      const void *u;
      //verifico estado del trigger
      if(trigg_enable){
          size_t i=num_input+trigg_enable-1;
          u=ssGetInputPortSignal(S,i);
          double *trigg=(double*)u;
          if(*trigg<=0)
              return;
      }
      //Ingreso de los datos para enviar al DEVICE
        if(num_input!=0){
            char	*pDeviceInput;
            char	*tipo = mxArrayToString(ssGetSFcnParam(S,INPUT_TYPES));
            size_t  	 aux = 0;
            char *devInputShm_name= mxArrayToString(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME));
            
            ISharedMemory	*theMemory = shareMemoryContainer.getFromName(devInputShm_name);
            //printf("devInputShm_name memOffset: %i \n",theMemory->getOffset());
            if(theMemory!=NULL){
                theMemory->lock();
                pDeviceInput=(char*)theMemory->data();
                theMemory->unlock();
            }else{
                ssSetErrorStatus(S," *theMemory=NULL \n");
                return;
            }
            size_t devInputShm_size = theMemory->size();
            size_t _size=0;
            for(size_t i=0;i<num_input;i++){
                if ( !ssGetInputPortRequiredContiguous(S,i) ) {
                    ssSetErrorStatus(S,"Matlab was not able to allocate memory in a contiguous area (ssGetInputPortRequiredContiguous)\n");
                    break;
                }
                switch(tipo[i]){ //verifico el tipo de dato
                    case 'f'://float
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(float);
//                         printf("[%i] Tama�o en char a ser copiado: %lu \n",i,_size); //DEBUG
//                         printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;
                    case 'd'://double
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(double);
                        //printf("[%i] Tama�o en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;
                    case 'i'://int32
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(int);
                        //printf("[%i] Tama�o en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;
                    case 'c'://char
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(char);
                        //printf("[%i] Tama�o en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;
                    case 'C'://unsigned char
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(unsigned char);
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;
                    case 'I'://unsigned int
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(unsigned int);
                        //printf("[%i] Tama�o en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;
                         
                    default : //double
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(double);
                        //printf("[%i] Tama�o en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        if(aux+_size<=devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        }else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        aux +=_size;
                        break;  
                }          
            }
            mxFree(tipo);
            mxFree(devInputShm_name);
        }            
  }
#endif /* MDL_UPDATE */



#define MDL_DERIVATIVES  /* Change to #undef to remove function */
#if defined(MDL_DERIVATIVES)
  /* Function: mdlDerivatives =================================================
   * Abstract:
   *    In this function, you compute the S-function block's derivatives.
   *    The derivatives are placed in the derivative vector, ssGetdX(S).
   */
  static void mdlDerivatives(SimStruct *S)
  {
  }
#endif /* MDL_DERIVATIVES */


/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{   
    //mexPrintf("mdlTerminate \n");//DEBUG
    int shmInputOffset=	(int)(mxGetPr(ssGetSFcnParam(S,INOFFSET))[0]);
    int shmOutputOffset =(int)(mxGetPr(ssGetSFcnParam(S,OUTOFFSET))[0]);
    char *devInputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME));
    char *devOutputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME));
    
    //mexPrintf("mdlTerminate %s \n",devInputShm_name);
    //mexPrintf("mdlTerminate %s \n",devOutputShm_name);
    
    shareMemoryContainer.removeFromName(devInputShm_name);
    shareMemoryContainer.removeFromName(devOutputShm_name);
    if(shareMemoryContainer.getCount()!=0)
        shareMemoryContainer.removeAll();
    mxFree(devInputShm_name);
    mxFree(devOutputShm_name);
}


/*======================================================*
 * See sfuntmpl_doc.c for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
