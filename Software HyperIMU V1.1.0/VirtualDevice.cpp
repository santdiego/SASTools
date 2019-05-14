#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <strsafe.h>
#include "VirtualDevice.h"
#include "ShareMemory.h"

static VIRTUAL_DEVICE_ENTRADA *pVirtualDevice_entrada;
static HANDLE hShare_entrada;
static VIRTUAL_DEVICE_SALIDA *pVirtualDevice_salida;
static HANDLE hShare_salida;

BOOL OpenShareMemory(TCHAR *cpName, int i)
{
			TCHAR nombre_entrada[256];
			TCHAR nombre_salida[256];
			if(i<0 || i>CANTIDAD-1){
				MessageBox(NULL,TEXT("Número de dispositivo incorrecto"), TEXT("Error"),MB_OK);
				return FALSE;
			}

			StringCchPrintf( nombre_entrada, 128, TEXT("%s_entrada_%i"),cpName,i); 
			StringCchPrintf( nombre_salida, 128, TEXT("%s_salida_%i"),cpName,i); 

			if( (pVirtualDevice_salida != NULL) || (pVirtualDevice_entrada != NULL)){
				MessageBox(NULL,TEXT("Memoria compartida actualmente abierta"), TEXT("Error"),MB_OK);
				return FALSE;
			}
	
			hShare_salida = CreateFileMapping( INVALID_HANDLE_VALUE,NULL, 
				PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_SALIDA), nombre_salida);

			hShare_entrada = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, 
				PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_ENTRADA), nombre_entrada);

			if(hShare_salida == NULL || hShare_entrada==NULL){
				MessageBox(NULL,TEXT("Error:Open Virtual Device::CreateFileMapping"),TEXT("Error"),MB_OK);
				return FALSE;
			}else{
				pVirtualDevice_salida = (VIRTUAL_DEVICE_SALIDA *) MapViewOfFile(hShare_salida,FILE_MAP_ALL_ACCESS, 
					0, 0, sizeof(VIRTUAL_DEVICE_SALIDA));
				pVirtualDevice_entrada = (VIRTUAL_DEVICE_ENTRADA *) MapViewOfFile(hShare_entrada,FILE_MAP_ALL_ACCESS, 
					0, 0, sizeof(VIRTUAL_DEVICE_ENTRADA));
			}
			if(pVirtualDevice_salida == NULL || pVirtualDevice_entrada == NULL){
				MessageBox(NULL,TEXT("Error: Open Virtual Device::MapViewOfFile"),TEXT("Error"),MB_OK);
				return FALSE;
			}

	return TRUE;
}

BOOL CloseShareMemory()
{
	if((pVirtualDevice_entrada != NULL) || (pVirtualDevice_salida != NULL)) {
		if( FlushViewOfFile(pVirtualDevice_entrada, sizeof(VIRTUAL_DEVICE_ENTRADA)) == 0 || FlushViewOfFile(pVirtualDevice_salida, sizeof(VIRTUAL_DEVICE_SALIDA)) == 0){
			MessageBox(NULL,TEXT("Error:Close Virtual Device::FlushViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pVirtualDevice_entrada) == 0 || UnmapViewOfFile(pVirtualDevice_salida) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::UnmapViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		if( CloseHandle(hShare_entrada) == 0 || CloseHandle(hShare_salida) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::CloseHandle"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		pVirtualDevice_entrada = NULL;
		pVirtualDevice_salida = NULL;
		return TRUE;
	}else{
		MessageBox(NULL,TEXT("Error: THE SHARE MEMORY WAS ALREADY CLOSE"),TEXT("Error"),MB_OK);
		return FALSE;
	}
}

BOOL ReadShareMemory(VIRTUAL_DEVICE_ENTRADA *pfData)
{

	if(pVirtualDevice_entrada == NULL){
		MessageBox(NULL,TEXT("Error:Read Virtual Device CANT FIND THE SHARE MEMORY"),TEXT("Error"),MB_OK);
		return FALSE;
	}
	CopyMemory((VIRTUAL_DEVICE_ENTRADA *)pfData, (VIRTUAL_DEVICE_ENTRADA *)pVirtualDevice_entrada, sizeof(VIRTUAL_DEVICE_ENTRADA));

return TRUE;
}

BOOL WriteShareMemory(VIRTUAL_DEVICE_SALIDA *pfData)
{

	CopyMemory((VIRTUAL_DEVICE_SALIDA *)pVirtualDevice_salida,(VIRTUAL_DEVICE_SALIDA *)pfData, sizeof(VIRTUAL_DEVICE_SALIDA));


	if( FlushViewOfFile(pVirtualDevice_salida, sizeof(VIRTUAL_DEVICE_SALIDA)) == 0){
		MessageBox(NULL,TEXT("Error: Close Virtual Device::FlushViewOfFile"),TEXT("Error"),MB_OK);
		return FALSE;
	}
	return TRUE;
}