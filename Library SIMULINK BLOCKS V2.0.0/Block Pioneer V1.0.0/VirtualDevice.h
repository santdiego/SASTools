#include "ShareMemory.h"

#define CANTIDAD_PIONEER 5

BOOL OpenShareMemory(TCHAR *cpName, int i);
BOOL CloseShareMemory(int i);
BOOL ReadShareMemory(VIRTUAL_DEVICE_SALIDA *pfData, int i);
BOOL WriteShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData, int i);