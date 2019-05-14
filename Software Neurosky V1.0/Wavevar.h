//defines de sample time of the network

#include "ShareMemory.h"

#define SAMPLE_TIME 1
#define Nida2 50
#define Nvuelta2 50


typedef struct structCONTROL 
{ 

	double dK[6];


} CONTROL;

typedef struct structSTATE 
{ 
	LARGE_INTEGER liFrec;
	char textoA[30];
	float dSensor[Nvuelta2];
	float dAccion[Nida2];
	double dSampleTime;
	DWORD m_dwFrec;
	TCHAR mensaje[32];
	bool run;
	int iPacksTx_dspic;
	int iPacksRx_dspic;
	int bandera_encabezado;
	int muestreo;
	int baud_rate;
	int puerto_com;
	int iDispositivo;
	int Nida1;
	int Nvuelta1;
	double dDurationRx_dspic;
	double dDurationTx_dspic;
	double dDuration_timer;

}STATE;

extern CONTROL Control;
extern STATE State;


void CALLBACK Interrupt(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5);
int StartAll(HWND hWnd);
void StopAll(HWND hWnd);
void InitState();



