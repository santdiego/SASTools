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
 *13/08/2015    Lanzamiento version 1

************************************************************************************************/

#define S_FUNCTION_NAME   sysTime //Cambiar este nombre por el nombre de su s-Function
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <windows.h>
#include <stdio.h>

/*===================*
 * S-function methods *
 *====================*/
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)

static void mdlCheckParameters(SimStruct *S)
{
    
}
#endif /* MDL_CHECK_PARAMETERS */



static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);	
    
    if (!ssSetNumInputPorts(S,0)) return;  
    
//     ssSetInputPortWidth(S,0,1);
//     ssSetInputPortDataType(S, 0, SS_UINT16);
//     ssSetInputPortRequiredContiguous(S, 0, true);
//     ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S,8)) return;
    for(int i=0;i<8;i++){
        ssSetOutputPortWidth(S,i,1);
        ssSetOutputPortDataType(S, i, SS_UINT16);
    }

    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    /* Specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
    ssSetOptions(S, 0);
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
    system("w32tm /resync");
  }
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. 
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    
    *(uint16_T*)ssGetOutputPortSignal(S,0)=st.wYear;
    *(uint16_T*)ssGetOutputPortSignal(S,1)=st.wMonth;
    *(uint16_T*)ssGetOutputPortSignal(S,2)=st.wDayOfWeek;
    *(uint16_T*)ssGetOutputPortSignal(S,3)=st.wDay;
    *(uint16_T*)ssGetOutputPortSignal(S,4)=st.wHour;
    *(uint16_T*)ssGetOutputPortSignal(S,5)=st.wMinute;
    *(uint16_T*)ssGetOutputPortSignal(S,6)=st.wSecond;
    *(uint16_T*)ssGetOutputPortSignal(S,7)=st.wMilliseconds;
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
