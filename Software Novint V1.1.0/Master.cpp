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
BOOL CALLBACK JoyProc2(HWND hwndDlg2, UINT message, WPARAM wParam, LPARAM lParam);
void EndWindow(HWND hWnd, HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);


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

   hMainIcon = LoadIcon(hInstance,(LPCTSTR)MAKEINTRESOURCE(IDI_ICON2)); 

   nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
   nidApp.hWnd = (HWND) hWnd;              //handle of the window which will process this app. messages 
   nidApp.uID = IDI_ICON2;           //ID of the icon that willl appear in the system tray 
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2));

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

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dPosition[0]);
			SetDlgItemText(hwndDlg1,IDC_POSX,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dPosition[1]);
			SetDlgItemText(hwndDlg1,IDC_POSY,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dPosition[2]);
			SetDlgItemText(hwndDlg1,IDC_POSZ,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dVelocity[0]);
			SetDlgItemText(hwndDlg1,IDC_VELX,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dVelocity[1]);
			SetDlgItemText(hwndDlg1,IDC_VELY,State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dVelocity[2]);
			SetDlgItemText(hwndDlg1,IDC_VELZ,State.mensaje);

			// Fill up text with which buttons are pressed
			StringCchCopy( State.mensaje, 128, TEXT("") );
			for( int i = 0; i < 4; i++ )
			{
				if ( State.dButtons[i] )
				{
					TCHAR sz[32];
					StringCchPrintf( sz, 4, TEXT("%01d "), i );
					StringCchCat( State.mensaje, 128, sz );
				}
			}
			SetDlgItemText(hwndDlg1,IDC_BUTTONS,State.mensaje);
		
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



BOOL CALLBACK JoyProc2(HWND hwndDlg2, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) { 
        case WM_ACTIVATE: 

			if(Control.iDispositivo==0){
				CheckDlgButton(hwndDlg2,IDC_NOVINT1,BST_CHECKED);
			}

			if(Control.iDispositivo==1){
				CheckDlgButton(hwndDlg2,IDC_NOVINT2,BST_CHECKED);
			}
			if(Control.iDispositivo==2){
				CheckDlgButton(hwndDlg2,IDC_NOVINT3,BST_CHECKED);
			}
			if(Control.iDispositivo==3){
				CheckDlgButton(hwndDlg2,IDC_NOVINT4,BST_CHECKED);
			}

			StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dSpring[0]);
			SetDlgItemText(hwndJoy2,IDC_KX, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dSpring[1]);
			SetDlgItemText(hwndJoy2,IDC_KY, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dSpring[2]);
			SetDlgItemText(hwndJoy2,IDC_KZ, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dDamping[0]);
			SetDlgItemText(hwndJoy2,IDC_BX1, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dDamping[1]);
			SetDlgItemText(hwndJoy2,IDC_BY1, State.mensaje);

			StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dDamping[2]);
			SetDlgItemText(hwndJoy2,IDC_BZ1, State.mensaje);

			return TRUE; 
        case WM_COMMAND: 
            switch (LOWORD(wParam))  { 

				case IDCANCEL: 

					 DestroyWindow(hwndDlg2); 
                     hwndJoy2 = NULL; 
	                 return TRUE;
				 case IDCARGAR: 
					 
					 if(IsDlgButtonChecked(hwndDlg2,IDC_NOVINT1)==BST_CHECKED){
							Control.iDispositivo=0;
					 }
					 if(IsDlgButtonChecked(hwndDlg2,IDC_NOVINT2)==BST_CHECKED){
							Control.iDispositivo=1;
					 }
					  if(IsDlgButtonChecked(hwndDlg2,IDC_NOVINT3)==BST_CHECKED){
							Control.iDispositivo=2;
					 }
					 if(IsDlgButtonChecked(hwndDlg2,IDC_NOVINT4)==BST_CHECKED){
							Control.iDispositivo=3;
					 }

					 #ifdef UNICODE
					 GetDlgItemText(hwndJoy2,IDC_KX, State.mensaje,sizeof(State.mensaje));
					 Control.dSpring[0]=_wtof(State.mensaje);
				
					 GetDlgItemText(hwndJoy2,IDC_KY, State.mensaje,sizeof(State.mensaje));
					 Control.dSpring[1]=_wtof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_KZ, State.mensaje,sizeof(State.mensaje));
					 Control.dSpring[2]=_wtof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_BX1, State.mensaje,sizeof(State.mensaje));
					 Control.dDamping[0]=_wtof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_BY1, State.mensaje,sizeof(State.mensaje));
					 Control.dDamping[1]=_wtof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_BZ1, State.mensaje,sizeof(State.mensaje));
					 Control.dDamping[2]=_wtof(State.mensaje);
					 #else
					 GetDlgItemText(hwndJoy2,IDC_KX, State.mensaje,sizeof(State.mensaje));
					 Control.dSpring[0]=atof(State.mensaje);
				
					 GetDlgItemText(hwndJoy2,IDC_KY, State.mensaje,sizeof(State.mensaje));
					 Control.dSpring[1]=atof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_KZ, State.mensaje,sizeof(State.mensaje));
					 Control.dSpring[2]=atof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_BX1, State.mensaje,sizeof(State.mensaje));
					 Control.dDamping[0]=atof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_BY1, State.mensaje,sizeof(State.mensaje));
					 Control.dDamping[1]=atof(State.mensaje);

					 GetDlgItemText(hwndJoy2,IDC_BZ1, State.mensaje,sizeof(State.mensaje));
					 Control.dDamping[2]=atof(State.mensaje);
					 #endif

	                 return TRUE;

                case IDCANCEL1: 

					 StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dSpring[0]);
					 SetDlgItemText(hwndJoy2,IDC_KX, State.mensaje);

					 StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dSpring[1]);
					 SetDlgItemText(hwndJoy2,IDC_KY, State.mensaje);

					 StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dSpring[2]);
					 SetDlgItemText(hwndJoy2,IDC_KZ, State.mensaje);

					 StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dDamping[0]);
					 SetDlgItemText(hwndJoy2,IDC_BX1, State.mensaje);

					 StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dDamping[1]);
					 SetDlgItemText(hwndJoy2,IDC_BY1, State.mensaje);

					 StringCchPrintf( State.mensaje, 128, TEXT("%8.1f"), (float)Control.dDamping[2]);
					 SetDlgItemText(hwndJoy2,IDC_BZ1, State.mensaje);
					
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
			if( hwndJoy1 != NULL){
				
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dPosition[0]);
				SetDlgItemText(hwndJoy1,IDC_POSX,State.mensaje);

				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dPosition[1]);
				SetDlgItemText(hwndJoy1,IDC_POSY,State.mensaje);

				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dPosition[2]);
				SetDlgItemText(hwndJoy1,IDC_POSZ,State.mensaje);
	
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dVelocity[0]);
				SetDlgItemText(hwndJoy1,IDC_VELX,State.mensaje);
	
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dVelocity[1]);
				SetDlgItemText(hwndJoy1,IDC_VELY,State.mensaje);
					
				StringCchPrintf( State.mensaje, 128, TEXT("%2.4f"), (float)State.dVelocity[2]);
				SetDlgItemText(hwndJoy1,IDC_VELZ,State.mensaje);
	
				// Fill up text with which buttons are pressed
				StringCchCopy( State.mensaje, 128, TEXT("") );
				for( int i = 0; i < 4; i++ )
				{
					if ( State.dButtons[i] )
					{
						TCHAR sz[32];
						StringCchPrintf( sz, 4, TEXT("%01d "), i );
						StringCchCat( State.mensaje, 128, sz );
					}
				}
				SetDlgItemText(hwndJoy1,IDC_BUTTONS,State.mensaje);
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
				InsertMenu(hPopMenu,-1,uFlag,ID_SET,TEXT("Set"));
				if (State.run  == FALSE )
				{
					uFlag |= MF_GRAYED;
				}
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

		case ID_SET:
			if (!IsWindow(hwndJoy2)) { 
				hwndJoy2 = CreateDialog(hInstance, 
					MAKEINTRESOURCE(IDD_SET), hWnd, (DLGPROC) JoyProc2); 
				ShowWindow(hwndJoy2, SW_SHOW); 
			} 
			break;

		case ID_START:
			if(StartAll(hWnd) == 0)
				return 0;
			else
			{
				StringCchPrintf( State.mensaje, 128, TEXT("Novint Nº%d activo"), Control.iDispositivo+1);
				StringCchCopy(nidApp.szTip, ARRAYSIZE(nidApp.szTip), State.mensaje);
				Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
			}
			break;

		case ID_STOP:
			StopAll(hWnd);
			StringCchPrintf( State.mensaje, 128, TEXT("Novint Nº%d detenido"), Control.iDispositivo+1);
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

