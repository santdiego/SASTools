#ifndef DEVICE_H_FILE
	#define DEVICE_H_FILE

#define DEFAULT_TIEMPO_MUESTREO_MEMORIA 50	//Tiempo en [ms] para escribir y leer en memoria
#define PI  3.14159265359

typedef struct tag_CONTROL 
{ 
	int P_Lin;
	int I_Lin;
	int D_Lin;
	int P_Rot;
	int I_Rot;
	int D_Rot;
	double Postura[3];

} CONTROL;

typedef struct tag_STATE 
{ 
	double dPose[3];
	double dVelocity[2];
	int iParametros[6];
	unsigned int iLaser[181];
	TCHAR mensaje[32];
	bool run;

} STATE;

#endif