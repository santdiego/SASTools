#define MAX_MEMORIES 1
#define SHARE_MEMORY_BASE_NAME "boschShareMemory"

//Necesarias para la s-funciton
#define NUM_INPUT 1 //Numero de entradas al bloque
#define NUM_OUTPUT 1 //Numero de salidas del bloque
#define NUM_PARAM 3 //Numero de parametros del bloque
//3 parametros: 1º numero de memoria, 2ºtamaño de entrada, 3ºtamaño de salida

#define TYP_IN1  SS_DOUBLE
#define TYP_OUT1 SS_DOUBLE

//Propias de la aplicacion
#define MAX_SIZE 500 //Tamaño de la memoria de entrada y de salida

typedef struct tag_VIRTUAL_DEVICE_INPUT {
	double jointVelocity[4];
} VIRTUAL_DEVICE_INPUT;

typedef struct tag_VIRTUAL_DEVICE_OUTPUT {
	double jointPosition[4];
} VIRTUAL_DEVICE_OUTPUT;
