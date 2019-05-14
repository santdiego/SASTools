// File: ShareMemory.h

#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE


#define M_COMPARTIDA  "HYPERIMU"

#define DOF_ENTRADA 1
#define DOF_SALIDA 12


typedef struct tag_VIRTUAL_DEVICE_ENTRADA {

	float Acciones[DOF_ENTRADA];


} VIRTUAL_DEVICE_ENTRADA;
typedef struct tag_VIRTUAL_DEVICE_SALIDA {

	float Estado[DOF_SALIDA];
	int PaquetesRx;
	float Sample_time;

} VIRTUAL_DEVICE_SALIDA;






#endif


