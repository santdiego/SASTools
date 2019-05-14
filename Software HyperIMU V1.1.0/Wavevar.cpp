#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>    
#include <math.h>      
#include "mmtimer.h"
#include "wavevar.h"
#include "File.h"
#include "ShareMemory.h"
#include "Library C++ Shared Memory V2.0.0\CshareMemory.h"

char memoria_escritura_char[12*4+1*4+1*4];
char memoria_lectura_char[1*4];
CshareMemory memoriaCompartida_entrada;
CshareMemory memoriaCompartida_salida;


NETPACK_TO_CLIENTE datos_hacia_cliente;


//----------------------------------------------------------------------------//
// VARIABLES GLOBALS
//timer
MMTIMER g_mmTimer;
//state data
WV_STATE State;
HANDLE hThreadRecv;




DWORD WINAPI SocketRecvThread(LPVOID lpParameter)
{
	//struct hostent *hostData;
	struct sockaddr_in addrServer;
	struct sockaddr_in addrFrom;
	int iFromlen;//, iReceive; 
	SOCKET hSocket;
	//NETPACK_TO_CLIENTE pNetData;
	LARGE_INTEGER liStart;
	LARGE_INTEGER liFinish;
	int iBufferSize =  MASCOT_BUFFSIZE;
	u_short porta;
	int rc=0;
	DWORD flags = 0;
	OVERLAPPED Rx_red;
	WSABUF dataBuf;
	DWORD bytesRecv;

	//---socket UDP------


	hSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, WSA_FLAG_OVERLAPPED);//socket asincronico

	if(hSocket == INVALID_SOCKET ){
		MessageBox(NULL,"Error SocketRecvThread:socket()","Error",MB_OK);
		return 0;
	}else{
		Rx_red.hEvent = WSACreateEvent();
		if (Rx_red.hEvent == NULL) {
			MessageBox(NULL,"Error WSACreateEvent()","Error",MB_OK);
		}
	}
	ZeroMemory(&addrServer,sizeof(addrServer));
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr(State.cpBufferIPlocal);

	//------number port-----

	    porta = State.puerto;	
		addrServer.sin_port = htons(porta);

	

//-------------socket <---> number IP + number port--------------
	if( bind(hSocket,(PSOCKADDR)&addrServer,sizeof(addrServer)) != 0) {
		if(hSocket == WSAENETDOWN){
			MessageBox(NULL,"Error SocketRecvThread:bind()","Error",MB_OK);
			closesocket(hSocket);
			hSocket = INVALID_SOCKET;
			return FALSE;
		}
	}
	iFromlen = sizeof(addrFrom);


	//---------------RECEPTION LOOP-----------------------


	dataBuf.buf = ( char * )&datos_hacia_cliente;
    dataBuf.len = sizeof(NETPACK_TO_CLIENTE);
	int senderAddrSize = sizeof(addrFrom);

	while(State.run==1) {


			//------------RECEPTION DATAGRAMA UDP/IP----------
			rc = WSARecvFrom(hSocket,&dataBuf,1, &bytesRecv, &flags, (struct sockaddr *)&addrFrom, &senderAddrSize, &Rx_red, NULL );
			

			rc = WSAWaitForMultipleEvents( 1, &Rx_red.hEvent, TRUE, INFINITE, TRUE);
			
			if(!WSAGetOverlappedResult(hSocket, &Rx_red, &State.bytesRecv, FALSE, &flags)){
				//MessageBox(NULL,"Error WSAGetOverlappedResult","Error",MB_OK);
			}

			 const char s[2] = ",";
			 char *token;

			 int i=0;
   
			   /* get the first token */
			   token = strtok(datos_hacia_cliente.celular, s);
   
			   /* walk through other tokens */
			   while( token != NULL && i<DOF_SALIDA) 
			   {
				   State.IMU[i]= (float)atof (token);
    
				  token = strtok(NULL, s);
				  i++;
			   }
			

			QueryPerformanceCounter(&liStart);
			if(State.m_dwFrec != 0) {
				State.m_dDurationRx = (float)((double)(liStart.LowPart - liFinish.LowPart) * 1000.0/ (double)State.m_dwFrec);
			}
			liFinish.LowPart=liStart.LowPart;



			State.iPacksRx=State.iPacksRx+1;


	} 




	if( closesocket(hSocket) == SOCKET_ERROR ){
	}


	return 1;
}
//----------------------------------------------------------------------------//
// FUNCTION: TIMER

void CALLBACK Interrupt(UINT Dummy1, UINT Dummy2, DWORD dwUser,
								DWORD Dummy4, DWORD Dummy5){


	VIRTUAL_DEVICE_ENTRADA pfInput;
	VIRTUAL_DEVICE_SALIDA pfOutput;
	int j;

	ZeroMemory(&pfInput,sizeof(VIRTUAL_DEVICE_ENTRADA));
	ZeroMemory(&pfOutput,sizeof(VIRTUAL_DEVICE_SALIDA));

	for(j=0;j<DOF_ENTRADA;j++){
	pfInput.Acciones[j]=State.Acciones[j];
	}

	for(j=0;j<DOF_SALIDA;j++){
		pfOutput.Estado[j]=State.IMU[j];
	}

	pfOutput.PaquetesRx=State.iPacksRx;

	pfOutput.Sample_time=State.m_dDurationRx;

		//memcpy(void *destino			,void *fuente	, size_t tamaño)
	memcpy(memoria_escritura_char		, &pfOutput.Estado[0], 12*4);	 //Copio los 12 floats 
	memcpy(memoria_escritura_char + 12*4		, &pfOutput.PaquetesRx, 4); //Agrego 1 int 
	memcpy(memoria_escritura_char + 12*4 + 4	, &pfOutput.Sample_time, 4); //Agrego 1 float

	//Escritura de los datos:
	memoriaCompartida_salida.writeShareMemory(memoria_escritura_char);


	


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

	DWORD dwThreadID1;
	if(State.run==0){
	State.run=1;
	State.iPacksRx=0;

	readFile(State.cpBufferIPlocal,&State.puerto,&State.sample_time,&State.device);

	if(!QueryPerformanceFrequency(&State.liFrec)) MessageBox(NULL,TEXT("High resolution counter not supported"),TEXT("Error"),MB_OK);
	
	//CshareMemory memoriaCompartida_entrada("HYPERIMU_entrada_1",1*4);
   //CshareMemory memoriaCompartida_salida("HYPERIMU_salida_1",12*4+1*4+1*4);
	sprintf_s(State.mensaje, TEXT("HYPERIMU_entrada_%d"), State.device);
	memoriaCompartida_entrada.setName(State.mensaje);
	memoriaCompartida_entrada.setSize(1*4);//en bytes

	sprintf_s(State.mensaje, TEXT("HYPERIMU_salida_%d"), State.device);
	memoriaCompartida_salida.setName(State.mensaje);
	memoriaCompartida_salida.setSize(12*4+1*4+1*4);//en bytes


	memoriaCompartida_entrada.openShareMemory();
	memoriaCompartida_salida.openShareMemory();

	//hilo UDP


	hThreadRecv = CreateThread(NULL,0,SocketRecvThread, NULL, 0, &dwThreadID1);
		
	if (hThreadRecv == NULL){
			MessageBox(NULL,"CreateThread::SocketRecvThread() Error", "Error",MB_OK);
		return 0;
	}else{
		if(!SetThreadPriorityBoost(hThreadRecv,TRUE)){
				MessageBox(NULL,"SetThreadPriorityBoost","Error",MB_OK);
		}
		if( !SetThreadPriority(hThreadRecv ,THREAD_PRIORITY_NORMAL)){
				MessageBox(NULL,"Set Prioridad del hilo Error TCP","Error",MB_OK);
		};

	}


		// Set the MMTIMER struct members for start the Mutimedia Timer */
	   g_mmTimer.uiPeriod = State.sample_time;  // Request of Period of the Multimedia Timer
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
		

	}else{
		MessageBox(NULL,TEXT("Presione stop"),TEXT("Error"),MB_OK);
	}

	return 1;
} // int StartAll(void)
//----------------------------------------------------------------------------//
void StopAll(HWND hWnd)
{
	State.iPacksRx=0;
	if(State.run==1){
	if ( mmTimerEnd(g_mmTimer) == FALSE) MessageBox(NULL,TEXT("Error mmTimerEnd"),TEXT("Error"),MB_OK);

	if( KillTimer(hWnd,1) ==0) MessageBox(NULL,TEXT("Error CloseHandle KillTimer"),TEXT("Error"),MB_OK);
	
    //Finalmente se cierran todas las memorias abiertas
	memoriaCompartida_entrada.closeShareMemory();
	memoriaCompartida_salida.closeShareMemory();

	State.run=0;
	}
} // StopAll() 





