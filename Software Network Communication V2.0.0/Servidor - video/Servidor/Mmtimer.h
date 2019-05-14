#ifndef MMTIMER_H_FILE
	#define MMTIMER_H_FILE
	
	#include <Mmsystem.h>
	
	typedef struct tag_MMTIMER {
		MMRESULT mmTimerID;
		UINT uiPeriod;
		LPTIMECALLBACK lpProc;
		DWORD dwData;
		HANDLE hEvent;
	} MMTIMER;

	BOOL mmTimerStart(MMTIMER *phtTimer);
	BOOL mmTimerEnd(MMTIMER htTimer);

#endif