#ifndef ACCESS_H
#define ACCESS_H

#pragma once

#include <vector>
#include "ShareMemoryLib/CshareMemoryContainer.h"
#include "ShareMemoryLib/CeventContainer.h"
#ifdef UNICODE
#undef UNICODE
#endif

class CAccess
{
public:
    CAccess();
    virtual ~CAccess();

    static void createNonGui();
    static void destroyNonGui();

    static CShareMemoryContainer* shareMemoryContainer;
    static CEventContainer* eventContainer;
};

#endif // ACCESS_H
