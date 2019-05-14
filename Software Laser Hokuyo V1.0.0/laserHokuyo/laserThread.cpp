#include "stdafx.h"
#include "laserThread.h"
#include "urg_sensor.h"
#include "urg_utils.h"
#include "ShareMemory.h"
#include "VirtualDevice.h"

HANDLE hHiloHokuyo; 
bool conectado=true;
int puerto;
VIRTUAL_DEVICE_OUT pfDataOut;

DWORD WINAPI laserThread(LPVOID lpParameter){
	
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);

	urg_t urg;
    int ret;
    long *distance_mm;
	const long connect_baudrate = 115200;
	const char connect_device[] = "COM3";
	ret = urg_open(&urg, URG_SERIAL, connect_device, connect_baudrate);

	if(ret)
	{
		system("cls");
		MessageBox(NULL,_T("Error al conectarse al laser...saliendo...\n"),_T(""),MB_OK);
		conectado = false;
		return -1;
	}

	 distance_mm = (long *)malloc(sizeof(long) *urg_max_data_size(&urg));
    // \todo check distance_mm is not NULL

	urg_set_scanning_parameter(&urg, urg_deg2step(&urg, -120), urg_deg2step(&urg, +120), 0);
    urg_start_measurement(&urg, URG_DISTANCE, URG_SCAN_INFINITY, 0);

	int n;
	int i;
	float angle_rad[MAXNUMFLOAT];
	float angle_deg[MAXNUMFLOAT];

	n = urg_get_distance(&urg, distance_mm, NULL);

	for (i = 0; i < n; ++i) {
		angle_rad[i] =  (float)urg_index2rad(&urg, i);
		angle_deg[i] =  (float)urg_index2deg(&urg, i);
    }

	memcpy(pfDataOut.angle_rad, angle_rad, n*sizeof(float));
	memcpy(pfDataOut.angle_deg, angle_deg, n*sizeof(float));

	while(conectado)
	{
		n = urg_get_distance(&urg, distance_mm, NULL);
		for (i = 0; i < n; ++i) {
			if((distance_mm[i]<20)||(distance_mm[i]>4000)) distance_mm[i]=4500;
			pfDataOut.distance_m[i]=float(distance_mm[i])/1000.0f;
		}
		
		WriteShareMemory(MEM1, &pfDataOut);

	}

	// 
    urg_close(&urg);

	return 0;
}