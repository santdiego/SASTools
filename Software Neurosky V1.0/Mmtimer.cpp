#include <windows.h>
#include "mmtimer.h"


//----------------------------------------------------------------------------//
//	FUNCTION mmTimerStart:
//	Init the multimedia timer.
//	MMTIMER *pmmTimer:
//	MMTIMER struct pointer (in/out)
//	return BOOL:
//		FALSE = Error
//		TRUE  = Ok
//----------------------------------------------------------------------------//
BOOL mmTimerStart(MMTIMER *pmmTimer)
{
	MMRESULT mmTimerID=0;
	TIMECAPS tcTimerInfo;
	// create the Multimedia Timer
	// get the minimum resolution
	if( timeGetDevCaps(&tcTimerInfo, sizeof(TIMECAPS)) != TIMERR_NOERROR){
		MessageBox(NULL,TEXT("Error:mmTimerStart::timeGetDevCaps"),TEXT("Error"),MB_OK);
		return FALSE;
	}

	// set the resolution
	//if( pmmTimer->uiPeriod < tcTimerInfo.wPeriodMin){
		//pmmTimer->uiPeriod = tcTimerInfo.wPeriodMin;
	//}
	//if( timeBeginPeriod(pmmTimer->uiPeriod) != TIMERR_NOERROR ){
		//MessageBox(NULL,"Error:mmTimerStart::timeBeginPeriod","Error",MB_OK);
		//return FALSE;
	//}
	// Try set the timer with Callback function
	// start the timed callback function
	if( pmmTimer->lpProc ){
		mmTimerID = timeSetEvent(pmmTimer->uiPeriod,1,pmmTimer->lpProc, pmmTimer->dwData, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		if( (UINT) mmTimerID == 0){
			MessageBox(NULL,TEXT("Error:mmTimerStart::timeSetEvent"),TEXT("Error"),MB_OK);
			return FALSE;
		}
	}
	if(mmTimerID){
		pmmTimer->mmTimerID = mmTimerID;
		return TRUE;
	}
	return FALSE;
}/* mmTimerStart function*/
//----------------------------------------------------------------------------//
// FUNTION mmTimerEnd
//	End the multimedia timer.
//	MMTIMER mmTimer:
//	MMTIMER struct (in)
//----------------------------------------------------------------------------//
BOOL mmTimerEnd(MMTIMER mmTimer)
{
	if( timeKillEvent(mmTimer.mmTimerID) != TIMERR_NOERROR ){
		//MessageBox(NULL,"Error:mmTimerEnd::timeKillEvent","Error",MB_OK);
		return FALSE;
	}
	/*if( timeEndPeriod(mmTimer.uiPeriod) != TIMERR_NOERROR ){
		MessageBox(NULL,"Error:mmTimerEnd::timeEndPeriod","Error",MB_OK);
		return FALSE;
	}*/
	return TRUE;
} 
//----------------------------------------------------------------------------//

