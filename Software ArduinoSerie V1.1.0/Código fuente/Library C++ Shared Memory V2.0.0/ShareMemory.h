// File: ShareMemory.h

#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE

#include <windows.h>
#include <mmsystem.h>



typedef struct tag_VIRTUAL_DEVICE_ARDRONE_OUTPUT{

	double dispositivo[20];

	//dispositivo[0][0] -> Battery level (1 -> 100%)
	//dispositivo[0][1] -> Phi en rad (roll)
	//dispositivo[0][2] -> Theta en rad (pitch)
	//dispositivo[0][3] -> Psi en rad (yaw)
	//dispositivo[0][4] -> Altitud (en metros)
	//dispositivo[0][5] -> Velocidad eje X (forward) (en m/s)
	//dispositivo[0][6] -> Velocidad eje Y (sideward) (en m/s)
	//dispositivo[0][7] -> Velocidad eje Z (en m/s)
	//dispositivo[0][8] -> Giróscopo[0] (en deg/s)
	//dispositivo[0][9] -> Giróscopo[1] (en deg/s)
	//dispositivo[0][10] -> Giróscopo[2] (en deg/s)
	//dispositivo[0][11] -> Acelerómetro[0] (en mili g)
	//dispositivo[0][12] -> Acelerómetro[1] (en mili g)
	//dispositivo[0][13] -> Acelerómetro[2] (en mili g)
	//dispositivo[0][14] -> Latitud (en deg)
	//dispositivo[0][15] -> Longitud (en deg)
	//dispositivo[0][16] -> Elevación (en metros)
	//dispositivo[0][17] -> WIFI signal quality (?)
	//dispositivo[0][18] -> Estado robot: emergencia=0; stop=1; start=2
	//dispositivo[0][19] -> Control manual o remoto
	

} VIRTUAL_DEVICE_ARDRONE_OUTPUT;

typedef struct tag_VIRTUAL_DEVICE_ARDRONE_INPUT{

	double dispositivo[4];


	
	//dispositivo[1][0] -> comando pitch (desde Matlab, entre -1 y 1, +-28.65 grados)
	//dispositivo[1][1] -> comando roll (desde Matlab, entre -1 y 1, +-28.65 grados)
	//dispositivo[1][2] -> comando gaz (desde Matlab, entre -1 y 1, +- 1 m/s)
	//dispositivo[1][3] -> comando yaw (desde Matlab, entre -1 y 1, +- 172 grados/segundo)
} VIRTUAL_DEVICE_ARDRONE_INPUT;



#endif


