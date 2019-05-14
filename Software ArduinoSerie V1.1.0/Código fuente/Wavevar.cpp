#include <winsock2.h> 
#include <windows.h>
#include <stdio.h>
#include <math.h> 
#include "mmtimer.h"
#include "wavevar.h"
#include "File.h"
#include "Library C++ Shared Memory V2.0.0\CshareMemory.h"

//----------------------------------------------------------------------------//
// VARIABLES GLOBALS
//timer
MMTIMER g_mmTimer;
//control data
CONTROL Control;
//state data
STATE State;
HANDLE g_hEvent_dspic=NULL;
HANDLE hThread_com_tx_dspic=NULL;
HANDLE hThread_com_rx_dspic=NULL;

char memoria_escritura_char[50*4];//char memoria_escritura_char[5*4];
char memoria_lectura_char[50*4];
CshareMemory memoriaCompartida_entrada;
CshareMemory memoriaCompartida_salida;


DWORD WINAPI Thread_com_rx_dspic(LPVOID lpParameter) ///Rx PC from DSPIC por USB
{
	

	DWORD resultado;
	LARGE_INTEGER liStart2={0,0};
	LARGE_INTEGER liFinish2={0,0};
	OVERLAPPED gOverlapped_Rx;
	DWORD dRx1;
	unsigned char  Rx1[50];
	gOverlapped_Rx.Offset     = 0; 
	gOverlapped_Rx.OffsetHigh = 0; 
	gOverlapped_Rx.hEvent     = CreateEvent(NULL,FALSE,FALSE,NULL); 


	while(State.run==1) {

					ReadFile(State.hcom, &Rx1[0], 1, &dRx1, &gOverlapped_Rx); 
		
					resultado=WaitForSingleObject(gOverlapped_Rx.hEvent,2*State.muestreo);
		
					if(resultado!=WAIT_OBJECT_0){
						Rx1[0]=0;
						State.dDurationRx_dspic=-1.0;
						State.bandera_encabezado=0;
					}else{

						State.iPacksRx_dspic++;
			
						if(Rx1[0]==0xFF && State.bandera_encabezado<2){	

							State.bandera_encabezado++;

										
										if(State.bandera_encabezado>=2){
											
										
														
										
											ReadFile(State.hcom, &Rx1[1], 2*State.Nvuelta1+2, &dRx1, &gOverlapped_Rx); 
											
		
											resultado=WaitForSingleObject(gOverlapped_Rx.hEvent,2*State.muestreo);
										

											if(resultado==WAIT_OBJECT_0){

												
		
												//
									
												if(Rx1[2*State.Nvuelta1+2]==0){

													QueryPerformanceCounter(&liStart2);
													State.dDurationRx_dspic = (double)(liStart2.LowPart - liFinish2.LowPart) * 1000.0/ (double)State.m_dwFrec;	
													liFinish2=liStart2;
													State.dSensor[0]=(float)(Rx1[1]);
													for(int k=1;k<State.Nvuelta1+1;k++){
														//State.dSensor[k]=(float)(Rx1[1+2*k]+256.0*(Rx1[2*k] & 0x3F));
														State.dSensor[k]=(float)(Rx1[1+2*k]+256.0*(Rx1[2*k] & 0x7F));
														//if((Rx1[2*k] & 0x40) >0)State.dSensor[k]=(float)-1.0*State.dSensor[k];
														if((Rx1[2*k] & 0x80) >0)State.dSensor[k]=(float)-1.0*State.dSensor[k];
					
													}

												
													
												}
												
											}else{
												State.dDurationRx_dspic=-2.0;
											}
											State.bandera_encabezado=0;
											if(State.hcom!=NULL) PurgeComm(State.hcom, PURGE_RXCLEAR);
											
										}
						}else{
										State.dDurationRx_dspic=-3.0;
										State.bandera_encabezado=0;
						}
					}
					
	


	}//fin while

	


	return 1;
}

//-----------------------------------------------------------------------------------
DWORD WINAPI Thread_com_tx_dspic(LPVOID lpParameter)    //transmite hacia el microcontrolador
{
	

	DWORD resultado=0;
	LARGE_INTEGER liStart1={0,0};
	LARGE_INTEGER liFinish1={0,0};
	OVERLAPPED gOverlapped_Tx;
	DWORD dTx1;
	unsigned char  Tx1[50];
	unsigned int aux;
	gOverlapped_Tx.Offset     = 0; 
	gOverlapped_Tx.OffsetHigh = 0; 
	gOverlapped_Tx.hEvent     = CreateEvent(NULL,FALSE,FALSE,NULL); 



	while(State.run==1) {

					resultado=WaitForSingleObject(g_hEvent_dspic,1000); 

					if((resultado==WAIT_OBJECT_0) /*&& (((byte)State.dAccion[0] & 0x80)==1)*/){

						Tx1[0]=0xFF;
						Tx1[1]=0xFF;
						Tx1[2]=(unsigned char) (State.dAccion[0]);//palabra de control (play, stop, pause)
						Tx1[3]=(unsigned char) (State.Nida1);
						Tx1[4]=(unsigned char) (State.Nvuelta1);

						
						for(int k=1; k<State.Nida1+1;k++){
								//aux= (unsigned int) (fabs(State.dAccion[k]));
								if(State.dAccion[k]>=0){
									aux= (unsigned int) (State.dAccion[k]);
								}else{
									aux= (unsigned int) (-1.0*State.dAccion[k]);
								}

								Tx1[3+2*k]=(unsigned char) (0x00FF & aux);
								//Tx1[2+2*k]=(unsigned char) ((((aux>> 8) & 0x00FF)) & 0x3F);
								Tx1[4+2*k]=(unsigned char) ((((aux>> 8) & 0x00FF)) & 0x007F);
								if(State.dAccion[k]<0.0){
									//Tx1[2+2*k]=Tx1[2+2*k] | 0x40;
									Tx1[4+2*k]=Tx1[4+2*k] | 0x80;//bit mas significativo en 1 (valor negativo)
								}
						}
			
						Tx1[2*State.Nida1+2+3]=0x00;//fin de trama -> podria incluir un checksum


						if(State.hcom!=NULL) WriteFile(State.hcom, &Tx1[0], 2+3+(2*State.Nida1+1), &dTx1, &gOverlapped_Tx); //envia acciones de control PC_TO_DSPIC
				
					
						resultado=WaitForSingleObject(gOverlapped_Tx.hEvent,2*State.muestreo);
						
						if(resultado==WAIT_OBJECT_0){
							State.iPacksTx_dspic++;
							QueryPerformanceCounter(&liStart1);
							State.dDurationTx_dspic = (double)(liStart1.LowPart - liFinish1.LowPart) * 1000.0/ (double)State.m_dwFrec;	
							liFinish1.LowPart=liStart1.LowPart;
							//ResetEvent(g_hEvent_dspic);
						}else{
							State.dDurationTx_dspic=-1.0;
						}
						


					}else{
						State.dDurationTx_dspic=-1.0;
					}

				
					if(State.hcom!=NULL) PurgeComm(State.hcom, PURGE_TXCLEAR);
					ResetEvent(g_hEvent_dspic);
					
					

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
			for(int k=0; k<(State.Nida1+1);k++){
					memcpy(&pfInput.Acciones[k], memoria_lectura_char+k*4, 1*4);	 
					State.dAccion[k]=pfInput.Acciones[k];
				
			}
	
			
			//ReadShareMemory(&pfInput);//lee de simulink las acciones a enviar al microcontrolador
	
			for(int k=0; k<State.Nvuelta1+1;k++){
				pfOutput.Estado[k]=State.dSensor[k];
			}

			//WriteShareMemory(&pfOutput);//escribe en simulink el estado de los sensores provenientes del microcontrolador
			
			memcpy(memoria_escritura_char, &pfOutput, (State.Nvuelta1+1)*4); //Agrego 5 flotantes
	
	        memoriaCompartida_salida.writeShareMemory(memoria_escritura_char,(State.Nvuelta1+1)*4);
			
			
			SetEvent(g_hEvent_dspic);//PC maestro arduino esclavo
	

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
	State.iDispositivo=1;

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
	//estructura de configuración de puerto COM
	DCB dcb1;
	//identificadores de los hilos
	DWORD dwThreadID1, dwThreadID2;

	char A[64], B[64], C[64],D[64],E[64],F[64];


	if(State.run==0){



		readFile(A, &State.muestreo, B, &State.puerto_com, C, &State.baud_rate,D,&State.iDispositivo,E,&State.Nida1,F,&State.Nvuelta1);


	State.run=1;
   

	g_hEvent_dspic = CreateEvent(NULL,TRUE,FALSE,NULL);
	if( g_hEvent_dspic == NULL){
		MessageBox(NULL,"Error: CreateEvent g_hEventStop","Error",MB_OK);
		return 0;
	}

	
	if(!QueryPerformanceFrequency(&State.liFrec)) MessageBox(NULL,TEXT("High resolution counter not supported"),TEXT("Error"),MB_OK);
	


	sprintf_s(State.mensaje,"ARDUINO_entrada_%d", State.iDispositivo);
	memoriaCompartida_entrada.setName(State.mensaje);
	memoriaCompartida_entrada.setSize((1+State.Nida1)*4);//en bytes

	sprintf_s(State.mensaje,"ARDUINO_salida_%d", State.iDispositivo);
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
	
	//sprintf_s(State.mensaje,"COM%i",State.puerto_com);
	sprintf_s(State.mensaje,"\\\\.\\COM%i",State.puerto_com);

	State.hcom=CreateFile(State.mensaje,GENERIC_READ | GENERIC_WRITE, 0,NULL,OPEN_EXISTING, FILE_FLAG_OVERLAPPED,NULL);
		
	if(State.hcom==INVALID_HANDLE_VALUE)
	{
		    sprintf_s(State.mensaje,"COM%i",State.puerto_com);
			MessageBox(NULL,State.mensaje,"Error",MB_OK);
			State.hcom=NULL;
			State.run=0;
			return 0;
	}else{



		PurgeComm(State.hcom, PURGE_TXCLEAR | PURGE_RXCLEAR);
										 
		//configurar el puerto serie
		dcb1.DCBlength = sizeof(DCB);
		GetCommState(State.hcom,&dcb1);
		dcb1.BaudRate = State.baud_rate;
		dcb1.ByteSize = 8;
		dcb1.Parity = NOPARITY;
		dcb1.StopBits = ONESTOPBIT;
		dcb1.fDtrControl =DTR_CONTROL_DISABLE;
		dcb1.fRtsControl=RTS_CONTROL_DISABLE;
		dcb1.fOutxDsrFlow= FALSE;


		if(SetCommState(State.hcom, &dcb1)==0){
						sprintf_s(State.mensaje,"SetCommState COM%i",State.puerto_com);
						MessageBox(NULL,State.mensaje,"Error",MB_OK);
		}else{

			
		hThread_com_tx_dspic = CreateThread(NULL,0,Thread_com_tx_dspic, NULL, 0, &dwThreadID1);	
		
		if ( hThread_com_tx_dspic == NULL){
				MessageBox(NULL,"Error al abrir hilo Tx COM", "Error",MB_OK);
			return 0;
		}else{
			if(!SetThreadPriorityBoost(hThread_com_tx_dspic,TRUE)){
				MessageBox(NULL,"SetThreadPriorityBoost","Error",MB_OK);
			}
	
			if( !SetThreadPriority(hThread_com_tx_dspic,THREAD_PRIORITY_NORMAL)){//
				MessageBox(NULL,"Set Prioridad del hilo Error COM1","Error",MB_OK);
			}
			
		}


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
		CloseHandle(hThread_com_tx_dspic);
		CloseHandle(hThread_com_rx_dspic);

		if ( mmTimerEnd(g_mmTimer) == FALSE) MessageBox(NULL,TEXT("Error mmTimerEnd"),TEXT("Error"),MB_OK);

		if( KillTimer(hWnd,1) ==0) MessageBox(NULL,TEXT("Error CloseHandle KillTimer"),TEXT("Error"),MB_OK);
	
		//if( CloseShareMemory() ==FALSE) MessageBox(NULL,TEXT("Error Close Memory"),TEXT("Error"),MB_OK);
		memoriaCompartida_entrada.closeShareMemory();
		memoriaCompartida_salida.closeShareMemory();

		CloseHandle(g_hEvent_dspic);
		CloseHandle(State.hcom);
		}


	
} // StopAll() 





