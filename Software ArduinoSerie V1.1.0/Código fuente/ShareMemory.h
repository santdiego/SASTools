// File: ShareMemory.h

#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE


#define M_COMPARTIDA  "ARDUINO"

#define DOF_ENTRADA 50
#define DOF_SALIDA 50


typedef struct tag_VIRTUAL_DEVICE_ENTRADA {

	float Acciones[DOF_ENTRADA+1];

} VIRTUAL_DEVICE_ENTRADA;

typedef struct tag_VIRTUAL_DEVICE_SALIDA {

	float Estado[DOF_SALIDA+1];

} VIRTUAL_DEVICE_SALIDA;


#endif


