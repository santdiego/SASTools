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
 *01/09/2015    Lanzamiento version 1

************************************************************************************************/

#define S_FUNCTION_NAME   simpleSHM //Cambiar este nombre por el nombre de su s-Function
#define S_FUNCTION_LEVEL 2

/*La memoria compartida de entrada figura como "nombreMemoria_entrada_#"
 *La memoria compartida de salida  figura como "nombreMemoria_salida_#" 
 * donde # es el numero de robot o dispositivo especificado en los parametros
 * y "nombreMemoria" debe ser fijada por el programador en el archivosFunctionProtoShareMemory.h
 */

#define NUM_PARAM       6 //Numero de parametros esperados
//Posicion de los parametros en el vector de parametros:
#define NOMBRE          0
#define DISPOSITIVO     1
#define ENTRADAS        2
#define SALIDAS         3
#define TIPO_ENTRADAS   4
#define TIPO_SALIDAS    5

#include "simstruc.h"
#include "matrix.h"
#include "CshareMemoryContainer.cpp"
#include "CshareMemory.cpp"

static CShareMemoryContainer simpleShareMemoryContainer;

/*====================*
 * S-function methods *
 *====================*/
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)

static void mdlCheckParameters(SimStruct *S)
{
    //printf("mdlCheckParameters \n");
    /*Parametros:
     *-Nombre base de la memoria
     *-Numero de dispositivo
     *-Entradas y dimensiones
     *-Salidas y dimensiones
     *-Tipo de Entradas
     *-Tipo de Salidas
     */
    /* get the length of the input string */
    
    //char *input_buf = mxArrayToString(ssGetSFcnParam(S,0));
    //printf("%s",input_buf );
    
	//Parametro 1 : nombre base de la memoria: 
	//verificar que no se exceda el maximo y que no este vacio
    static size_t nameLength = (mxGetM(ssGetSFcnParam(S,NOMBRE)) * mxGetN(ssGetSFcnParam(S,NOMBRE))) + 1;
    
    if(nameLength<2||nameLength>50)
    { 
        ssSetErrorStatus(S,"El nombre asignado a la memoria no es Valido \n");
        return;
    }
    //Parametro 4 : Tipo de Entradas
	//Verificar que coincida con el numero de entradas
    size_t num_input =mxGetN(ssGetSFcnParam(S,ENTRADAS));
    size_t num_output=mxGetN(ssGetSFcnParam(S,SALIDAS));
    
    //Verifica si se ingreso entradas y salidas o ambos 
    if(num_input!=0){
        if (num_output!=0){
            ssSetErrorStatus(S,"Especifique entradas o salidas pero no ambos!\n");
            return;
        }
    }else if (num_output==0){
        ssSetErrorStatus(S,"Especifique por lo menos una Entrada / Salida \n");
        return;
    }  
            
    if(num_input!=mxGetN(ssGetSFcnParam(S,TIPO_ENTRADAS)))
    { 
        ssSetErrorStatus(S,"Los vectores de numero de entrada y tipo de entrada no coinciden \n");
        return;
    }
    //Parametro 5 : Tipo de Salidas
	//Verificar que coincida con el numero de salidas
    if(num_output!=mxGetN(ssGetSFcnParam(S,TIPO_SALIDAS)))
    { 
        ssSetErrorStatus(S,"Los vectores de numero de salida y tipo de salida no coinciden \n");
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
    /* See sfuntmpl_doc.c for more details on the macros below */
    size_t num_input   = mxGetN(ssGetSFcnParam(S,ENTRADAS));
    size_t num_output  = mxGetN(ssGetSFcnParam(S,SALIDAS));
    char *tipoE     = mxArrayToString(ssGetSFcnParam(S,TIPO_ENTRADAS));
    char *tipoS     = mxArrayToString(ssGetSFcnParam(S,TIPO_SALIDAS));
    
    ssSetNumSFcnParams(S, NUM_PARAM);  /* Number of expected parameters */ 
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
	
    if(num_input!=0){
        if (!ssSetNumInputPorts(S,(int_T) num_input)) return;  
        for(int i=0;i<num_input;i++){
            ssSetInputPortWidth(S,i,(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i]);          
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

	/*--------------------Fin Entradas--------------------------------*/
	if(num_output!=0){
        if (!ssSetNumOutputPorts(S,(int_T) num_output)) return;
        for(int i=0;i<num_output;i++){
            ssSetOutputPortWidth(S,i,(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i]);
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
	/*----------------------Fin Salidas--------------------------------*/

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
    printf("mdlInitializeSampleTimes \n");
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
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
    int device      =	(int)(mxGetPr(ssGetSFcnParam(S,DISPOSITIVO))[0]) - 1;
	size_t num_input	=   mxGetN(ssGetSFcnParam(S,ENTRADAS));
    size_t num_output  =   mxGetN(ssGetSFcnParam(S,SALIDAS));
    
	char *stringMem = new char[50];
    char *baseName	= mxArrayToString(ssGetSFcnParam(S,NOMBRE));
    char *tipoE     = mxArrayToString(ssGetSFcnParam(S,TIPO_ENTRADAS));
    char *tipoS     = mxArrayToString(ssGetSFcnParam(S,TIPO_SALIDAS));
    
    sprintf(stringMem, "%s_%i",baseName,device+1);
    //printf("%s \n",stringMem); //DEBUG LINES: Utilice estas lineas para verificar el nombre de la memoria compartida
    //printf("%s \n",stringMem);

    //Determinacion del tamano de la memoria compartida de entrada:
    size_t  size_mem=0;
	int mult;
    
    if(num_input!=0){
        for(int i=0;i<num_input;i++){
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
            size_mem+=mult*(int)(mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i]);
        }
        //Se instancia e inicializan los punteros a la clase memoria   
        CshareMemory *InShareMemory =new CshareMemory(stringMem ,size_mem);
        //Se abre la memoria compartida y se coloca el objeto en el contenedor para su administracion
        if(InShareMemory->openShareMemory()==true){
            if(simpleShareMemoryContainer.insert(InShareMemory,2*device)!=true) //Asigna la Id de contenedor a la memoria :2*numero de memoria
                ssSetErrorStatus(S,"ERROR ID duplicada");
        }else{
            ssSetErrorStatus(S,"ERROR InShareMemory->openShareMemory() \n");
        }
    }
    
    else if(num_output!=0){
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
            size_mem+=mult*(int)(mxGetPr(ssGetSFcnParam(S,SALIDAS))[i]);
        }
        //Se instancia e inicializan los punteros a la clase memoria 
        //Memoria para sacar datos (salida del al bloque, lectura de memoria)
        CshareMemory *OutShareMemory=new CshareMemory(stringMem,size_mem);
        if(OutShareMemory->openShareMemory()==true){
            if(simpleShareMemoryContainer.insert(OutShareMemory,2*device)!=true) //Asigna la Id de contenedor a la memoria :2*numero de memoria	+1 
                ssSetErrorStatus(S,"ERROR ID duplicada");
        }else{
            ssSetErrorStatus(S,"ERROR OutShareMemory->openShareMemory()\n");
        }
    }

    //printf("Cantidad de memorias en contenedor: %d \n ",simpleShareMemoryContainer.getCount());

	delete stringMem;
    
    mxFree(baseName);
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
    size_t  num_output  = mxGetN(ssGetSFcnParam(S,SALIDAS)); 

    if(num_output>0){
        int     device  = (int)(mxGetPr(ssGetSFcnParam(S,DISPOSITIVO))[0]) - 1;
        char    *tipo	= mxArrayToString(ssGetSFcnParam(S,TIPO_SALIDAS));
        char    *pDeviceOutput;  
        int     aux=0;

        CshareMemory *theMemory = simpleShareMemoryContainer.getFromID(2*device);
        size_t size_mem = theMemory->getSize(); //LINEA DE PRUEBA
        //Lectura de memoria compartida
        if (theMemory!=NULL){
            pDeviceOutput = new char[size_mem]; //LINEA DE PRUEBA
            if(theMemory->readShareMemory(pDeviceOutput)!=true)
                ssSetErrorStatus(S,"ERROR readShareMemory \n");
        }else{
            //printf("Dispositivo no encontrado: %d\n ",2*device);
            //printf("Cantidad de memorias en contenedor: %d\n ",simpleShareMemoryContainer.getCount());
            ssSetErrorStatus(S,"simpleShareMemoryContainer.getFromID(2*device) : MEMORY NOT FOUND \n");
        }   
        //Saque los datos del bloque
        for(int i=0;i<num_output;i++){
            switch(tipo[i]){ //verifico el tipo de dato
                case 'f'://float
                    memcpy( (float*)ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i]*sizeof(float)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i]*sizeof(float);
                    break;
                case 'd'://double
                    memcpy( (double*)ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i]*sizeof(double)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i]*sizeof(double);
                    break;
                case 'i'://int32
                    memcpy( (int*)ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(int)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(int);
                    break;
                case 'c'://char
                    memcpy( (char*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(char)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(char);
                    break;
               case 'C'://unsigned char
                    memcpy( (unsigned char*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(unsigned char)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(unsigned char);
                    break;
               case 'I'://unsigned int
                    memcpy( (unsigned int*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(unsigned int)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(unsigned int);
                    break;
                default : //double
                    memcpy((double*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(double)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,SALIDAS))[i] * sizeof(double);
                    break;          
            }
        }
        delete pDeviceOutput;
        mxFree(tipo);
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
      size_t num_input = mxGetN(ssGetSFcnParam(S,ENTRADAS));
        //Ingreso de los datos para enviar al dispositivo
        if(num_input!=0){
            char	*pDeviceInput;	
            char	*tipo = mxArrayToString(ssGetSFcnParam(S,TIPO_ENTRADAS));
            size_t    aux = 0;
            int     device = (int)(mxGetPr(ssGetSFcnParam(S,DISPOSITIVO))[0]) - 1;
            const void *u;
            CshareMemory	*theMemory = simpleShareMemoryContainer.getFromID(2*device);
            size_t size_mem = theMemory->getSize();
            pDeviceInput= new char[size_mem];
            for(int i=0;i<num_input;i++){
                switch(tipo[0]){ //verifico el tipo de dato
                    case 'f'://float
                        u = (const float*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(float));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(float);
                        break;
                    case 'd'://double
                        u = (const double*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(double));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(double);
                        break;
                    case 'i'://int32
                        u = (const int*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(int));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(int);
                        break;
                    case 'c'://char
                        u = (const char*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(char));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(char);
                        break;
                    case 'C'://unsigned char
                        u = (const unsigned  char*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(unsigned char));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(unsigned char);
                        break;
                    case 'I'://unsigned int
                        u = (const unsigned  int*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(unsigned int));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(unsigned int);
                        break;
                    default : //double
                        u = (const double*) ssGetInputPortSignal(S,i);
                        memcpy((pDeviceInput+aux),u, (int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(double));
                        aux+=(int)mxGetPr(ssGetSFcnParam(S,ENTRADAS))[i] * sizeof(double);
                        break;          
                }          
            }
            if (theMemory!=NULL){
                if(theMemory->writeShareMemory(pDeviceInput,size_mem)!=true)
                    ssSetErrorStatus(S,"ERROR writeShareMemory \n");
            }else{
                ssSetErrorStatus(S,"simpleShareMemoryContainer.getFromID(2*device) : MEMORY NOT FOUND \n");
            }
            delete pDeviceInput;
            mxFree(tipo);
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
    int     device = (int)(mxGetPr(ssGetSFcnParam(S,DISPOSITIVO))[0]) - 1;
    simpleShareMemoryContainer.removeFromID(2*device);
    simpleShareMemoryContainer.removeFromID(2*device);
    if(simpleShareMemoryContainer.getCount()==0)
        simpleShareMemoryContainer.removeAll();
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
