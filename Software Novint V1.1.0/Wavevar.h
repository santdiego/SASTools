//defines de sample time of the network

#include "ShareMemory.h"

#define SAMPLE_TIME 1

typedef struct structCONTROL 
{ 

	double dSpring[3];
	double dDamping[3];
	int iDispositivo;


} CONTROL;

typedef struct structSTATE 
{ 
	LARGE_INTEGER liFrec;
	double dPosition[3];
	double dForce[3];
	bool dButtons[4];
	double dVelocity[3];
	double dSampleTime;
	DWORD m_dwFrec;
	TCHAR mensaje[32];
	bool run;

} STATE;

extern CONTROL Control;
extern STATE State;


void CALLBACK Interrupt(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5);
int StartAll(HWND hWnd);
void StopAll(HWND hWnd);
void InitState();


