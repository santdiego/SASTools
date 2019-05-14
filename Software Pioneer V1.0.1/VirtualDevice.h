#include "ShareMemory.h"

#define CANTIDAD_PIONEER 5

BOOL OpenShareMemory(TCHAR *cpName, int i);
BOOL CloseShareMemory();
BOOL ReadShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData);
BOOL WriteShareMemory(VIRTUAL_DEVICE_SALIDA *pfData);