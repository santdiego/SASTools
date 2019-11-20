#pragma once

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

///ESTRUCTURA QUE FORMAN LA MEMORIA VIRTUAL//////
#define M_COMPARTIDA_HOKUYO TEXT("LASER")
const int NUMREADINGS = 683;
const int SHARED_NAME_SIZE = 256;
const int minDistance = 200;
const int maxDistance = 4000;
//20mm ~ 4000mm

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