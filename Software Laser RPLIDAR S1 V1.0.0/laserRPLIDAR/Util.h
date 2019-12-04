#pragma once
#include <Windows.h>
#include <thread>
#include <stdio.h>
#include <io.h>
#include <tchar.h>
#include <chrono>
#include <iostream>
#include <inttypes.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header
#include <signal.h>

#pragma comment(lib, "rplidar_driver.lib")

///ESTRUCTURA QUE FORMAN LA MEMORIA VIRTUAL//////
#define M_COMPARTIDA_RPLIDAR TEXT("RPLIDAR")
#define DEFAULT_PORT "\\\\.\\COM16"
#define DEFAULT_TIEMPO_MUESTREO 100
#define DEFAULT_BAUDRATE 256000
#define DEFAULT_MODE 1   //DenseBoost
#define DEFAULT_RPM 600
const int MAXREADINGS = 1200;
const int SHARED_NAME_SIZE = 256;

// Global Variables:
void laser_func();
void CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired); //Función del Timer (muestreo)//
bool stop_signal;
volatile sig_atomic_t ctrl_c_pressed = 0;
volatile sig_atomic_t window_cross_pressed = 0;
HANDLE EventoTimer;//Evento q se señaliza cada periodo de muestreo//
char comPort[14] = DEFAULT_PORT;
_u32 baudrate = DEFAULT_BAUDRATE;
_u8 ts = DEFAULT_TIEMPO_MUESTREO;
_u16 mode = DEFAULT_MODE;
_u16 rpm = DEFAULT_RPM;

template <class StructType>
class CSharedStruct
{
private:
	HANDLE m_hFileMapping;
	TCHAR   m_hSharedName[SHARED_NAME_SIZE];
	DWORD  m_dwMaxDataSize;
	StructType *m_pvData;

	BOOL m_bCreated;
	BOOL m_bNamed;

public:
	CSharedStruct();
	CSharedStruct(const TCHAR *Name);
	VOID Name(const TCHAR *Name);
	VOID Release();
	BOOL Acquire();

	StructType *operator->();
};


template <class StructType>
StructType *CSharedStruct<StructType>::operator->()
{
	return m_pvData;
}

template <class StructType>
CSharedStruct<StructType>::CSharedStruct()
{
	m_hFileMapping = NULL;
	m_pvData = NULL;
	m_bNamed = false;
	m_dwMaxDataSize = sizeof(StructType);
}

template <class StructType>
CSharedStruct<StructType>::CSharedStruct(const TCHAR *Name)
{
	m_hFileMapping = NULL;
	m_pvData = NULL;
	_tcscpy_s(m_hSharedName, SHARED_NAME_SIZE - 1, Name);
	m_bNamed = true;
	m_dwMaxDataSize = sizeof(StructType);
}

template <class StructType>
VOID CSharedStruct<StructType>::Name(const TCHAR *Name)
{
	_tcscpy_s(m_hSharedName, SHARED_NAME_SIZE - 1, Name);
	m_bNamed = true;
}

template <class StructType>
VOID CSharedStruct<StructType>::Release()
{
	if (m_pvData)
	{
		UnmapViewOfFile(m_pvData);
	}

	if (m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
	}
}

template <class StructType>
BOOL CSharedStruct<StructType>::Acquire()
{
	m_dwMaxDataSize = 0;
	if (m_bNamed)
	{
		m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
			PAGE_READWRITE, 0, sizeof(StructType), m_hSharedName);
	}
	else
		return FALSE;

	if (m_hFileMapping == NULL)
	{
		return FALSE;
	}

	m_pvData = (StructType *)MapViewOfFile(m_hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	if (m_pvData == NULL)
	{
		CloseHandle(m_hFileMapping);
		return FALSE;
	}

	return TRUE;
}

typedef struct tag_LaserData {
	float	 angle_deg[MAXREADINGS];
	float	 distance_m[MAXREADINGS];
	uint8_t	 quality[MAXREADINGS];
	uint16_t count;  //Número de valores leídos que forman una vuelta completa 0-360 (no es constante)
}LaserData;