//defines de sample time of the network

#include "ShareMemory.h"

//periodo de muestro entre cliente/servidor en comunicacion peer-to-peer UDP/IP
#define SAMPLE_TIME 10

#define MASCOT_BUFFSIZE	32768*2



DWORD WINAPI SocketRecvThread(LPVOID lpParameter);



typedef struct tag_NETPACK_TO_CLIENTE{
	
		  char celular[100];

} NETPACK_TO_CLIENTE;


typedef struct structWVSTATE 
{ 

	char textoA[30];
	char tiempo[30];
	char mensaje[30];
	int iPacksRx;
	float m_dDurationRx;
	DWORD m_dwFrec;
	LARGE_INTEGER liFrec;
	DWORD bytesRecv;
	char cpBufferIPlocal[30];
	bool run;
	float IMU[DOF_SALIDA];
	float Acciones[DOF_ENTRADA];
	NETPACK_TO_CLIENTE sensores;
	USHORT puerto;
	USHORT sample_time;
	USHORT device;


} WV_STATE;

extern WV_STATE State;
extern NETPACK_TO_CLIENTE datos_hacia_cliente;


void CALLBACK Interrupt(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5);
int StartAll(HWND hWnd);
void StopAll(HWND hWnd);
void InitState();


