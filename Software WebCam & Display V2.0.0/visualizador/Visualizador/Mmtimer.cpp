#include <Windows.h>
#include <tchar.h>
#include "mmtimer.h"
#pragma comment(lib,"Winmm.lib")

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

	// start the timed callback function
	if( pmmTimer->lpProc ){
		mmTimerID = timeSetEvent(pmmTimer->uiPeriod,0,pmmTimer->lpProc, pmmTimer->dwData, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		if( (UINT) mmTimerID == 0){
			MessageBox(NULL,_T("Error:mmTimerStart::timeSetEvent"),_T("Error"),MB_OK);
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
	return TRUE;
} 
//----------------------------------------------------------------------------//
