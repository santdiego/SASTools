#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <tchar.h>
#include <strsafe.h>
#include "VirtualDevice.h"

static VIRTUAL_DEVICE_ENTRADA *pVirtualDevice_entrada;
static HANDLE hShare_entrada;

static VIRTUAL_DEVICE_CONTROL *pVirtualDevice_control;
static HANDLE hShare_control;

BOOL OpenShareMemory(TCHAR *cpName, int i)
{
			TCHAR nombre_entrada[256];

			if(i<0 || i>CANTIDAD_WEBCAM-1){
				MessageBox(NULL,TEXT("Número de dispositivo incorrecto"), TEXT("Error"),MB_OK);
				return FALSE;
			}

			StringCchPrintf( nombre_entrada, 128, TEXT("%s_entrada_%i"),cpName,i);

			if( pVirtualDevice_entrada != NULL){
				MessageBox(NULL,TEXT("Memoria compartida actualmente abierta"), TEXT("Error"),MB_OK);
				return FALSE;
			}
	
			hShare_entrada = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, 
				PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_ENTRADA), nombre_entrada);

			if(hShare_entrada == NULL){
				MessageBox(NULL,TEXT("Error:Open Virtual Device::CreateFileMapping"),TEXT("Error"),MB_OK);
				return FALSE;
			}else{
				pVirtualDevice_entrada = (VIRTUAL_DEVICE_ENTRADA *) MapViewOfFile(hShare_entrada,FILE_MAP_ALL_ACCESS, 
					0, 0, sizeof(VIRTUAL_DEVICE_ENTRADA));
			}
			if(pVirtualDevice_entrada == NULL){
				MessageBox(NULL,TEXT("Error: Open Virtual Device::MapViewOfFile"),TEXT("Error"),MB_OK);
				return FALSE;
			}

	return TRUE;
}

BOOL CloseShareMemory()
{
	if(pVirtualDevice_entrada != NULL) {
		if( FlushViewOfFile(pVirtualDevice_entrada, sizeof(VIRTUAL_DEVICE_ENTRADA)) == 0){
			MessageBox(NULL,TEXT("Error:Close Virtual Device::FlushViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pVirtualDevice_entrada) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::UnmapViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		if( CloseHandle(hShare_entrada) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::CloseHandle"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		pVirtualDevice_entrada = NULL;
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


BOOL OpenControlShareMemory(TCHAR *cpName, int i)
{
			TCHAR nombre_control[256];
			
			if(i<0 || i>CANTIDAD_WEBCAM-1){
				MessageBox(NULL,TEXT("Número de dispositivo incorrecto"), TEXT("Error"),MB_OK);
				return FALSE;
			}

			StringCchPrintf( nombre_control, 128, TEXT("%s_control_%i"),cpName,i);

			if( pVirtualDevice_control != NULL){
				MessageBox(NULL,TEXT("Memoria compartida actualmente abierta"), TEXT("Error"),MB_OK);
				return FALSE;
			}
	
			hShare_control = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, 
				PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_CONTROL), nombre_control);

			if(hShare_control == NULL){
				MessageBox(NULL,TEXT("Error:Open Virtual Device::CreateFileMapping"),TEXT("Error"),MB_OK);
				return FALSE;
			}else{
				pVirtualDevice_control = (VIRTUAL_DEVICE_CONTROL *) MapViewOfFile(hShare_control,FILE_MAP_ALL_ACCESS, 
					0, 0, sizeof(VIRTUAL_DEVICE_CONTROL));
			}
			if(pVirtualDevice_control == NULL){
				MessageBox(NULL,TEXT("Error: Open Virtual Device::MapViewOfFile"),TEXT("Error"),MB_OK);
				return FALSE;
			}

	return TRUE;
}

BOOL CloseControlShareMemory()
{
	if(pVirtualDevice_control != NULL) {
		if( FlushViewOfFile(pVirtualDevice_control, sizeof(VIRTUAL_DEVICE_CONTROL)) == 0){
			MessageBox(NULL,TEXT("Error:Close Virtual Device::FlushViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pVirtualDevice_control) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::UnmapViewOfFile"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		if( CloseHandle(hShare_control) == 0){
			MessageBox(NULL,TEXT("Error: Close Virtual Device::CloseHandle"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	
		pVirtualDevice_control = NULL;
		return TRUE;
	}else{
		MessageBox(NULL,TEXT("Error: THE SHARE MEMORY WAS ALREADY CLOSE"),TEXT("Error"),MB_OK);
		return FALSE;
	}
}

BOOL ReadControlShareMemory(VIRTUAL_DEVICE_CONTROL *pfData)
{
	if(pVirtualDevice_control == NULL){
		MessageBox(NULL,TEXT("Error:Read Virtual Device CANT FIND THE SHARE MEMORY"),TEXT("Error"),MB_OK);
		return FALSE;
	}
	CopyMemory((VIRTUAL_DEVICE_CONTROL *)pfData, (VIRTUAL_DEVICE_CONTROL *)pVirtualDevice_control, sizeof(VIRTUAL_DEVICE_CONTROL));
return TRUE;
}