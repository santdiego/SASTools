#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <strsafe.h>

#include "resource.h"
#include "Device.h"

#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1

//Global instance handle
HINSTANCE hInstGlobal;

// Global Variables:
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szApplicationToolTip[MAX_LOADSTRING];	    // the main window class name

HWND hwndSetParam = NULL;
HWND hwndSetPose = NULL;
HWND hwndGetEstado = NULL;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK SetParamProc(HWND hwndDlg1, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SetPoseProc(HWND hwndDlg2, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GetEstadoProc(HWND hwndDlg3, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitWindow(HWND hWnd);
void EndWindow(HWND hWnd, HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

extern CONTROL Control;
extern STATE State;
extern bool setPose;
extern bool setParametros;
extern int nTcpPort;
extern void ErrorHandler(LPTSTR lpszFunction);
extern int StartAll(HWND hWnd);
extern void StopAll(HWND hWnd);

//----------------------------------------------------------------------------//
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpszCmdLine,int nCmdShow )
{

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SYSTRAYDEMO, szWindowClass, MAX_LOADSTRING);
	
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Main message loop:
	BOOL bRet;
  
	while ( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
			ErrorHandler(TEXT("GetMessage"));
		}
		else if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   HICON hMainIcon;

   hInstGlobal = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hMainIcon = LoadIcon(hInstance,(LPCTSTR)MAKEINTRESOURCE(IDI_ICON)); 

   nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
   nidApp.hWnd = (HWND) hWnd;              //handle of the window which will process this app. messages 
   nidApp.uID = IDI_ICON;           //ID of the icon that willl appear in the system tray 
   nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags 
   nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
   nidApp.uCallbackMessage = WM_USER_SHELLICON; 
   LoadString(hInstance, IDS_APPTOOLTIP,nidApp.szTip,MAX_LOADSTRING);
   Shell_NotifyIcon(NIM_ADD, &nidApp); 

   return TRUE;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	return RegisterClassEx(&wcex);
}

void EndWindow(HWND hWnd, HINSTANCE hInstance)
{
	CloseWindow(hWnd);
	DestroyWindow(hWnd);
	//UnregisterClass(szWindowClass,hInstance);
}

BOOL CALLBACK SetParamProc(HWND hwndDlg1, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch (message) { 
	case WM_ACTIVATE: 

		StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.P_Lin);
		SetDlgItemText(hwndDlg1,IDC_PLin, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.D_Lin);
		SetDlgItemText(hwndDlg1,IDC_DLin, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.I_Lin);
		SetDlgItemText(hwndDlg1,IDC_ILin, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.P_Rot);
		SetDlgItemText(hwndDlg1,IDC_PRot, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.D_Rot);
		SetDlgItemText(hwndDlg1,IDC_DRot, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.I_Rot);
		SetDlgItemText(hwndDlg1,IDC_IRot, State.mensaje);

		return TRUE; 
	case WM_COMMAND: 
		switch (LOWORD(wParam))  { 

		case IDCANCEL: 

			DestroyWindow(hwndDlg1); 
			hwndSetParam = NULL; 
			return TRUE;
		case IDC_SET: 

			#ifdef UNICODE
			GetDlgItemText(hwndDlg1,IDC_PLin, State.mensaje,sizeof(State.mensaje));
			Control.P_Lin=_wtoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_DLin, State.mensaje,sizeof(State.mensaje));
			Control.D_Lin=_wtoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_ILin, State.mensaje,sizeof(State.mensaje));
			Control.I_Lin=_wtoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_PRot, State.mensaje,sizeof(State.mensaje));
			Control.P_Rot=_wtoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_DRot, State.mensaje,sizeof(State.mensaje));
			Control.D_Rot=_wtoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_IRot, State.mensaje,sizeof(State.mensaje));
			Control.I_Rot=_wtoi(State.mensaje);
			#else
			GetDlgItemText(hwndDlg1,IDC_PLin, State.mensaje,sizeof(State.mensaje));
			Control.P_Lin=atoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_DLin, State.mensaje,sizeof(State.mensaje));
			Control.D_Lin=atoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_ILin, State.mensaje,sizeof(State.mensaje));
			Control.I_Lin=atoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_PRot, State.mensaje,sizeof(State.mensaje));
			Control.P_Rot=atoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_DRot, State.mensaje,sizeof(State.mensaje));
			Control.D_Rot=atoi(State.mensaje);

			GetDlgItemText(hwndDlg1,IDC_IRot, State.mensaje,sizeof(State.mensaje));
			Control.I_Rot=atoi(State.mensaje);
			#endif

			setParametros=true;

			return TRUE;

		case IDC_CANCEL: 

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.P_Lin);
			SetDlgItemText(hwndDlg1,IDC_PLin, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.D_Lin);
			SetDlgItemText(hwndDlg1,IDC_DLin, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.I_Lin);
			SetDlgItemText(hwndDlg1,IDC_ILin, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.P_Rot);
			SetDlgItemText(hwndDlg1,IDC_PRot, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.D_Rot);
			SetDlgItemText(hwndDlg1,IDC_DRot, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), Control.I_Rot);
			SetDlgItemText(hwndDlg1,IDC_IRot, State.mensaje);

			return TRUE;
		} 
	} 
	return FALSE; 
}

BOOL CALLBACK SetPoseProc(HWND hwndDlg2, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) { 
	case WM_ACTIVATE: 

		StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[0]);
		SetDlgItemText(hwndDlg2,IDC_POSX, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[1]);
		SetDlgItemText(hwndDlg2,IDC_POSY, State.mensaje);

		StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[2]*180.0/PI);
		SetDlgItemText(hwndDlg2,IDC_THETA, State.mensaje);

		return TRUE; 
	case WM_COMMAND: 
		switch (LOWORD(wParam))  { 

		case IDCANCEL: 

			DestroyWindow(hwndDlg2); 
			hwndSetPose = NULL; 
			return TRUE;
		case IDC_SET: 

			#ifdef UNICODE
			GetDlgItemText(hwndDlg2,IDC_POSX, State.mensaje,sizeof(State.mensaje));
			Control.Postura[0]=_wtof(State.mensaje);

			GetDlgItemText(hwndDlg2,IDC_POSY, State.mensaje,sizeof(State.mensaje));
			Control.Postura[1]=_wtof(State.mensaje);

			GetDlgItemText(hwndDlg2,IDC_THETA, State.mensaje,sizeof(State.mensaje));
			Control.Postura[2]=_wtof(State.mensaje);
			#else
			GetDlgItemText(hwndDlg2,IDC_POSX, State.mensaje,sizeof(State.mensaje));
			Control.Postura[0]=atof(State.mensaje);

			GetDlgItemText(hwndDlg2,IDC_POSY, State.mensaje,sizeof(State.mensaje));
			Control.Postura[1]=atof(State.mensaje);

			GetDlgItemText(hwndDlg2,IDC_THETA, State.mensaje,sizeof(State.mensaje));
			Control.Postura[2]=atof(State.mensaje);
			#endif

			setPose=true;
			
			return TRUE;

		case IDC_CANCEL: 

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[0]);
			SetDlgItemText(hwndDlg2,IDC_POSX, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[1]);
			SetDlgItemText(hwndDlg2,IDC_POSY, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[2]*180.0/PI);
			SetDlgItemText(hwndDlg2,IDC_THETA, State.mensaje);

			return TRUE;
		} 
	} 
	return FALSE;  
}

BOOL CALLBACK GetEstadoProc(HWND hwndDlg3, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) { 
        case WM_ACTIVATE: 

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[0]);
			SetDlgItemText(hwndDlg3,IDC_POSX,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[1]);
			SetDlgItemText(hwndDlg3,IDC_POSY,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[2]);
			SetDlgItemText(hwndDlg3,IDC_THETA,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dVelocity[0]);
			SetDlgItemText(hwndDlg3,IDC_LINVEL,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dVelocity[1]);
			SetDlgItemText(hwndDlg3,IDC_ROTVEL,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[0]);
			SetDlgItemText(hwndDlg3,IDC_LIN_P,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[1]);
			SetDlgItemText(hwndDlg3,IDC_LIN_I,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[2]);
			SetDlgItemText(hwndDlg3,IDC_LIN_D,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[3]);
			SetDlgItemText(hwndDlg3,IDC_ROT_P,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[4]);
			SetDlgItemText(hwndDlg3,IDC_ROT_I,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[5]);
			SetDlgItemText(hwndDlg3,IDC_ROT_D,State.mensaje);
					
			return TRUE; 
        case WM_COMMAND: 
            switch (LOWORD(wParam))  { 
                case IDCANCEL: 
					 DestroyWindow(hwndDlg3); 
                     hwndGetEstado = NULL; 
	                 return TRUE;
            } 
    } 
    return FALSE;  
} 
//----------------------------------------------------------------------------//

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
    POINT lpClickPoint;

	switch (message)
	{

	case WM_TIMER: 
		if( hwndGetEstado != NULL){

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[0]);
			SetDlgItemText(hwndGetEstado,IDC_POSX,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[1]);
			SetDlgItemText(hwndGetEstado,IDC_POSY,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dPose[2]);
			SetDlgItemText(hwndGetEstado,IDC_THETA,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dVelocity[0]);
			SetDlgItemText(hwndGetEstado,IDC_LINVEL,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), State.dVelocity[1]);
			SetDlgItemText(hwndGetEstado,IDC_ROTVEL,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[0]);
			SetDlgItemText(hwndGetEstado,IDC_LIN_P,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[1]);
			SetDlgItemText(hwndGetEstado,IDC_LIN_I,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[2]);
			SetDlgItemText(hwndGetEstado,IDC_LIN_D,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[3]);
			SetDlgItemText(hwndGetEstado,IDC_ROT_P,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[4]);
			SetDlgItemText(hwndGetEstado,IDC_ROT_I,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%3d"), State.iParametros[5]);
			SetDlgItemText(hwndGetEstado,IDC_ROT_D,State.mensaje);
		}

		break;

	case WM_USER_SHELLICON: 
		// systray msg callback 
		switch(LOWORD(lParam)) 
		{   
		case WM_RBUTTONDOWN: 
			UINT uFlag = MF_BYPOSITION|MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();

			if (State.run  == FALSE )
			{
				uFlag |= MF_GRAYED;
			}
			InsertMenu(hPopMenu,-1,uFlag,ID_PARAMETROS,TEXT("Parametros"));
			InsertMenu(hPopMenu,-1,uFlag,ID_POSTURA,TEXT("Postura"));
			InsertMenu(hPopMenu,-1,uFlag,ID_ESTADO,TEXT("Estado")); 
			if ( State.run == FALSE )
			{
				InsertMenu(hPopMenu,-1,MF_BYPOSITION|MF_STRING,ID_START,TEXT("Enable"));									
			}
			else 
			{
				InsertMenu(hPopMenu,-1,MF_BYPOSITION|MF_STRING,ID_STOP,TEXT("Disable"));				
			}

			InsertMenu(hPopMenu,-1,MF_BYPOSITION|MF_STRING, ID_EXIT,TEXT("Exit"));

			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
			return TRUE; 

		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_PARAMETROS:
			if (!IsWindow(hwndSetParam))
			{ 
				hwndSetParam = CreateDialog(hInstGlobal, 
					MAKEINTRESOURCE(IDD_PARAMETROS), hWnd, (DLGPROC) SetParamProc); 
				ShowWindow(hwndSetParam, SW_SHOW); 
			} 
			break;

		case ID_POSTURA:
			if (!IsWindow(hwndSetPose))
			{ 
				hwndSetPose = CreateDialog(hInstGlobal, 
					MAKEINTRESOURCE(IDD_POSTURA), hWnd, (DLGPROC) SetPoseProc); 
				ShowWindow(hwndSetPose, SW_SHOW); 
			} 
			break;

		case ID_ESTADO:
			if (!IsWindow(hwndGetEstado))
			{ 
				hwndGetEstado = CreateDialog(hInstGlobal, 
					MAKEINTRESOURCE(IDD_ESTADO), hWnd, (DLGPROC) GetEstadoProc); 
				ShowWindow(hwndGetEstado, SW_SHOW); 
			} 
			break;

		case ID_START:
			if(StartAll(hWnd) == 0)
				return 0;
			else
			{
				MessageBox(NULL, "Hola", "Hola", MB_OK);
				StringCchCopy(nidApp.szTip, ARRAYSIZE(nidApp.szTip), TEXT("Pioneer conectándose..."));
				Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
			}
			break;

		case ID_STOP:
			StopAll(hWnd);
			if(nTcpPort >= 0)
				StringCchPrintf( State.mensaje, 128, TEXT("Pioneer Nº%d detenido"), nTcpPort+1);
			else
				StringCchPrintf( State.mensaje, 128, TEXT("Pioneer no disponible"));
			StringCchCopy(nidApp.szTip, ARRAYSIZE(nidApp.szTip), State.mensaje);
			Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
			break;

		case ID_EXIT:
			StopAll(hWnd);
			Shell_NotifyIcon(NIM_DELETE,&nidApp);
			EndWindow(hWnd, hInstGlobal);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//----------------------------------------------------------------------------/