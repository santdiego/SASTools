#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE

#include <strsafe.h>

#define M_COMPARTIDA  "IMAGEN_COMPRIMIDA"
#define TAM_MAX	32000

typedef struct tag_VIRTUAL_DEVICE_SALIDA{

	unsigned char	ucImagenComp[TAM_MAX];
	int				iCantBytesImagenComp;

} VIRTUAL_DEVICE_SALIDA;

/*typedef struct tag_VIRTUAL_DEVICE_ENTRADA{

	unsigned char	ucImagenComp[TAM_MAX];
	int				iCantBytesImagenComp;

} VIRTUAL_DEVICE_ENTRADA;*/

#endif