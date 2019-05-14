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
 *08/10/2015 Lanzamiento Version 1
************************************************************************************************/

#define S_FUNCTION_NAME  sysCommand
#define S_FUNCTION_LEVEL 2

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
#include <windows.h>


/* Error handling
 * --------------
 *
 * You should use the following technique to report errors encountered within
 * an S-function:
 *
 *       ssSetErrorStatus(S,"Error encountered due to ...");
 *       return;
 *
 * Note that the 2nd argument to ssSetErrorStatus must be persistent memory.
 * It cannot be a local variable. For example the following will cause
 * unpredictable errors:
 *
 *      mdlOutputs()
 *      {
 *         char msg[256];         {ILLEGAL: to fix use "static char msg[256];"}
 *         sprintf(msg,"Error due to %s", string);
 *         ssSetErrorStatus(S,msg);
 *         return;
 *      }
 *
 */

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */

  #define START  0
  #define PERIOD 1
  #define STOP   2

static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 3);  /* Number of expected parameters */
    //Se esperan 3 parametros: Comando al inicio de la ejecucion y comando al final de la ejecucion. 
    //Y periodo de ejeccion de comando periodico
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    /* Number of Input Ports */
    if (!ssSetNumInputPorts(S, 1)) 
    {
        return;
    }
    
    //Se espera una entrada: Comando Periodico
    if (!ssSetInputPortDimensionInfo(S, 0, DYNAMIC_DIMENSION)) return;
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    _ssSetInputPortNumDimensions(S, 0, 1);
    ssSetInputPortDataType(S, 0, SS_UINT8);
    //Una salida: Respuesta de la consola
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);

//     ssSetNumSampleTimes(S, 1);
//     ssSetNumRWork(S, 0);
//     ssSetNumIWork(S, 0);
//     ssSetNumPWork(S, 0);
//     ssSetNumModes(S, 0);
//     ssSetNumNonsampledZCs(S, 0);

    /* Specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

       
    /* Dimension Modes of the Ports */
    ssSetInputPortDimensionsMode(S, 0, INHERIT_DIMS_MODE);
    //ssSetInputPortRequiredContiguous(S, 0, true);


    ssSetOptions(S,
                 SS_OPTION_WORKS_WITH_CODE_REUSE |
                 SS_OPTION_EXCEPTION_FREE_CODE |
                 SS_OPTION_USE_TLC_WITH_ACCELERATOR);
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    //Adquirir sample time y configurarlo
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
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
  static int ret=0;
  static char firstTime=0;
  static void mdlStart(SimStruct *S) 
  {   
      if( mxGetN(ssGetSFcnParam(S,START))>0){
          char *start	= mxArrayToString(ssGetSFcnParam(S,START));
          int i=system(start);       
      }
   
  }
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int *y = (int*)ssGetOutputPortSignal(S,0);
    *y = ret;
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
//       int uWidth1 = ssGetInputPortWidth(S, 0); //Largo del comando de entrada
//        if(uWidth1>0){
//            const char *u = (const char*) ssGetInputPortSignal(S,0);
//            printf("%s \n",u);
//            //ret=system(u); //Ejecuto el comando de sistema
//        }
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
    if( mxGetN(ssGetSFcnParam(S,STOP))>0){
        char *stop	= mxArrayToString(ssGetSFcnParam(S,STOP));
        int i = system(stop);       
    }
}


/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
