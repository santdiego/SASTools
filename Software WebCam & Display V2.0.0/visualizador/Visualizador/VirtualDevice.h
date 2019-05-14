#include "ShareMemory.h"

#define CANTIDAD_WEBCAM 1

BOOL OpenShareMemory(TCHAR *cpName, int i);
BOOL CloseShareMemory();
BOOL ReadShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData);

BOOL OpenControlShareMemory(TCHAR *cpName, int i);
BOOL CloseControlShareMemory();
BOOL ReadControlShareMemory(VIRTUAL_DEVICE_CONTROL *pfData);