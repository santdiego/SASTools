#include <winsock2.h> 
#include <windows.h>
#include <stdio.h>
#include <math.h> 
  
#include "haptics.h"  

#include "VirtualDevice.h"
#include "mmtimer.h"
#include "wavevar.h"

//----------------------------------------------------------------------------//
// VARIABLES GLOBALS
//timer
MMTIMER g_mmTimer;
//control data
CONTROL Control;
//state data
STATE State;
DWORD  dwThreadID3;
bool haptic_device=0;

// The haptics object, with which we must interact
HapticsClass gHaptics;


//----------------------------------------------------------------------------//
// FUNCTION: TIMER

void CALLBACK Interrupt(UINT Dummy1, UINT Dummy2, DWORD dwUser,
								DWORD Dummy4, DWORD Dummy5){
	VIRTUAL_DEVICE_ENTRADA pfInput;
	VIRTUAL_DEVICE_SALIDA pfOutput;

	ZeroMemory(&pfInput,sizeof(VIRTUAL_DEVICE_ENTRADA));
	ZeroMemory(&pfOutput,sizeof(VIRTUAL_DEVICE_SALIDA));


	if(haptic_device==TRUE){
		gHaptics.getPosition(State.dPosition);
		gHaptics.getVelocity(State.dVelocity);
		gHaptics.getButtons(State.dButtons);

		pfOutput.Estado[0]=State.dPosition[0];
		pfOutput.Estado[1]=State.dPosition[1];
		pfOutput.Estado[2]=State.dPosition[2];

		pfOutput.Estado[3]=State.dVelocity[0];
		pfOutput.Estado[4]=State.dVelocity[1];
		pfOutput.Estado[5]=State.dVelocity[2];

		pfOutput.Botones[0]=State.dButtons[0];
		pfOutput.Botones[1]=State.dButtons[1];
		pfOutput.Botones[2]=State.dButtons[2];
		pfOutput.Botones[3]=State.dButtons[3];

		WriteShareMemory(&pfOutput);

		ReadShareMemory(&pfInput);

		if(pfInput.Actualizar_parametros!=gHaptics.changeParametros)
		{
			Control.dDamping[0]=pfInput.Parametros[0];
			Control.dDamping[1]=pfInput.Parametros[1];
			Control.dDamping[2]=pfInput.Parametros[2];

			Control.dSpring[0]=pfInput.Parametros[3];
			Control.dSpring[1]=pfInput.Parametros[4];
			Control.dSpring[2]=pfInput.Parametros[5];
			if(gHaptics.changeParametros)
				gHaptics.changeParametros=false;
			else
				gHaptics.changeParametros=true;
		}

		gHaptics.setDamping(Control.dDamping);
		gHaptics.setSpring(Control.dSpring);

		State.dForce[0]=pfInput.Acciones[0];
		State.dForce[1]=pfInput.Acciones[1];
		State.dForce[2]=pfInput.Acciones[2];

		gHaptics.SetForce(State.dForce);
	}
}
//----------------------------------------------------------------------------//
void InitState()
{
	LARGE_INTEGER liFrec;
	HANDLE Proceso;
	DWORD ProcesoId;


	liFrec.LowPart = 0;
	liFrec.HighPart = 0;
	if(!QueryPerformanceFrequency(&liFrec)){
		MessageBox(NULL,TEXT("High resolution counter not supported"),TEXT("Error"),MB_OK);
		liFrec.LowPart = 0;
		liFrec.HighPart = 0;
	}	
	
	State.m_dwFrec = liFrec.LowPart;


	ProcesoId=(DWORD)GetCurrentProcessId();

	Proceso=OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcesoId);

	if(Proceso==NULL){
				MessageBox(NULL,TEXT("Error function OpenProcess"),TEXT("Error"),MB_OK);
	}

	if(!SetPriorityClass(Proceso,REALTIME_PRIORITY_CLASS)){
				MessageBox(NULL,TEXT("Error function SetPriorityClass"),TEXT("Error"),MB_OK);
	}
					
}
//----------------------------------------------------------------------------//
int StartAll(HWND hWnd)
{

	if(State.run==0){
    BOOL bJoystick;


	gHaptics.init(Control.iDispositivo);
	// Tell the user what to do if the device is not calibrated
    if (!gHaptics.isDeviceCalibrated())
        MessageBox(// The next two lines are one long string
				   NULL,
                   TEXT("Please home the device by extending\n")
                   TEXT("then pushing the arms all the way in."),
                   TEXT("Not Homed"),
                   MB_OK);


	if(!QueryPerformanceFrequency(&State.liFrec)) MessageBox(NULL,TEXT("High resolution counter not supported"),TEXT("Error"),MB_OK);
	

	bJoystick = OpenShareMemory(TEXT(M_COMPARTIDA),Control.iDispositivo);

	if (bJoystick == FALSE ){
		MessageBox(NULL,TEXT(M_COMPARTIDA),TEXT("Error"),MB_OK);
		return 0;
	}


		// Set the MMTIMER struct members for start the Mutimedia Timer */
		g_mmTimer.uiPeriod = SAMPLE_TIME;  // Request of Period of the Multimedia Timer
		g_mmTimer.lpProc = Interrupt; // Callback timer function 
		g_mmTimer.dwData = 0; // Parameter for the Callback timer function 
		if(!mmTimerStart(&g_mmTimer)) {
			MessageBox(NULL,TEXT("JoystickThread::mmTimerStart() Error"),TEXT("Error"), 
				MB_OK);
			return 0;
		}
		if( !SetTimer(hWnd,1,100,NULL) ) {
			MessageBox(NULL,TEXT("Set Timer Error"),TEXT("Error"),MB_OK);
			return 0;
		}
		State.run=1;

	}else{
		MessageBox(NULL,TEXT("Presione stop"),TEXT("Error"),MB_OK);
	}

	return 1;
} // int StartAll(void)
//----------------------------------------------------------------------------//
void StopAll(HWND hWnd)
{

	if ( mmTimerEnd(g_mmTimer) == FALSE) MessageBox(NULL,TEXT("Error mmTimerEnd"),TEXT("Error"),MB_OK);

	if( KillTimer(hWnd,1) ==0) MessageBox(NULL,TEXT("Error CloseHandle KillTimer"),TEXT("Error"),MB_OK);
	
	if( CloseShareMemory() ==FALSE) MessageBox(NULL,TEXT("Error Close Memory"),TEXT("Error"),MB_OK);

	gHaptics.uninit();
	State.run=0;
} // StopAll() 





