#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE

#include <strsafe.h>

#define M_COMPARTIDA  "IMAGEN_COMPRIMIDA_RECIBIR"
#define TAM_MAX	60000

typedef struct tag_VIRTUAL_DEVICE_CONTROL{

	bool signalWrite;

} VIRTUAL_DEVICE_CONTROL;

typedef struct tag_VIRTUAL_DEVICE_ENTRADA{

	unsigned char	ucImagenComp[TAM_MAX];
	int				iCantBytesImagenComp;

} VIRTUAL_DEVICE_ENTRADA;

#endif