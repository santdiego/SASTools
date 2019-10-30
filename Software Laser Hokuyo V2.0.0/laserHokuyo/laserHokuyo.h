#pragma once

#include <Windows.h>
#include <thread>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <tchar.h>
#include <chrono>
#include <iostream>
#include "..\urg_library-1.2.4\include\cpp\Urg_driver.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "urg_cpp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "setupapi.lib")

///ESTRUCTURA QUE FORMAN LA MEMORIA VIRTUAL//////
#define M_COMPARTIDA_HOKUYO TEXT("LASER")
#define DEFAULT_PORT "COM3"
#define DEFAULT_TIEMPO_MUESTREO 100
const int NUMREADINGS = 683;
const int SHARED_NAME_SIZE = 256;

// Global Variables:
void laser_func();
void CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired); //Función del Timer (muestreo)//
volatile bool stop_signal;
HANDLE EventoTimer;//Evento q se señaliza cada periodo de muestreo//
char comPort[6] = DEFAULT_PORT;
USHORT ts = DEFAULT_TIEMPO_MUESTREO;

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
	_tcscpy_s(m_hSharedName, SHARED_NAME_SIZE-1, Name);
	m_bNamed = true;
	m_dwMaxDataSize = sizeof(StructType);
}

template <class StructType>
VOID CSharedStruct<StructType>::Name(const TCHAR *Name)
{
	_tcscpy_s(m_hSharedName, SHARED_NAME_SIZE-1, Name);
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
	long distance_mm[NUMREADINGS];
	float angle_deg[NUMREADINGS];
	float angle_rad[NUMREADINGS];
}LaserData;