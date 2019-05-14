#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE

#include <strsafe.h>

#define M_COMPARTIDA  "PIONEER"

#define DOF_ENTRADA 2
#define DOF_SALIDA 5
#define NUM_PARAM 6
#define NUM_LASER 181

typedef struct tag_VIRTUAL_DEVICE_ENTRADA {

	double Acciones[DOF_ENTRADA];
	double Postura[3];
	int Parametros[NUM_PARAM];
	bool Actualizar_parametros;
	bool Actualizar_postura;

} VIRTUAL_DEVICE_ENTRADA;
typedef struct tag_VIRTUAL_DEVICE_SALIDA {

	double Estado[DOF_SALIDA];
	int Parametros[NUM_PARAM];
	unsigned int MedidasLaser[NUM_LASER];

} VIRTUAL_DEVICE_SALIDA;


#endif