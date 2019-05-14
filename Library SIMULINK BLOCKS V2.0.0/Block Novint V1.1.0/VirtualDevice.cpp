#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <strsafe.h>
#include "VirtualDevice.h"

static VIRTUAL_DEVICE_ENTRADA *pVirtualDevice_entrada[CANTIDAD_NOVINT];
static HANDLE hShare_entrada[CANTIDAD_NOVINT];
static VIRTUAL_DEVICE_SALIDA *pVirtualDevice_salida[CANTIDAD_NOVINT];
static HANDLE hShare_salida[CANTIDAD_NOVINT];

BOOL OpenShareMemory(TCHAR *cpName, int i)
{
			TCHAR nombre_entrada[256];
			TCHAR nombre_salida[256];
			if(i<0 || i>CANTIDAD_NOVINT-1){
				MessageBox(NULL,TEXT("Número de dispositivo incorrecto"), TEXT("Error"),MB_OK);
				return FALSE;
			}

			StringCchPrintf( nombre_entrada, 128, TEXT("%s_entrada_%i"),cpName,i); 
			StringCchPrintf( nombre_salida, 128, TEXT("%s_salida_%i"),cpName,i); 

			if( (pVirtualDevice_salida[i] != NULL) || (pVirtualDevice_entrada[i] != NULL)){
				MessageBox(NULL,TEXT("Memoria compartida actualmente abierta"), TEXT("Error"),MB_OK);
				return FALSE;
			}
	
			hShare_salida[i] = CreateFileMapping( INVALID_HANDLE_VALUE,NULL, 
				PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_SALIDA), nombre_salida);

			hShare_entrada[i] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, 
				PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_ENTRADA), nombre_entrada);

			if(hShare_salida[i] == NULL || hShare_entrada[i] ==NULL){
				MessageBox(NULL,TEXT("Error:Open Virtual Device::CreateFileMapping"),TEXT("Error"),MB_OK);
				return FALSE;
			}else{
				pVirtualDevice_salida[i] = (VIRTUAL_DEVICE_SALIDA *) MapViewOfFile(hShare_salida[i],FILE_MAP_ALL_ACCESS, 
					0, 0, sizeof(VIRTUAL_DEVICE_SALIDA));
				pVirtualDevice_entrada[i] = (VIRTUAL_DEVICE_ENTRADA *) MapViewOfFile(hShare_entrada[i],FILE_MAP_ALL_ACCESS, 
					0, 0, sizeof(VIRTUAL_DEVICE_ENTRADA));
			}
			if(pVirtualDevice_salida[i] == NULL || pVirtualDevice_entrada[i] == NULL){
				MessageBox(NULL,TEXT("Error: Open Virtual Device::MapViewOfFile"),TEXT("Error"),MB_OK);
				return FALSE;
			}

	return TRUE;
}

BOOL CloseShareMemory(int i)
{
	if((pVirtualDevice_entrada[i] != NULL) || (pVirtualDevice_salida[i] != NULL)) {
		if( FlushViewOfFile(pVirtualDevice_entrada[i], sizeof(VIRTUAL_DEVICE_ENTRADA)) == 0 || FlushViewOfFile(pVirtualDevice_salida[i], sizeof(VIRTUAL_DEVICE_SALIDA)) == 0){
			MessageBox(NULL,TEXT("Error:Close Virtual Device::FlushViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pVirtualDevice_entrada[i]) == 0 || UnmapViewOfFile(pVirtualDevice_salida[i]) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::UnmapViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		if( CloseHandle(hShare_entrada[i]) == 0 || CloseHandle(hShare_salida[i]) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::CloseHandle"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		pVirtualDevice_entrada[i] = NULL;
		pVirtualDevice_salida[i] = NULL;
		return TRUE;
	}else{
		MessageBox(NULL,TEXT("Error: THE SHARE MEMORY WAS ALREADY CLOSE"),TEXT("Error"),MB_OK);
		return FALSE;
	}
}

BOOL ReadShareMemory(VIRTUAL_DEVICE_SALIDA *pfData, int i)
{

	if(pVirtualDevice_salida[i] == NULL){
		MessageBox(NULL,TEXT("Error:Read Virtual Device CANT FIND THE SHARE MEMORY"),TEXT("Error"),MB_OK);
		return FALSE;
	}
	CopyMemory((VIRTUAL_DEVICE_SALIDA *)pfData, (VIRTUAL_DEVICE_SALIDA *)pVirtualDevice_salida[i], sizeof(VIRTUAL_DEVICE_SALIDA));

return TRUE;
}

BOOL WriteShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData, int i)
{

	CopyMemory((VIRTUAL_DEVICE_ENTRADA *)pVirtualDevice_entrada[i],(VIRTUAL_DEVICE_ENTRADA *)pfData, sizeof(VIRTUAL_DEVICE_ENTRADA));


	if( FlushViewOfFile(pVirtualDevice_entrada[i], sizeof(VIRTUAL_DEVICE_ENTRADA)) == 0){
		MessageBox(NULL,TEXT("Error: Close Virtual Device::FlushViewOfFile"),TEXT("Error"),MB_OK);
		return FALSE;
	}
	return TRUE;
}