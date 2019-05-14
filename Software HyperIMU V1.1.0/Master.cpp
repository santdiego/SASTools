#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <strsafe.h>

#include "resource.h"
#include "wavevar.h"

//instance handle
HINSTANCE hInstance;

HWND hwndJoy1 = NULL;
HWND hwndJoy2 = NULL;
HINSTANCE hInstance1;

#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1

// Global Variables:
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szApplicationToolTip[MAX_LOADSTRING];	    // the main window class name
HINSTANCE hInst;	// current instance

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK JoyProc1(HWND hwndDlg1, UINT message, WPARAM wParam, LPARAM lParam);
void EndWindow(HWND hWnd, HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);


//----------------------------------------------------------------------------//
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpszCmdLine,int nCmdShow )
{

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	WORD wVersionRequested;
	WSADATA wsaData;


//---------Init socket librarie----------------
	wVersionRequested = MAKEWORD( 2, 2 );
	if ( WSAStartup( wVersionRequested, &wsaData ) != 0 ) {
		MessageBox(NULL,"Error: WSAStartup()","Error",MB_OK);
		return 0;
	}
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {
		if( WSACleanup() == SOCKET_ERROR ){
			MessageBox(NULL,"Error: WSACleanup()","Error",MB_OK);
		}
		return 0;
	}


	InitState();
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
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
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

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hMainIcon = LoadIcon(hInstance,(LPCTSTR)MAKEINTRESOURCE(IDI_ICON1)); 
 

   nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
   nidApp.hWnd = (HWND) hWnd;              //handle of the window which will process this app. messages 
   nidApp.uID = IDI_ICON1;           //ID of the icon that willl appear in the system tray 
 
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassEx(&wcex);
}

void EndWindow(HWND hWnd, HINSTANCE hInstance)
{
	CloseWindow(hWnd);
	DestroyWindow(hWnd);
	UnregisterClass(szWindowClass,hInstance);
}

BOOL CALLBACK JoyProc1(HWND hwndDlg1, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) { 
        case WM_ACTIVATE: 

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[0]);
			SetDlgItemText(hwndDlg1,IDC_POSX,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[1]);
			SetDlgItemText(hwndDlg1,IDC_POSY,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[2]);
			SetDlgItemText(hwndDlg1,IDC_POSZ,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[3]);
			SetDlgItemText(hwndDlg1,IDC_VELX,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[4]);
			SetDlgItemText(hwndDlg1,IDC_VELY,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[5]);
			SetDlgItemText(hwndDlg1,IDC_VELZ,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.m_dDurationRx);
			SetDlgItemText(hwndJoy1,IDC_AUX1,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%d"), (int)State.iPacksRx);
			SetDlgItemText(hwndJoy1,IDC_AUX2,State.mensaje);

		
			return TRUE; 
        case WM_COMMAND: 
            switch (LOWORD(wParam))  { 
                case IDCANCEL: 
					 DestroyWindow(hwndDlg1); 
                     hwndJoy1 = NULL; 
	                 return TRUE;
            } 
    } 
    return FALSE; 
} 


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
    POINT lpClickPoint;

	switch (message)
	{

	case WM_TIMER: 
			if( hwndJoy1 != NULL){
				
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[0]);
				SetDlgItemText(hwndJoy1,IDC_POSX,State.mensaje);

				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[1]);
				SetDlgItemText(hwndJoy1,IDC_POSY,State.mensaje);

				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[2]);
				SetDlgItemText(hwndJoy1,IDC_POSZ,State.mensaje);
	
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[3]);
				SetDlgItemText(hwndJoy1,IDC_VELX,State.mensaje);
	
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[4]);
				SetDlgItemText(hwndJoy1,IDC_VELY,State.mensaje);
					
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.IMU[5]);
				SetDlgItemText(hwndJoy1,IDC_VELZ,State.mensaje);

				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.m_dDurationRx);
				SetDlgItemText(hwndJoy1,IDC_AUX1,State.mensaje);

				StringCchPrintf( State.mensaje, 128, TEXT("%d"), (int)State.iPacksRx);
				SetDlgItemText(hwndJoy1,IDC_AUX2,State.mensaje);
	

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
			
				InsertMenu(hPopMenu,-1,uFlag,ID_ESTADO,TEXT("Sensors")); 
			
			
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
		case ID_ESTADO:
			if (!IsWindow(hwndJoy1)) { 
				hwndJoy1 = CreateDialog(hInstance, 
					MAKEINTRESOURCE(IDD_ESTADO), hWnd, (DLGPROC) JoyProc1); 
				ShowWindow(hwndJoy1, SW_SHOW); 
			} 
			break;



		case ID_START:
			if(StartAll(hWnd) == 0)
				return 0;
			else
			{
				StringCchPrintf( State.mensaje, 128, TEXT("Cliente HyperIMU activo"));
				StringCchCopy(nidApp.szTip, ARRAYSIZE(nidApp.szTip), State.mensaje);
				Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
			}
			break;

		case ID_STOP:
			StopAll(hWnd);
			StringCchPrintf( State.mensaje, 128, TEXT("Cliente HyperIMU detenido"));
			StringCchCopy(nidApp.szTip, ARRAYSIZE(nidApp.szTip), State.mensaje);
			Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
			break;

		case ID_EXIT:
			Shell_NotifyIcon(NIM_DELETE,&nidApp);
			EndWindow(hWnd, hInstance);

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		/*
		case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;*/
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

