#ifndef TISHAREDMEMORY_H
#define TISHAREDMEMORY_H
#include <windows.h>
#include <string.h>
const int SHARED_NAME_SIZE = 256;

template <class StructType>
class CSharedStruct
{
private:
    HANDLE m_hFileMapping;
    char   m_hSharedName[SHARED_NAME_SIZE];
    DWORD  m_dwMaxDataSize;
    StructType *m_pvData;

    BOOL m_bCreated;
    BOOL m_bNamed;

public:
    CSharedStruct();
    CSharedStruct(char *Name);
    VOID Name(char *Name);
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
CSharedStruct<StructType>::CSharedStruct(char *Name)
{
    m_hFileMapping = NULL;
    m_pvData = NULL;
    strncpy(m_hSharedName, Name, SHARED_NAME_SIZE - 1);
    m_bNamed = true;
    m_dwMaxDataSize = sizeof(StructType);
}

template <class StructType>
VOID CSharedStruct<StructType>::Name(char *Name)
{
    strncpy(m_hSharedName, Name, SHARED_NAME_SIZE - 1);
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

class TISharedMEmory
{

public:
    TISharedMEmory();
};

#endif // TISHAREDMEMORY_H
