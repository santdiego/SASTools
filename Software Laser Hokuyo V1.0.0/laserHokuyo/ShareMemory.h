// File: ShareMemory.h

#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE

#define MAXNUMFLOAT 683
#define MEM1 _T("MemoriaHokuyo1")
#define numMemory 1

typedef struct tag_VIRTUAL_DEVICE_OUT {

	float angle_deg[MAXNUMFLOAT];
	float angle_rad[MAXNUMFLOAT];
	float distance_m[MAXNUMFLOAT];

} VIRTUAL_DEVICE_OUT;

#endif


