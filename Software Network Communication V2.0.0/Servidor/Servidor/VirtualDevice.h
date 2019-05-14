#ifndef VIRTUALDEVICE_H_FILE
	#define VIRTUALDEVICE_H_FILE

	#include "ShareMemory.h"

		BOOL OpenPairSharedMemory(TCHAR *cpName);
		BOOL ClosePairSharedMemory(TCHAR *cpName);
		BOOL ReadShareMemory(TCHAR *cpName, VIRTUAL_DEVICE_IN *pfData);
		BOOL WriteShareMemory(TCHAR *cpName, VIRTUAL_DEVICE_OUT *pfData);

		BOOL NetOpenPairSharedMemory(TCHAR *cpName);
		BOOL NetClosePairSharedMemory(TCHAR *cpName);
		BOOL NetReadShareMemory(TCHAR *cpName, NET_VIRTUAL_DEVICE_IN *pfData);
		BOOL NetWriteShareMemory(TCHAR *cpName, NET_VIRTUAL_DEVICE_OUT *pfData);

#endif