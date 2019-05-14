#include "ShareMemory.h"

#define CANTIDAD_WEBCAM 1

BOOL OpenShareMemory(TCHAR *cpName, int i);
BOOL CloseShareMemory();
//BOOL ReadShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData);
BOOL ReadShareMemory(VIRTUAL_DEVICE_SALIDA *pfData);
//BOOL WriteShareMemory(LPBITMAPINFOHEADER ImagenComp, int CantBytes);