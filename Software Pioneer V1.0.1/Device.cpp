#include <winsock2.h> 
#include <windows.h>
#include <stdio.h>
#include <math.h> 
  
#include "Pioneer.h"  
#include "VirtualDevice.h"
#include "Device.h"

#include <iostream>
using namespace std;

//-------	---------------------------------------------------------------------//
// VARIABLES GLOBALES
//timer
HANDLE hTimerQueue = NULL;
HANDLE hTimer = NULL;

//control data
CONTROL Control;
//state data
STATE State;

bool timer_creado;
bool memoria_abierta;
bool changeParametros;
bool changePose;
bool setPose;
bool setParametros;

// The robot object, with which we must interact
extern ArRobot robot;
extern ArRobotPacket paquete;
extern ArSick sick;					//Se crea el objeto sick para manejar el laser
extern bool useSim;
static VIRTUAL_DEVICE_ENTRADA pfInput;
static VIRTUAL_DEVICE_SALIDA pfOutput;

extern DWORD WINAPI HiloPioneer( LPVOID lpParam );
void CALLBACK TimerRoutine(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5);
int StartAll(HWND hWnd);
void StopAll(HWND hWnd);
DWORD  dwHiloIdPioneer;
HANDLE hHiloPioneer;						// Handle del hilo del Pioneer
HANDLE hEventoLeerPID;						// Handle del evento LeerParamPID
double tiempoLeerPID;

//FUNCIONES GLOBALES/////
void Escribir_pos(double x, double y, double theta)
{
	x=x*1000.0;
	y=y*1000.0;
	theta=theta*180.0/PI;
	//ArPose pos_origen;//,odompose; //posicion origen robot
	ArPose pos_nueva;//,encopose; //posicion nueva robot

	robot.lock();

	//pos_origen = robot.getPose();
	pos_nueva.setX(x);
	pos_nueva.setY(y);
	pos_nueva.setTh(theta);
	robot.moveTo(pos_nueva);

	if(useSim)
	{
		ArPose pose;	
		pose.setX(x);
		pose.setY(y);
		pose.setTh(theta);
		ArRobotPacket pk;
		pk.setID(ArCommands::SIM_SET_POSE);
		pk.uByteToBuf(0);  // arg type, N/A for this command
		pk.byte4ToBuf(ArMath::roundInt(pose.getX()));
		pk.byte4ToBuf(ArMath::roundInt(pose.getY()));
		pk.byte4ToBuf(ArMath::roundInt(pose.getTh()));
		pk.finalizePacket();
		robot.getDeviceConnection()->write(pk.getBuf(),pk.getLength());
	}
	robot.unlock();
}

void Leer_Laser()
{
	////////////////////////////////////////////////////////////////////
	const std::list<ArSensorReading *> *readings;
	std::list<ArSensorReading *>::const_iterator it;
	int i;
	//MIDO EL LASER
	sick.lockDevice();
	readings = sick.getRawReadings();
	for (i = 0, it = readings->begin(); it != readings->end(); it++, i++)
	{
		State.iLaser[i] = (*it)->getRange();
	}
	sick.unlockDevice();
}

void ErrorHandler(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

//----------------------------------------------------------------------------//
// FUNCTION: TIMER
void CALLBACK TimerRoutine(UINT Dummy1, UINT Dummy2, DWORD dwUser,	DWORD Dummy4, DWORD Dummy5)
{
	if(robot.isConnected()){
		
		tiempoLeerPID=tiempoLeerPID+((double)(DEFAULT_TIEMPO_MUESTREO_MEMORIA)/1000.0);
		if(tiempoLeerPID > 3.0)
		{
			tiempoLeerPID=0.0;
			SetEvent(hEventoLeerPID);
		}

		robot.lock();
		State.dPose[0]=robot.getX()/1000.0;				//[m]
		State.dPose[1]=robot.getY()/1000.0;				//[m]
		State.dPose[2]=robot.getTh()*PI/180.0;			//[rad]
		State.dVelocity[0]=robot.getVel()/1000.0;		//[m/s]
		State.dVelocity[1]=robot.getRotVel()*PI/180.0;	//[rad/s]
		robot.unlock();

		//Leer_Laser();

		pfOutput.Estado[0]=State.dPose[0];
		pfOutput.Estado[1]=State.dPose[1];
		pfOutput.Estado[2]=State.dPose[2];
		pfOutput.Estado[3]=State.dVelocity[0];
		pfOutput.Estado[4]=State.dVelocity[1];
	
		for(int i=0;i<NUM_LASER;i++)
			pfOutput.MedidasLaser[i]=State.iLaser[i];

		for(int i=0;i<NUM_PARAM;i++)
			pfOutput.Parametros[i]=State.iParametros[i];
		
		WriteShareMemory(&pfOutput);

		ReadShareMemory(&pfInput);
		
		if(pfInput.Actualizar_parametros!=changeParametros)
		{
			Control.P_Lin=pfInput.Parametros[0];
			Control.I_Lin=pfInput.Parametros[1];
			Control.D_Lin=pfInput.Parametros[2];

			Control.P_Rot=pfInput.Parametros[3];
			Control.I_Rot=pfInput.Parametros[4];
			Control.D_Rot=pfInput.Parametros[5];

			robot.lock();
			robot.comInt(82,Control.P_Rot);		//ROTKP
			robot.comInt(83,Control.D_Rot);		//ROTKV
			robot.comInt(84,Control.I_Rot);		//ROTKI
			robot.comInt(85,Control.P_Lin);		//TRANSKP
			robot.comInt(86,Control.D_Lin);		//TRANSKV
			robot.comInt(87,Control.I_Lin);		//TRANSKI
			robot.unlock();

			if(changeParametros)
				changeParametros=false;
			else
				changeParametros=true;
		}
		else
		{
			if(setParametros)
			{
				setParametros=false;
				robot.lock();
				robot.comInt(82,Control.P_Rot);		//ROTKP
				robot.comInt(83,Control.D_Rot);		//ROTKV
				robot.comInt(84,Control.I_Rot);		//ROTKI
				robot.comInt(85,Control.P_Lin);		//TRANSKP
				robot.comInt(86,Control.D_Lin);		//TRANSKV
				robot.comInt(87,Control.I_Lin);		//TRANSKI
				robot.unlock();
			}
		}
		
		if(pfInput.Actualizar_postura!=changePose)
		{
			Escribir_pos(pfInput.Postura[0],pfInput.Postura[1],pfInput.Postura[2]);
			if(changePose)
				changePose=false;
			else
				changePose=true;
		}
		else
		{
			if(setPose)
			{
				setPose=false;
				Escribir_pos(Control.Postura[0],Control.Postura[1],Control.Postura[2]*PI/180.0);
			}
		}

		robot.lock();
		robot.setVel(pfInput.Acciones[0]*1000.0);
		robot.setRotVel(pfInput.Acciones[1]*180.0/PI);
		robot.unlock();
	}
}

//----------------------------------------------------------------------------//
int StartAll(HWND hWnd)
{
	LARGE_INTEGER liFrec;
	HANDLE Proceso;
	DWORD ProcesoId;

	liFrec.LowPart = 0;
	liFrec.HighPart = 0;
	if(!QueryPerformanceFrequency(&liFrec))
		ErrorHandler(TEXT("QueryPerformanceFrequency"));

	ProcesoId=(DWORD)GetCurrentProcessId();
	Proceso=OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcesoId);

	if(Proceso==NULL)
		ErrorHandler(TEXT("Proceso==NULL"));

	if(!SetPriorityClass(Proceso,REALTIME_PRIORITY_CLASS))
		ErrorHandler(TEXT("SetPriorityClass"));

	SecureZeroMemory(&pfInput,sizeof(VIRTUAL_DEVICE_ENTRADA));
	SecureZeroMemory(&pfOutput,sizeof(VIRTUAL_DEVICE_SALIDA));

	timer_creado=false;
	memoria_abierta=false;
	changeParametros=false;
	changePose=false;
	setPose=false;
	setParametros=false;
	tiempoLeerPID=0.0;

	if(!State.run)
	{
		// Create the timer queue.
		hTimerQueue = CreateTimerQueue();
		if (NULL == hTimerQueue)
		{
			ErrorHandler(TEXT("CreateTimerQueue"));
			return 0;
		}

		// Set a timer to call the timer routine in 10 seconds.
		if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, 
			(WAITORTIMERCALLBACK)TimerRoutine, NULL , 0, DEFAULT_TIEMPO_MUESTREO_MEMORIA, 0)) //DEFAULT_TIEMPO_ENVIO [ms] definido en el .h
		{
			ErrorHandler(TEXT("CreateTimerQueueTimer"));
			return 0;
		}
			
		if( !SetTimer(hWnd,1,100,NULL) ) {
			ErrorHandler(TEXT("SetTimer del Dialogo"));
			return 0;
		}
		else
			timer_creado=true;

		///////////////////////////////////////////////////////
		///// Creacion del Evento Leer PID ////////////////////	
		///////////////////////////////////////////////////////
		hEventoLeerPID = CreateEvent(
			NULL,				// no security attributes
			TRUE,				// manual-reset event
			TRUE,				// initial state is signaled
			NULL				// object name
			); 
		if (hEventoLeerPID == NULL)
		{
			ErrorHandler(TEXT("CreateEvent LeerParamPID"));
			return 0;
		}

		//----------------------
		//Crea el Hilo del ROBOT :
		hHiloPioneer = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			HiloPioneer,			// thread function name
			(LPVOID)NULL,			// argument to thread function 
			0,                      // use default creation flags 
			&dwHiloIdPioneer);		// returns the thread identifier 
		// Check the return value for success.
		// If CreateThread fails, terminate execution. 
		// This will automatically clean up threads and memory. 

		if (hHiloPioneer == NULL) 
		{
			ErrorHandler(TEXT("CreateThread Hilo Pioneer"));
			return 0;
		}

		State.run=true;

		for(int i=0;i<6;i++)
			State.iParametros[i]=0;

		tiempoLeerPID=0.0;

	}else
		MessageBox(NULL,TEXT("Presione stop"),TEXT("Error"),MB_OK);

	return 1;
} // int StartAll(void)
//----------------------------------------------------------------------------//
void StopAll(HWND hWnd)
{
	if(State.run)
	{
		if( hTimerQueue != NULL)
		{
			if( hTimer != NULL)
			{
				// Cancels a pending timer-queue timer.
				if (!DeleteTimerQueueTimer(hTimerQueue,hTimer,INVALID_HANDLE_VALUE))
					ErrorHandler(TEXT("DeleteTimerQueueTimer"));
			}

			// Delete all timers in the timer queue.
			if (!DeleteTimerQueueEx(hTimerQueue,INVALID_HANDLE_VALUE))
				ErrorHandler(TEXT("DeleteTimerQueueEx"));
		}

		if(memoria_abierta)
		{
			if(!CloseShareMemory())
				ErrorHandler(TEXT("CloseShareMemory"));
		}

		if(timer_creado)
		{
			if( KillTimer(hWnd,1)==0)
				ErrorHandler(TEXT("KillTimer"));
		}
	}

	if(robot.isConnected())
	{
		robot.remRangeDevice(&sick);
		sick.lockDevice();
		sick.disconnect();
		sick.unlockDevice();

		robot.lock();
		robot.stopRunning(true);
		robot.unlock();

		Sleep(100);

		if(hEventoLeerPID!=NULL)
		{
			SetEvent(hEventoLeerPID);
			Sleep(50);
			CloseHandle(hEventoLeerPID);
		}
	}

	State.run=false;
} // StopAll() 
