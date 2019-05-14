#include "ShareMemory.h"

#define CANTIDAD_NOVINT 4

BOOL OpenShareMemory(TCHAR *cpName, int i);
BOOL CloseShareMemory();
BOOL ReadShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData);
BOOL WriteShareMemory(VIRTUAL_DEVICE_SALIDA *pfData);






