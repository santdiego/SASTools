// Servidor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Servidor.h"
#include "Net.h"
#include "File.h"
#include "VirtualDevice.h"

#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON (WM_USER + 1)

// Global Variables:
HINSTANCE hInst;								// current instance
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szApplicationToolTip[MAX_LOADSTRING];	    // the main window class name
extern SOCKET SocketArray[2];
extern WSAEVENT EventArray[2];
extern bool flagSendAddr;
extern bool flagWaitForEvents;
char inIP[16]="127.0.0.1", outIP[16]="127.0.0.1";
USHORT inPort=10000, outPort=10001;
char protocol[4]="UDP";
CHANNEL_PROPERTIES channelProp;
HWND g_hDlgState;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Config(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	State(HWND, UINT, WPARAM, LPARAM);
extern int initWinsockDll(HWND hwnd);
extern int endWinsockDll(HWND hwnd);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SERVIDOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVIDOR));

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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DISCONNECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SERVIDOR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_DISCONNECT));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   HICON hMainIcon;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   hMainIcon = LoadIcon(hInstance,(LPCTSTR)MAKEINTRESOURCE(IDI_DISCONNECT)); 

   nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
   nidApp.hWnd = (HWND) hWnd;              //handle of the window which will process this app. messages 
   nidApp.uID = IDI_DISCONNECT;           //ID of the icon that willl appear in the system tray 
   nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO; //ORing of all the flags 
   nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
   nidApp.uCallbackMessage = WM_USER_SHELLICON;
   nidApp.dwInfoFlags=NIIF_INFO;
   LoadString(hInstance, IDS_DISCONNECT,nidApp.szTip,MAX_LOADSTRING);
   Shell_NotifyIcon(NIM_ADD, &nidApp); 

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	POINT lpClickPoint;
	
	switch (message)
	{
		case WM_USER_SHELLICON: 
		// systray msg callback 
		switch(LOWORD(lParam)) 
		{   
			case WM_RBUTTONDOWN: 
				UINT uFlag = MF_BYPOSITION|MF_STRING;
				GetCursorPos(&lpClickPoint);
				hPopMenu = CreatePopupMenu();
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ABOUT,_T("Acerca de"));
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,ID_FILE_SEP,_T("SEP"));
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_CONFIG,_T("Configurar"));
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,ID_FILE_SEP,_T("SEP"));
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_STATE,_T("Estado"));
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,ID_FILE_SEP,_T("SEP"));
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_EXIT,_T("Salir"));

				SetForegroundWindow(hWnd);
				TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
				return TRUE; 

		}
		break;

	case WM_CREATE:
		NetOpenPairSharedMemory(MEMREDS1);
		OpenPairSharedMemory(MEMDATOSS1);
		readFile(inIP,&inPort,outIP,&outPort,protocol);
		initWinsockDll(NULL);
		CreateThread( NULL, 0, EnableConnect, NULL, 0, NULL);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		
		case IDM_CONFIG:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIGBOX), hWnd, Config);
			break;

		case IDM_STATE:
			g_hDlgState=CreateDialog(hInst, MAKEINTRESOURCE(IDD_STATEBOX), hWnd, State);
			ShowWindow(g_hDlgState, SW_SHOW);
			break;

		case IDM_EXIT:
			flagWaitForEvents=false;
			WSASetEvent(EventArray[0]);
			Shell_NotifyIcon(NIM_DELETE,&nidApp);

			Sleep(100);

			endWinsockDll(NULL);

			NetClosePairSharedMemory(MEMREDS1);
			ClosePairSharedMemory(MEMDATOSS1);

			DestroyWindow(hWnd);

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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for config box.
INT_PTR CALLBACK Config(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char port[6];
	static char portIN_ant[6]="",portOUT_ant[6]="",portSERVER_ant[6]="";
	static bool changeForSet=false;
	UINT index;
		
	switch (message)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg,IDC_PORTIN),EM_SETLIMITTEXT,(WPARAM)5,NULL);
		SendMessage(GetDlgItem(hDlg,IDC_PORTOUT),EM_SETLIMITTEXT,(WPARAM)5,NULL);
				
		SetDlgItemTextA(hDlg,IDC_IPADDRESSIN,inIP);
		SetDlgItemTextA(hDlg,IDC_IPADDRESSOUT,outIP);
		SetDlgItemInt(hDlg,IDC_PORTIN,inPort,false);
		SetDlgItemInt(hDlg,IDC_PORTOUT,outPort,false);
		
		if(strcmp(protocol,"TCP")==0)	CheckDlgButton(hDlg,IDC_RADIOTCP,BST_CHECKED);
		else CheckDlgButton(hDlg,IDC_RADIOUDP,BST_CHECKED);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if(HIWORD(wParam)==EN_CHANGE){
			
			switch(LOWORD(wParam)){
				case IDC_PORTIN:
				case IDC_PORTOUT:
					
					if(changeForSet==false){

						GetDlgItemTextA(hDlg,LOWORD(wParam),port,sizeof(port));
						SendMessage((HWND)lParam,EM_GETSEL,NULL,(LPARAM)&index);
						if(index>0){
							if( !isdigit(port[index-1]) ){
								changeForSet=true;
								if(LOWORD(wParam)==IDC_PORTIN)		SetDlgItemTextA(hDlg,LOWORD(wParam),portIN_ant);
								if(LOWORD(wParam)==IDC_PORTOUT)		SetDlgItemTextA(hDlg,LOWORD(wParam),portOUT_ant);
								index--;
								SendMessage((HWND)lParam,EM_SETSEL,(WPARAM)index,(LPARAM)index);
								break;						
							}
							if( isdigit(port[index-1]) ){

								if(atoi(port)>=65535){
										changeForSet=true;
										SetDlgItemInt(hDlg,LOWORD(wParam),65535,false);
										SendMessage((HWND)lParam,EM_SETSEL,(WPARAM)index,(LPARAM)index);
								}

								if(LOWORD(wParam)==IDC_PORTIN)		strcpy_s(portIN_ant,port);
								if(LOWORD(wParam)==IDC_PORTOUT)		strcpy_s(portOUT_ant,port);
							}
						}
					}
					else{
						changeForSet=false;
					}

					break;
			}

			return (INT_PTR)TRUE;
		}
		
		if (LOWORD(wParam) == IDC_GUARDAR)
		{
			flagWaitForEvents=false;
			WSASetEvent(EventArray[0]);
			
			Sleep(100);

			GetDlgItemTextA(hDlg,IDC_IPADDRESSIN,inIP,sizeof(inIP));
			GetDlgItemTextA(hDlg,IDC_IPADDRESSOUT,outIP,sizeof(outIP));
			
			inPort = (USHORT)GetDlgItemInt(hDlg,IDC_PORTIN,NULL,false);
			outPort = (USHORT)GetDlgItemInt(hDlg,IDC_PORTOUT,NULL,false);
			
			if(IsDlgButtonChecked(hDlg,IDC_RADIOTCP)) strcpy_s(protocol,"TCP");
			else strcpy_s(protocol,"UDP");
			
			writeFile(inIP, inPort, outIP, outPort, protocol);

			CreateThread( NULL, 0, EnableConnect, NULL, 0, NULL);

			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}


// Message handler for State box.
INT_PTR CALLBACK State(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char string[20];

	switch (message)
	{
	case WM_INITDIALOG:
		SetTimer(hDlg,0,500,NULL);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			KillTimer(hDlg,0);
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_TIMER:
		sprintf_s(string, sizeof(string), "%d", channelProp.numberSentPackets);
		SetDlgItemTextA(hDlg,IDC_TOTALSENTP,string);
		sprintf_s(string, sizeof(string), "%d", channelProp.numberRecvPackets);
		SetDlgItemTextA(hDlg,IDC_TOTALRECVP,string);
		sprintf_s(string, sizeof(string), "%d", channelProp.numberLostPackets);
		SetDlgItemTextA(hDlg,IDC_TOTALLOSTP,string);
		sprintf_s(string, sizeof(string), "%d", channelProp.numberDelPackets);
		SetDlgItemTextA(hDlg,IDC_TOTALDELP,string);
		sprintf_s(string, sizeof(string), "%.2f 1/s", (float)channelProp.rateSentPackets);
		SetDlgItemTextA(hDlg,IDC_RATESENTP,string);
		sprintf_s(string, sizeof(string), "%.2f 1/s", (float)channelProp.rateRecvPackets);
		SetDlgItemTextA(hDlg,IDC_RATERECVP,string);
		sprintf_s(string, sizeof(string), "%.2f 1/s", (float)channelProp.rateLostPackets);
		SetDlgItemTextA(hDlg,IDC_RATELOSTP,string);
		sprintf_s(string, sizeof(string), "%.2f 1/s", (float)channelProp.rateDelPackets);
		SetDlgItemTextA(hDlg,IDC_RATEDELP,string);
		sprintf_s(string, sizeof(string), "%.2f ms", (float)channelProp.roundTripDelay);
		SetDlgItemTextA(hDlg,IDC_ROUNDTRIPD,string);
		break;
	}

	return (INT_PTR)FALSE;
}