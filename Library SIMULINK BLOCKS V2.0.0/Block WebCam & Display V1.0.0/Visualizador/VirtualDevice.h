#include "ShareMemory.h"

#define CANTIDAD_WEBCAM 1

BOOL OpenShareMemory(TCHAR *cpName, int i);
BOOL CloseShareMemory();
BOOL WriteShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData);

BOOL OpenControlShareMemory(TCHAR *cpName, int i);
BOOL CloseControlShareMemory();
BOOL WriteControlShareMemory(VIRTUAL_DEVICE_CONTROL *pfData);