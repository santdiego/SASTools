/**********************************************************************************************
Author : Ing. Diego Daniel Santiago.
diego.daniel.santiago@gmail.com
dsantiago@inaut.unsj.edu.ar
Facultad de Ingeniera
Universidad Nacional de San Juan
San Juan - Argentina

INAUT - Instituto de Automatica
http://www.inaut.unsj.edu.ar/
CONICET - Consejo Nacional de Investigaciones CientÃ­ficas y TÃ©cnicas.
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
 *              Se unificaros los tipos de datos destinados a tamaño a "unsigned long"
 *06/07/2017  	Se probaron varios tipos de tiempo de muestreo, se determino que INHERITED_SAMPLE_TIME es el mas adecuado para la aplicacion
************************************************************************************************/

#define S_FUNCTION_NAME   matlabGenericSHM2 //Cambiar este nombre por el nombre de su s-Function
#define S_FUNCTION_LEVEL 2

/*La memoria compartida de entrada figura como "nombreMemoria_entrada_#"
 *La memoria compartida de salida  figura como "nombreMemoria_salida_#" 
 * donde # es el numero de robot o DEVICE especificado en los parametros
 * y "nombreMemoria" debe ser fijada por el programador en el archivosFunctionProtoShareMemory.h
 */

#define NUM_PARAM       7 //Numero de parametros esperados
//Posicion de los parametros en el vector de parametros:
enum{DEV_INPUT_SHM_NAME,DEV_OUTPUT_SHM_NAME,INPUTS,OUTPUTS,INPUT_TYPES,OUTPUT_TYPES,TS};

#include "simstruc.h"
#include "matrix.h"
#include "CshareMemoryContainer.cpp"
#include "CshareMemory.cpp"
#include <iostream>

static CShareMemoryContainer shareMemoryContainer;

/*====================*
 * S-function methods *
 *====================*/
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)

static void mdlCheckParameters(SimStruct *S)
{
    
    
    //char *input_buf = mxArrayToString(ssGetSFcnParam(S,0));
    //printf("%s",input_buf );
    
    static unsigned long nameLength = (mxGetM(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME)) * mxGetN(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME))) + 1;
    
    if(nameLength<2||nameLength>50)
    { 
        //printf("%i",nameLength);
        ssSetErrorStatus(S,"The device Output shared memory name assigned is not Valid \n");
        return;
    }
    nameLength = (mxGetM(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME)) * mxGetN(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME))) + 1;
    
    if(nameLength<2||nameLength>50)
    { 
        ssSetErrorStatus(S,"The device Input shared memory name assigned is not Valid\n");
        return;
    }
    
    //Parametro 4 : Tipo de INPUTS
	//Verificar que coincida con el numero de INPUTS
    unsigned long num_input =mxGetN(ssGetSFcnParam(S,INPUTS));
    unsigned long num_output=mxGetN(ssGetSFcnParam(S,OUTPUTS));
    
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
    unsigned long num_input   = mxGetN(ssGetSFcnParam(S,INPUTS));
    unsigned long num_output  = mxGetN(ssGetSFcnParam(S,OUTPUTS));
    char *tipoE     = mxArrayToString(ssGetSFcnParam(S,INPUT_TYPES));
    char *tipoS     = mxArrayToString(ssGetSFcnParam(S,OUTPUT_TYPES));
    
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
        for(int i=0;i<num_output;i++){
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
   // int device      =	(int)(mxGetPr(ssGetSFcnParam(S,DEVICE))[0]) - 1;
	unsigned long num_input	=   mxGetN(ssGetSFcnParam(S,INPUTS));
    unsigned long num_output  =   mxGetN(ssGetSFcnParam(S,OUTPUTS));
    
	//char *devOutputShm_name = new char[50];
	//char *devInputShm_name  = new char[50];
    char *devOutputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME));
    char *devInputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME));
    char *tipoE     = mxArrayToString(ssGetSFcnParam(S,INPUT_TYPES));
    char *tipoS     = mxArrayToString(ssGetSFcnParam(S,OUTPUT_TYPES));

//    mex//printf(devInputShm_name);
//     mex//printf(devOutputShm_name);
    
    //Determinacion del tamaÃ±o de la memoria compartida de entrada:
    unsigned long  devInputShm_size=0;
	unsigned long  devOutputShm_size=0;
	unsigned long mult;
   
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
            devInputShm_size+=mult*(unsigned long)(mxGetPr(ssGetSFcnParam(S,INPUTS))[i]);
            //printf("mult[%i]:%lu \n",i,mult);
            //printf("(mxGetPr(ssGetSFcnParam(S,INPUTS))[%i]:%lu \n",i,(unsigned long)(mxGetPr(ssGetSFcnParam(S,INPUTS))[i]));
            //printf("devInputShm_size[%i]:%lu \n",i,devInputShm_size);
        }
        //Se instancia e inicializan los punteros a la clase memoria   
        CshareMemory *InShareMemory =new CshareMemory(devInputShm_name ,devInputShm_size);
        //Se abre la memoria compartida y se coloca el objeto en el contenedor para su administracion
        if(InShareMemory->openShareMemory()==true){
            if(shareMemoryContainer.insert(InShareMemory)==-1)
                ssSetErrorStatus(S,"ERROR ID duplicada");
        }else{
            ssSetErrorStatus(S,"ERROR InShareMemory->openShareMemory() \n");
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
            devOutputShm_size+=mult*(unsigned long)(mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i]);
            //printf("mult[%i]:%lu \n",i,mult);
            //printf("(mxGetPr(ssGetSFcnParam(S,OUTPUTS))[%i]:%lu \n",i,(unsigned long)(mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i]));
            //printf("devOutputShm_size[%i]:%lu \n",i,devOutputShm_size);
            
        }
        //Se instancia e inicializan los punteros a la clase memoria 
        //Memoria para sacar datos (salida del al bloque, lectura de memoria)
        CshareMemory *OutShareMemory=new CshareMemory(devOutputShm_name,devOutputShm_size);
        if(OutShareMemory->openShareMemory()==true){
            if(shareMemoryContainer.insert(OutShareMemory)==-1)
                ssSetErrorStatus(S,"ERROR ID duplicada");
        }else{
            ssSetErrorStatus(S,"ERROR OutShareMemory->openShareMemory()\n");
        }
    }

    //printf("Cantidad de memorias en contenedor: %d \n ",shareMemoryContainer.getCount());

	//delete devOutputShm_name;
	//delete devInputShm_name;
    
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
    unsigned long  num_output  = mxGetN(ssGetSFcnParam(S,OUTPUTS)); 
    
    if(num_output>0){
        char    *devOutputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME));
        char    *tipo	= mxArrayToString(ssGetSFcnParam(S,OUTPUT_TYPES));
        char    *pDeviceOutput;  
        int     aux=0;
        unsigned long _size=0;
        CshareMemory *theMemory = shareMemoryContainer.getFromName(devOutputShm_name);
        unsigned long devOutputShm_size = theMemory->getSize(); //LINEA DE PRUEBA
        //printf("devOutputShm_size : %i \n",devOutputShm_size); //DEBUG
        //Lectura de memoria compartida
        if (theMemory!=NULL){
            pDeviceOutput = new char[devOutputShm_size]; //LINEA DE PRUEBA
            //printf("read \n");
            if(theMemory->readShareMemory(pDeviceOutput)!=true){
                ssSetErrorStatus(S,"ERROR readShareMemory \n");
            }
        }else{
            //printf("DEVICE no encontrado: %d\n ",2*device+1);
            //printf("Cantidad de memorias en contenedor: %d\n ",shareMemoryContainer.getCount());
            ssSetErrorStatus(S,"shareMemoryContainer.getFromName(devInputShm_name) : MEMORY NOT FOUND \n");
        }   
        //Saque los datos del bloque
        for(int i=0;i<num_output;i++){
            switch(tipo[i]){ //verifico el tipo de dato
                case 'f'://float
                    _size=ssGetInputPortWidth(S,i)*sizeof(float);                       
                    memcpy(ssGetOutputPortSignal(S,i),pDeviceOutput+aux, _size);
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i]*sizeof(float);
                    break;
                case 'd'://double
                    memcpy( (double*)ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i]*sizeof(double)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i]*sizeof(double);
                    break;
                case 'i'://int32
                    memcpy( (int*)ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(int)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(int);
                    break;
                case 'c'://char
                    memcpy( (char*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(char)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(char);
                    break;
               case 'C'://unsigned char
                    memcpy( (unsigned char*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(unsigned char)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(unsigned char);
                    break;
               case 'I'://unsigned int
                    memcpy( (unsigned int*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(unsigned int)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(unsigned int);
                    break;
                default : //double
                    memcpy((double*) ssGetOutputPortSignal(S,i),
                            pDeviceOutput+aux,
                            (int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(double)
                           );
                    aux+=(int)mxGetPr(ssGetSFcnParam(S,OUTPUTS))[i] * sizeof(double);
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
      //printf("mdlUpdate \n");//DEBUG
      unsigned long num_input = mxGetN(ssGetSFcnParam(S,INPUTS));
      //Ingreso de los datos para enviar al DEVICE
        if(num_input!=0){
            char	*pDeviceInput;	
            char	*tipo = mxArrayToString(ssGetSFcnParam(S,INPUT_TYPES));
            unsigned long  	 aux = 0;
            char *devInputShm_name= mxArrayToString(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME));
            const void *u;
            CshareMemory	*theMemory = shareMemoryContainer.getFromName(devInputShm_name);
            unsigned long devInputShm_size = theMemory->getSize();
            pDeviceInput= new char[devInputShm_size];
            unsigned long _size=0;

            
            for(int i=0;i<num_input;i++){
                if ( !ssGetInputPortRequiredContiguous(S,i) ) {
                    ssSetErrorStatus(S,"Matlab was not able to allocate memory in a contiguous area (ssGetInputPortRequiredContiguous)\n");
                    break;
                }
                switch(tipo[i]){ //verifico el tipo de dato
                    case 'f'://float
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(float);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;
                    case 'd'://double
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(double);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;
                    case 'i'://int32
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(int);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;
                    case 'c'://char
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(char);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;
                    case 'C'://unsigned char
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(unsigned char);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;
                    case 'I'://unsigned int
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(unsigned int);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;
                         
                    default : //double
                        u = ssGetInputPortSignal(S,i);
                        _size=ssGetInputPortWidth(S,i)* sizeof(double);
                        //printf("[%i] Tamaño en char a ser copiado: %lu \n",i,_size); //DEBUG
                        //printf("[%i] Indice en Arreglo de destino: %lu \n",i,aux);//DEBUG
                        //printf("[%i] Tamaño destino: %lu \n",i,devInputShm_size);//DEBUG
                        if(aux+_size<devInputShm_size){
                            memcpy((pDeviceInput+aux),u, _size); //ERROR 
                        else {ssSetErrorStatus(S,"ERROR coding \n")};          
                        }aux +=_size;
                        break;  
                }          
            }
             
            if (theMemory!=NULL){
                //printf("write \n");
                if(theMemory->writeShareMemory(pDeviceInput,devInputShm_size)!=true)
                    ssSetErrorStatus(S,"ERROR writeShareMemory \n");
            }else{
                ssSetErrorStatus(S,"shareMemoryContainer.getFromID(2*device) : MEMORY NOT FOUND \n");
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
    //printf("mdlTerminate \n");//DEBUG
    char *devInputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_OUTPUT_SHM_NAME));
    char *devOutputShm_name	= mxArrayToString(ssGetSFcnParam(S,DEV_INPUT_SHM_NAME));
    //int     device = (int)(mxGetPr(ssGetSFcnParam(S,DEVICE))[0]) - 1;
    //shareMemoryContainer.removeFromID(2*device);
    //shareMemoryContainer.removeFromID(2*device+1);
    shareMemoryContainer.removeFromName(devInputShm_name);
    shareMemoryContainer.removeFromName(devOutputShm_name);
    if(shareMemoryContainer.getCount()==0)
        shareMemoryContainer.removeAll();
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
