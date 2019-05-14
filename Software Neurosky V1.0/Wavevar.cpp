#include <winsock2.h> 
#include <windows.h>
#include <stdio.h>
#include <math.h> 
#include "mmtimer.h"
#include "wavevar.h"
#include "File.h"
#include "thinkgear.h"
#include "Library C++ Shared Memory V2.0.0\CshareMemory.h"

//----------------------------------------------------------------------------//
// VARIABLES GLOBALS
//timer
MMTIMER g_mmTimer;
//control data
CONTROL Control;
//state data
STATE State;

HANDLE hThread_com_tx_dspic=NULL;
HANDLE hThread_com_rx_dspic=NULL;

char memoria_escritura_char[50*4];//char memoria_escritura_char[5*4];
char memoria_lectura_char[50*4];
CshareMemory memoriaCompartida_entrada;
CshareMemory memoriaCompartida_salida;
int   connectionId = 0;
int   errCode      = 0;
int   packetsRead  = 0;

DWORD WINAPI Thread_com_rx_dspic(LPVOID lpParameter) ///Rx PC from DSPIC por USB
{
	
	int j=0;
	LARGE_INTEGER liStart1={0,0};
	LARGE_INTEGER liFinish1={0,0};

	LARGE_INTEGER liStart2={0,0};
	LARGE_INTEGER liFinish2={0,0};

	//errCode = MWM15_setFilterType(connectionId,MWM15_FILTER_TYPE_50HZ);//MWM15_FILTER_TYPE_60HZ
	//errCode = TG_EnableAutoRead(connectionId,1);

	/*
TG_DATA_POOR_SIGNAL 1
TG_DATA_ATTENTION 2
TG_DATA_MEDITATION 3
TG_DATA_RAW 4
TG_DATA_DELTA 5
TG_DATA_THETA 6
TG_DATA_ALPHA1 7
TG_DATA_ALPHA2 8
TG_DATA_BETA1 9
TG_DATA_BETA2 10
TG_DATA_GAMMA1 11
TG_DATA_GAMMA2 12
*/

	while(State.run==1) {

					 /* Read a single Packet from the connection */
					packetsRead = TG_ReadPackets( connectionId, -1);
            
					/* If TG_ReadPackets() was able to read a Packet of data... */
					if( packetsRead >=0 ) {

						for(j=1;j<13;j++){
							if( TG_GetValueStatus(connectionId, j) != 0 ) {
							State.dSensor[j-1]=(float)TG_GetValue(connectionId, j);

						
						}
							QueryPerformanceCounter(&liStart1);
							State.dDurationTx_dspic = (double)(liStart1.LowPart - liFinish1.LowPart) * 1000.0/ (double)State.m_dwFrec;	
							liFinish1=liStart1;
					}
						
						Sleep(1);
                
					} /* end "If TG_ReadPackets() was able to read a Packet..." */
					else{
						Sleep(2);
					}
		
					
					
	


	}//fin while

	


	return 1;
}


// FUNCTION: TIMER

void CALLBACK Interrupt(UINT Dummy1, UINT Dummy2, DWORD dwUser,
								DWORD Dummy4, DWORD Dummy5){
	VIRTUAL_DEVICE_ENTRADA pfInput;
	VIRTUAL_DEVICE_SALIDA pfOutput;
	static LARGE_INTEGER liStart={0,0};
	static LARGE_INTEGER liFinish={0,0};


	ZeroMemory(&pfInput,sizeof(VIRTUAL_DEVICE_ENTRADA));
	ZeroMemory(&pfOutput,sizeof(VIRTUAL_DEVICE_SALIDA));

	QueryPerformanceCounter(&liStart);
	State.dDuration_timer = (double)(liStart.LowPart - liFinish.LowPart) * 1000.0/ (double)State.m_dwFrec;	
	liFinish.LowPart=liStart.LowPart;




		if(State.run==1){
	    
			//Lectura del contenido de la memoria
			memoriaCompartida_entrada.readShareMemory(memoria_lectura_char);
			
	        //Pueden darse formato a los datos de la siguiente forma
			for(int k=0; k<(State.Nida1);k++){
					memcpy(&pfInput.Acciones[k], memoria_lectura_char+k*4, 1*4);	 
					State.dAccion[k]=pfInput.Acciones[k];
				
			}
	
			
			//ReadShareMemory(&pfInput);//lee de simulink las acciones a enviar al microcontrolador
	
			for(int k=0; k<State.Nvuelta1;k++){
				pfOutput.Estado[k]=State.dSensor[k];
			}

			//WriteShareMemory(&pfOutput);//escribe en simulink el estado de los sensores provenientes del microcontrolador
			
			memcpy(memoria_escritura_char, &pfOutput, (State.Nvuelta1)*4); 
	
	        memoriaCompartida_salida.writeShareMemory(memoria_escritura_char,(State.Nvuelta1)*4);
			
	
	

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
	Proceso=GetCurrentProcess();

	if(Proceso==NULL){
				MessageBox(NULL,TEXT("Error function OpenProcess"),TEXT("Error"),MB_OK);
	}else {

		if(!SetPriorityClass(Proceso,REALTIME_PRIORITY_CLASS)){
					MessageBox(NULL,TEXT("Error function SetPriorityClass"),TEXT("Error"),MB_OK);
		}
	}
					
}
//----------------------------------------------------------------------------//
int StartAll(HWND hWnd)
{

	//identificadores de los hilos
	DWORD dwThreadID2;

	char A[64], B[64], C[64],D[64],E[64],F[64];


	if(State.run==0){


	readFile(A, &State.muestreo, B, &State.puerto_com, C, &State.baud_rate,D,&State.iDispositivo,E,&State.Nida1,F,&State.Nvuelta1);


	State.run=1;

	
	if(!QueryPerformanceFrequency(&State.liFrec)) MessageBox(NULL,TEXT("High resolution counter not supported"),TEXT("Error"),MB_OK);
	


	sprintf_s(State.mensaje,"NEUROSKY_entrada_%d", State.iDispositivo);
	memoriaCompartida_entrada.setName(State.mensaje);
	memoriaCompartida_entrada.setSize((1+State.Nida1)*4);//en bytes

	sprintf_s(State.mensaje,"NEUROSKY_salida_%d", State.iDispositivo);
	memoriaCompartida_salida.setName(State.mensaje);
	memoriaCompartida_salida.setSize((1+State.Nvuelta1)*4);//en bytes


	memoriaCompartida_entrada.openShareMemory();
	memoriaCompartida_salida.openShareMemory();

	/*bJoystick = OpenShareMemory(TEXT(M_COMPARTIDA),State.iDispositivo-1);

	if (bJoystick == FALSE ){
		MessageBox(NULL,TEXT(M_COMPARTIDA),TEXT("Error"),MB_OK);
		return 0;
	}*/


		
		g_mmTimer.uiPeriod = State.muestreo;  // Request of Period of the Multimedia Timer
		g_mmTimer.lpProc = Interrupt; // Callback timer function 
		g_mmTimer.dwData = 0; 
		if(!mmTimerStart(&g_mmTimer)) {
			MessageBox(NULL,TEXT("JoystickThread::mmTimerStart() Error"),TEXT("Error"), 
				MB_OK);
			return 0;
		}
		if( !SetTimer(hWnd,1,100,NULL) ) {
			MessageBox(NULL,TEXT("Set Timer Error"),TEXT("Error"),MB_OK);
			return 0;
		}
	



	sprintf_s(State.mensaje,"COM%i",State.puerto_com);

	//int dllVersion = TG_GetVersion();
    //printf( "Stream SDK for PC version: %d\n", dllVersion );
    
    /* Get a connection ID handle to ThinkGear */
	connectionId = TG_GetNewConnectionId();


    errCode = TG_Connect( connectionId,
                         State.mensaje,
                         TG_BAUD_115200,
                         TG_STREAM_PACKETS );
    if( errCode < 0 ) {
            sprintf_s(State.mensaje,"COM%i",State.puerto_com);
			MessageBox(NULL,State.mensaje,"Error",MB_OK);
			State.run=0;
			return 0;
			//fprintf( stderr, "ERROR: TG_Connect() returned %d.\n", errCode );
        //wait();
        //exit( EXIT_FAILURE );
    }else{
	

		hThread_com_rx_dspic = CreateThread(NULL,0,Thread_com_rx_dspic, NULL, 0, &dwThreadID2);
		
		if ( hThread_com_rx_dspic == NULL){
				MessageBox(NULL,"Error al abrir hilo Rx COM", "Error",MB_OK);
			return 0;
		}else{
			if(!SetThreadPriorityBoost(hThread_com_rx_dspic,TRUE)){
				MessageBox(NULL,"SetThreadPriorityBoost","Error",MB_OK);
			}
		
			if( !SetThreadPriority(hThread_com_rx_dspic,THREAD_PRIORITY_NORMAL)){//
				MessageBox(NULL,"Set Prioridad del hilo Error COM","Error",MB_OK);
			}
		}


		
	}
		

	}else{
		MessageBox(NULL,TEXT("Presione stop"),TEXT("Error"),MB_OK);
		return 0;
	}


		

	return 1;
} // int StartAll(void)
//----------------------------------------------------------------------------//
void StopAll(HWND hWnd)
{
	if(State.run==1){
		State.run=0;
		Sleep(50);
		//errCode = TG_EnableAutoRead(connectionId,0);

		TG_Disconnect(connectionId); // disconnect test
    
		/* Clean up */
		TG_FreeConnection( connectionId );
	
		CloseHandle(hThread_com_rx_dspic);

		if ( mmTimerEnd(g_mmTimer) == FALSE) MessageBox(NULL,TEXT("Error mmTimerEnd"),TEXT("Error"),MB_OK);

		if( KillTimer(hWnd,1) ==0) MessageBox(NULL,TEXT("Error CloseHandle KillTimer"),TEXT("Error"),MB_OK);
	
		//if( CloseShareMemory() ==FALSE) MessageBox(NULL,TEXT("Error Close Memory"),TEXT("Error"),MB_OK);
		memoriaCompartida_entrada.closeShareMemory();
		memoriaCompartida_salida.closeShareMemory();

		}


	
} // StopAll() 



