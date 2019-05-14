// File: ShareMemory.h

#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE


#define M_COMPARTIDA  "NOVINT"

#define DOF_ENTRADA 3
#define DOF_SALIDA 6
#define NUM_PARAM 6
#define NUM_BOTONES 4

typedef struct tag_VIRTUAL_DEVICE_ENTRADA {

	double Acciones[DOF_ENTRADA];
	double Parametros[NUM_PARAM];
	bool Actualizar_parametros;

} VIRTUAL_DEVICE_ENTRADA;
typedef struct tag_VIRTUAL_DEVICE_SALIDA {

	double Estado[DOF_SALIDA];
	bool Botones[NUM_BOTONES];

} VIRTUAL_DEVICE_SALIDA;


#endif


