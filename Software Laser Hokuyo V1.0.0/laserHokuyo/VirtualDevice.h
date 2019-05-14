#ifndef VIRTUALDEVICE_H_FILE
	#define VIRTUALDEVICE_H_FILE

	#include "ShareMemory.h"

		BOOL OpenSharedMemory(TCHAR *cpName);
		BOOL CloseSharedMemory(TCHAR *cpName);
		BOOL WriteShareMemory(TCHAR *cpName, VIRTUAL_DEVICE_OUT *pfData);

#endif