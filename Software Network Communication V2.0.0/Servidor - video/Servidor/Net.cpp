#include "stdafx.h"
#include "Net.h"
#include "Mmtimer.h"
#include "ShareMemory.h"
#include "VirtualDevice.h"

#define MAX_LOADSTRING	100
#define MAX_BUFFER_IN sizeof(UINT)+MAXNUMBYTES
#define SIZEHORIZON		10

SOCKET SocketArray[2];  //SocketArray[0]:Listen1, SocketArray[0]:Accept1(Read), SocketArray[1]:Listen2, SocketArray[1]:Accept2(Write),
WSAEVENT EventArray[2];
extern char inIP[16], outIP[16];
extern USHORT inPort, outPort;
extern char protocol[4];
sockaddr_in addrRemoteIn;
sockaddr_in addrRemoteOut;
bool flagWaitForEvents;
bool flagCompleteConnection;
double frec;
bool firstData;
extern CHANNEL_PROPERTIES channelProp;
UINT baseTimeRecv=10;
UINT baseTimeSend=10;
UINT indexRecvPacket;
UINT numSentPackets;
UINT numRecvPackets;
UINT numLostPackets;
UINT numDelPackets;
UINT countTimeSend;
UINT countTimeRecv;
UINT SentPackets[SIZEHORIZON];
UINT RecvPackets[SIZEHORIZON];
UINT LostPackets[SIZEHORIZON];
UINT DelPackets[SIZEHORIZON];
double SentTime[SIZEHORIZON];
double RecvTime[SIZEHORIZON];
double remoteTime;
double recvTime;
double roundTripDelay;
extern HINSTANCE hInst;								// current instance
extern NOTIFYICONDATA nidApp;
bool flagSendAddr;

void recvData(SOCKET recvSocket, char *protocol, char *data, int sizeData);
void sendData(SOCKET sendSocket, char *protocol, char *data, int sizeData, CHANNEL_PROPERTIES *channelProp);
void CALLBACK timerProc(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5);

typedef struct tag_HEADER_DATA {
	UINT indexPacket;
	double localTime;
	double remoteTime;
} HEADER_DATA;

HEADER_DATA headerData;

void inicializoVariables(DWORD *EventTotal){

	*EventTotal=0;

	flagCompleteConnection=false;
	flagWaitForEvents=true;
	firstData=false;

	indexRecvPacket=0;
	numSentPackets=0;
	numRecvPackets=0;
	numLostPackets=0;
	numDelPackets=0;
	countTimeSend=0;
	countTimeRecv=0;
	memset(SentPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(RecvPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(LostPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(DelPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(SentTime,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(RecvTime,0,(UINT)SIZEHORIZON*sizeof(UINT));
	remoteTime=0.0;
	recvTime=0.0;
	roundTripDelay=0.0;

	flagSendAddr=false;
}

int initWinsockDll(HWND hwnd){

	// Initialize Winsock
	TCHAR string[500];
	WORD wVersionRequested;
	WSADATA wsaData;
	int iResult;
	wVersionRequested = MAKEWORD(2,2); //version 2.2
	iResult = WSAStartup(wVersionRequested, &wsaData);
	if (iResult != NO_ERROR) {
		//"WinSock Initialization FAILED",
		_stprintf_s(string,sizeof(string), _T("WSAStartup failed with error: %d "),iResult);
		MessageBox(hwnd, string, _T("Error at WSAStartup()"), MB_OK);
	}

	return iResult;

}

int endWinsockDll(HWND hwnd){

	int iResult;
	iResult = WSACleanup();
	TCHAR string[500];
	if (iResult == SOCKET_ERROR) {
		//"WinSock Ending FAILED",
		_stprintf_s(string,sizeof(string), _T("WSACleanup failed with error: %d "),WSAGetLastError());
		MessageBox(hwnd, string, _T("Error at WSACleanup()"), MB_OK);
	}

	return iResult;
}

void loadIPandPort(sockaddr_in *pAddr, char *IP, USHORT port){
	TCHAR string[500];
	char hostName[128];
	char* localIP;
	if (gethostname(hostName,sizeof(hostName)) == 0){
		hostent *host = gethostbyname(hostName);
		if(host == NULL) {
			MessageBox(NULL,_T("The PC haven´t host"),_T("Error"),MB_OK);
		}
		else{
			localIP = inet_ntoa (*(struct in_addr *)*host->h_addr_list);
		}
	}
	else {
		_stprintf_s(string,sizeof(string),_T("gethostname failed with error: %d"),WSAGetLastError());
		MessageBox(NULL,string,_T("Error at gethostname()"),MB_OK);
		//return socketPack;
	}

	memset(pAddr, 0, sizeof(pAddr));
	pAddr->sin_family = AF_INET; // Internet address family
	if((IP == NULL)||(strcmp(IP,"127.0.0.1")==0)){
		//dataTimer.inAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
		pAddr->sin_addr.s_addr = inet_addr(localIP);

	}
	else {
		pAddr->sin_addr.s_addr = inet_addr(IP);
	}
	pAddr->sin_port = htons(port); // Local port
}

DWORD WINAPI EnableConnect(LPVOID lpParameter){
	
	TCHAR string[100];
	char *buffer;
	SOCKADDR_IN inAddr;
	SOCKADDR_IN outAddr;
	SOCKET auxSocket;
	DWORD EventTotal;
	DWORD Index,i;
	WSANETWORKEVENTS NetworkEvents;
	HICON hMainIcon;
	MMTIMER timer;

	inicializoVariables(&EventTotal);

	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li)){
		MessageBox(NULL, _T("QueryPerformanceFrequency failed!"), _T("Warning"), MB_OK);
		return 0;
	}
	frec=double(li.QuadPart)/1000.0;

	loadIPandPort(&inAddr, inIP, inPort);
	loadIPandPort(&outAddr, outIP, outPort);

	EventTotal=0;

	if(strcmp(protocol,"TCP")==0){

		if ((SocketArray[0] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) == INVALID_SOCKET) {
			_stprintf_s(string,sizeof(string), _T("(inListenSocket) socket function failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
		}

		if (bind(SocketArray[0], (sockaddr*) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
			_stprintf_s(string, sizeof(string), _T("(inListenSocket) bind failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
		}

		if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			_stprintf_s(string, sizeof(string), _T("(inListenSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
		}
	
		if(WSAEventSelect(SocketArray[0], EventArray[0],  FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR ) {
			_stprintf_s(string, sizeof(string), _T("(inListenSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
		}
							
		if (listen(SocketArray[0], 1) == SOCKET_ERROR){
			_stprintf_s(string, sizeof(string), _T("(inListenSocket) listen failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at listen()"), MB_OK);
		}

		EventTotal++;
	
		flagWaitForEvents=true;

		while(flagWaitForEvents){

			Index=WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE);
			Index=Index-WSA_WAIT_EVENT_0;

			// Iterate through all events individually, starting with signalled one,to see if more than one is signaled
			for(i=Index; i < EventTotal ;i++)
			{
				Index = WSAWaitForMultipleEvents(1, &EventArray[i], FALSE, 0, FALSE);
				if ((Index == WSA_WAIT_FAILED) || (Index == WSA_WAIT_TIMEOUT))
					continue;
				else
				{
					Index = i;
					if(WSAEnumNetworkEvents(SocketArray[Index],EventArray[Index],&NetworkEvents)==SOCKET_ERROR){
						_stprintf_s(string, sizeof(string), _T("(inListenSocket) WSAEnumNetworkEvents failed with error: %d"), WSAGetLastError());
						MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
					}

					// Process FD_ACCEPT notifications
					if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
					{
						if(NetworkEvents.iErrorCode[FD_ACCEPT_BIT]!=0)
						{
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) FD_ACCEPT failed with error: %d"), NetworkEvents.iErrorCode[FD_ACCEPT_BIT]);
							MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
						}
						else
						{
							if(Index==0){

								auxSocket = accept(SocketArray[0], (sockaddr *) NULL, NULL);

								closesocket(SocketArray[0]);
								SocketArray[0] = auxSocket;

								if(WSAEventSelect(SocketArray[0], EventArray[0],  FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR ) {
									_stprintf_s(string, sizeof(string), _T("(inSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
									MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
								}

								if((strcmp(inet_ntoa(inAddr.sin_addr),inet_ntoa(outAddr.sin_addr))==0)&&(inAddr.sin_port == outAddr.sin_port)){
									SocketArray[1]=SocketArray[0];
																		
									hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_CONNECT));
									nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
									LoadString(hInst, IDS_CONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
									LoadString(hInst, IDS_CONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
									LoadString(hInst, IDS_CONNECT,nidApp.szTip,MAX_LOADSTRING);
									Shell_NotifyIcon(NIM_MODIFY, &nidApp); 

									flagCompleteConnection=true;

									timer.uiPeriod = 1;
									timer.lpProc = timerProc;
									timer.dwData = (DWORD)NULL;

									if(!mmTimerStart(&timer)){
										MessageBox(NULL,_T("mmTimerStart human 1 function failed"),_T("Error"),MB_OK);
									}
								}
								else
								{
									Sleep(10);
									if(send(SocketArray[0], (char *)&outAddr, sizeof(outAddr), 0)==SOCKET_ERROR){
										if(WSAGetLastError()==WSAEWOULDBLOCK){
											flagSendAddr=true;
										}
									}

									if ((SocketArray[1] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) == INVALID_SOCKET) {
										_stprintf_s(string,sizeof(string), _T("(outListenSocket) socket function failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
									}

									if (bind(SocketArray[1], (sockaddr*) &outAddr, sizeof(outAddr)) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(outListenSocket) bind failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}

									if ((EventArray[1] = WSACreateEvent()) == WSA_INVALID_EVENT)
									{
										_stprintf_s(string, sizeof(string), _T("(outListenSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}
	
									if(WSAEventSelect(SocketArray[1], EventArray[1],  FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(outListenSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
									}
							
									if (listen(SocketArray[1], 1) == SOCKET_ERROR){
										_stprintf_s(string, sizeof(string), _T("(outListenSocket) listen failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at listen()"), MB_OK);
									}

									EventTotal++;
								}

							}

							if(Index==1){
							
								if(WSAEventSelect(SocketArray[0], EventArray[0],  FD_READ | FD_CLOSE) == SOCKET_ERROR ) {
									_stprintf_s(string, sizeof(string), _T("(inSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
									MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
								}
							
							
								auxSocket = accept(SocketArray[1], (sockaddr *) NULL, NULL);

								closesocket(SocketArray[1]);
								SocketArray[1] = auxSocket;

								if(WSAEventSelect(SocketArray[1], EventArray[1],  FD_WRITE | FD_CLOSE) == SOCKET_ERROR ) {
									_stprintf_s(string, sizeof(string), _T("(inListenSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
									MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
								}

								hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_CONNECT));
								nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
								LoadString(hInst, IDS_CONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
								LoadString(hInst, IDS_CONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
								LoadString(hInst, IDS_CONNECT,nidApp.szTip,MAX_LOADSTRING);
								Shell_NotifyIcon(NIM_MODIFY, &nidApp); 

								flagCompleteConnection=true;

								timer.uiPeriod = 1;
								timer.lpProc = timerProc;
								timer.dwData = (DWORD)NULL;

								if(!mmTimerStart(&timer)){
									MessageBox(NULL,_T("mmTimerStart human 1 function failed"),_T("Error"),MB_OK);
								}

							}
						}
					}

					// Process FD_READ notification
					if (NetworkEvents.lNetworkEvents & FD_READ)
					{
						if(NetworkEvents.iErrorCode[FD_READ_BIT]!=0)
						{
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) FD_READ failed with error: %d"), NetworkEvents.iErrorCode[FD_READ_BIT]);
							MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
						}
						else
						{
							buffer=(char *)malloc(sizeof(HEADER_DATA)+MAX_BUFFER_IN);
							memset(buffer,0,sizeof(HEADER_DATA)+MAX_BUFFER_IN);
							recvData(SocketArray[1], "TCP", buffer, MAX_BUFFER_IN);
							free(buffer);
							firstData=true;
						}
					}

					// Process FD_WRITE notification
					if (NetworkEvents.lNetworkEvents & FD_WRITE)
					{
						if(NetworkEvents.iErrorCode[FD_WRITE_BIT]!=0)
						{
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) FD_READ failed with error: %d"), NetworkEvents.iErrorCode[FD_WRITE_BIT]);
							MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
						}
						else
						{
							if(flagSendAddr==true){
		
								if(send(SocketArray[0], (char *)&outAddr, sizeof(outAddr), 0)!=SOCKET_ERROR){
									flagSendAddr=false;
								}

							}

						}
					}

					// Process FD_CLOSE notifications
					if (NetworkEvents.lNetworkEvents & FD_CLOSE)
					{
						hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_DISCONNECT));
						nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
						LoadString(hInst, IDS_DISCONNECT,nidApp.szTip,MAX_LOADSTRING);
						Shell_NotifyIcon(NIM_MODIFY, &nidApp); 

						shutdown(SocketArray[0],SD_BOTH);
						closesocket(SocketArray[0]);
						WSACloseEvent(EventArray[0]);

						shutdown(SocketArray[1],SD_BOTH);
						closesocket(SocketArray[1]);
						WSACloseEvent(EventArray[1]);

						inicializoVariables(&EventTotal);

						if ((SocketArray[0] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) == INVALID_SOCKET) {
							_stprintf_s(string,sizeof(string), _T("(inListenSocket) socket function failed with error: %d"), WSAGetLastError());
							MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
						}

						if (bind(SocketArray[0], (sockaddr*) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) bind failed with error: %d"), WSAGetLastError());
							MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
						}

						if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
						{
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
							MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
						}
	
						if(WSAEventSelect(SocketArray[0], EventArray[0],  FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR ) {
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
							MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
						}
							
						if (listen(SocketArray[0], 1) == SOCKET_ERROR){
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) listen failed with error: %d"), WSAGetLastError());
							MessageBox(NULL, string, _T("Error at listen()"), MB_OK);
						}

						EventTotal++;

					}

					//WSAResetEvent(EventArray[Index]);
				}  // end of else
			}  // end of for
		}  // end of while

		mmTimerEnd(timer);

		shutdown(SocketArray[0], SD_BOTH);
		shutdown(SocketArray[1], SD_BOTH);
	}

	if(strcmp(protocol,"UDP")==0){

		sockaddr_in addr;
		int sizeAddr = sizeof(sockaddr_in);
		UINT code;
				
		if ((SocketArray[0] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) == INVALID_SOCKET) {
			_stprintf_s(string,sizeof(string), _T("(inSocket) socket function failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
		}

		if (bind(SocketArray[0], (sockaddr *) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
			//_stprintf_s(string, sizeof(string), _T("(inAddr) bind failed with error: %d"), WSAGetLastError());
			//MessageBox(hWnd, string, _T("Error at bind()"), MB_OK);
		}

		if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			_stprintf_s(string, sizeof(string), _T("(inAddr) WSACreateEvent() failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
		}
	
		if(WSAEventSelect(SocketArray[0], EventArray[0], FD_READ | FD_WRITE) == SOCKET_ERROR ) {
			_stprintf_s(string, sizeof(string), _T("(inAddr) WSAEventSelect failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
		}

		EventTotal++;

		flagWaitForEvents=true;

		while(flagWaitForEvents){

			Index=WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE);

			Index=Index-WSA_WAIT_EVENT_0;

			// Iterate through all events individually, starting with signalled one,to see if more than one is signaled
			for(i=Index; i < EventTotal ;i++)
			{
				Index = WSAWaitForMultipleEvents(1, &EventArray[i], FALSE, 0, FALSE);
				if ((Index == WSA_WAIT_FAILED) || (Index == WSA_WAIT_TIMEOUT))
					continue;
				else
				{
					Index = i;
					if(WSAEnumNetworkEvents(SocketArray[Index],EventArray[Index],&NetworkEvents)==SOCKET_ERROR){
						_stprintf_s(string, sizeof(string), _T("WSAEnumNetworkEvents failed with error: %d"), WSAGetLastError());
						MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
					}

					// Process FD_READ notification
					if (NetworkEvents.lNetworkEvents & FD_READ)
					{
						if(NetworkEvents.iErrorCode[FD_READ_BIT]!=0)
						{
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) FD_READ failed with error: %d"), NetworkEvents.iErrorCode[FD_READ_BIT]);
							MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
						}
						else
						{
							buffer=(char *)malloc(sizeof(UINT)+sizeof(HEADER_DATA)+MAX_BUFFER_IN);
							memset(buffer,0,sizeof(UINT)+sizeof(HEADER_DATA)+MAX_BUFFER_IN);
							if(recvfrom(SocketArray[Index], buffer, sizeof(UINT)+sizeof(HEADER_DATA)+MAX_BUFFER_IN, 0, (sockaddr *)&addr, &sizeAddr)!=SOCKET_ERROR){

								memcpy(&code, &buffer[0], sizeof(UINT));

								if(code==CODE_CONNECT){

									if(Index==0){

										addrRemoteOut=addr;

										code=CODE_ACEPT;
										free(buffer);
										buffer=(char *)malloc(sizeof(UINT)+sizeof(outAddr));
										memset(buffer,0,sizeof(UINT)+sizeof(outAddr));
										memcpy(&buffer[0], &code, sizeof(UINT));
										memcpy(&buffer[sizeof(UINT)], &outAddr, sizeof(outAddr));

										sendto(SocketArray[0], buffer, sizeof(UINT)+sizeof(outAddr), 0, (sockaddr *)&addrRemoteOut, sizeof(addrRemoteOut));

										if((strcmp(inet_ntoa(inAddr.sin_addr),inet_ntoa(outAddr.sin_addr))==0)&&(inAddr.sin_port == outAddr.sin_port)){
											SocketArray[1]=SocketArray[0];
											addrRemoteIn=addrRemoteOut;
											
											hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_CONNECT));
											nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
											LoadString(hInst, IDS_CONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
											LoadString(hInst, IDS_CONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
											LoadString(hInst, IDS_CONNECT,nidApp.szTip,MAX_LOADSTRING);
											Shell_NotifyIcon(NIM_MODIFY, &nidApp); 

											flagCompleteConnection=true;

											timer.uiPeriod = 1;
											timer.lpProc = timerProc;
											timer.dwData = (DWORD)NULL;

											if(!mmTimerStart(&timer)){
												MessageBox(NULL,_T("mmTimerStart human 1 function failed"),_T("Error"),MB_OK);
											}
										}
										else{

											if(WSAEventSelect(SocketArray[0], EventArray[0], FD_READ) == SOCKET_ERROR ) {
												_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
												MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
											}

											if ((SocketArray[1] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) == INVALID_SOCKET) {
												_stprintf_s(string,sizeof(string), _T("(outSocket) socket function failed with error: %d"), WSAGetLastError());
												MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
											}

											if (bind(SocketArray[1], (sockaddr *) &outAddr, sizeof(outAddr)) == SOCKET_ERROR ) {
												//_stprintf_s(string, sizeof(string), _T("(outAddr) bind failed with error: %d"), WSAGetLastError());
												//MessageBox(hWnd, string, _T("Error at bind()"), MB_OK);
											}
												
											if ((EventArray[1] = WSACreateEvent()) == WSA_INVALID_EVENT)
											{
												_stprintf_s(string, sizeof(string), _T("(outSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
												MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
											}
	
											if(WSAEventSelect(SocketArray[1], EventArray[1], FD_READ | FD_WRITE) == SOCKET_ERROR ) {
												_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
												MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
											}

											EventTotal++;
										}
									}

									if(Index==1){
									
										addrRemoteIn=addr;
																					
										code=CODE_ACEPT;
										free(buffer);
										buffer=(char *)malloc(sizeof(UINT));
										memset(buffer,0,sizeof(UINT));
										memcpy(&buffer[0], &code, sizeof(UINT));
										sendto(SocketArray[1], buffer, sizeof(UINT), 0, (sockaddr *)&addrRemoteIn, sizeof(addrRemoteIn));

										if(WSAEventSelect(SocketArray[1], EventArray[1], FD_WRITE) == SOCKET_ERROR ) {
											_stprintf_s(string, sizeof(string), _T("(inSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
											MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
										}

										hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_CONNECT));
										nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
										LoadString(hInst, IDS_CONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
										LoadString(hInst, IDS_CONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
										LoadString(hInst, IDS_CONNECT,nidApp.szTip,MAX_LOADSTRING);
										Shell_NotifyIcon(NIM_MODIFY, &nidApp); 

										flagCompleteConnection=true;

										timer.uiPeriod = 1;
										timer.lpProc = timerProc;
										timer.dwData = (DWORD)NULL;

										if(!mmTimerStart(&timer)){
											MessageBox(NULL,_T("mmTimerStart human 1 function failed"),_T("Error"),MB_OK);
										}
									}

								}

								if(code==CODE_DATA){
									
									recvData(NULL, "UDP", buffer, MAX_BUFFER_IN);
									firstData=true;
								}

								if(code==CODE_CLOSE){
									
									hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_DISCONNECT));
									nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
									LoadString(hInst, IDS_DISCONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
									LoadString(hInst, IDS_DISCONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
									LoadString(hInst, IDS_DISCONNECT,nidApp.szTip,MAX_LOADSTRING);
									Shell_NotifyIcon(NIM_MODIFY, &nidApp);

									code=CODE_CLOSE;
									free(buffer);
									buffer=(char *)malloc(sizeof(UINT));
									memset(buffer,0,sizeof(UINT));
									memcpy(&buffer[0], &code, sizeof(UINT));
									sendto(SocketArray[1], buffer, sizeof(UINT),  0, (sockaddr *)&addrRemoteIn, sizeof(addrRemoteIn));
									
									closesocket(SocketArray[0]);
									WSACloseEvent(EventArray[0]);

									closesocket(SocketArray[1]);
									WSACloseEvent(EventArray[1]);

									inicializoVariables(&EventTotal);

									if ((SocketArray[0] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) == INVALID_SOCKET) {
										_stprintf_s(string,sizeof(string), _T("(inSocket) socket function failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
									}

									if (bind(SocketArray[0], (sockaddr *) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
										//_stprintf_s(string, sizeof(string), _T("(inAddr) bind failed with error: %d"), WSAGetLastError());
										//MessageBox(hWnd, string, _T("Error at bind()"), MB_OK);
									}

									if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
									{
										_stprintf_s(string, sizeof(string), _T("(inAddr) WSACreateEvent() failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}
	
									if(WSAEventSelect(SocketArray[0], EventArray[0], FD_READ | FD_WRITE) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(inAddr) WSAEventSelect failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
									}
									EventTotal++;

								}
							}
							
							free(buffer);
						}
					}

					// Process FD_WRITE notification
					if (NetworkEvents.lNetworkEvents & FD_WRITE)
					{
						if(NetworkEvents.iErrorCode[FD_WRITE_BIT]!=0)
						{
							_stprintf_s(string, sizeof(string), _T("(inListenSocket) FD_READ failed with error: %d"), NetworkEvents.iErrorCode[FD_WRITE_BIT]);
							MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
						}
						else
						{
						

						}
					}
				}  // end of else
			}  // end of for
		}  // end of while

		mmTimerEnd(timer);

		code=CODE_CLOSE;
		buffer=(char *)malloc(sizeof(UINT));
		memset(buffer,0,sizeof(UINT));
		memcpy(&buffer[0], &code, sizeof(UINT));
		sendto(SocketArray[1], buffer, sizeof(UINT), 0, (sockaddr *)&addrRemoteIn, sizeof(addrRemoteIn));
		free(buffer);
	}

	flagCompleteConnection=false;

	hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_DISCONNECT));
	nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
	LoadString(hInst, IDS_DISCONNECT,nidApp.szTip,MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
	
	closesocket(SocketArray[0]);
	WSACloseEvent(EventArray[0]);

	closesocket(SocketArray[1]);
	WSACloseEvent(EventArray[1]);

	return 0;
}

void recvData(SOCKET recvSocket, char *protocol, char *buffer, int sizeData){
	
	static NET_VIRTUAL_DEVICE_OUT pfRedOut={0};
	static VIRTUAL_DEVICE_OUT pfDataOut={0};
	static bool signalRead=true;
	LARGE_INTEGER li;
	char dataIn[MAX_BUFFER_IN];

	if(strcmp(protocol,"TCP")==0){
		recv(recvSocket,buffer,sizeof(HEADER_DATA)+sizeData,0);
								
		memcpy(&headerData, &buffer[0], sizeof(HEADER_DATA));

		if(headerData.indexPacket > indexRecvPacket){
			memcpy(dataIn, &buffer[sizeof(HEADER_DATA)], sizeData);
			
			remoteTime=headerData.localTime;

			QueryPerformanceCounter(&li);
			recvTime=double(li.QuadPart)/frec;
			roundTripDelay=recvTime-headerData.remoteTime;

			indexRecvPacket=headerData.indexPacket;

			numRecvPackets++;

			numLostPackets=indexRecvPacket-numRecvPackets;
		}
		else{
			numDelPackets++;
		}
	}

	if(strcmp(protocol,"UDP")==0){
		memcpy(&headerData, &buffer[sizeof(UINT)], sizeof(HEADER_DATA));
		if(headerData.indexPacket > indexRecvPacket){

			memcpy(&dataIn[0], &buffer[sizeof(UINT)+sizeof(HEADER_DATA)], sizeData);

			remoteTime=headerData.localTime;
											
			QueryPerformanceCounter(&li);
			recvTime=double(li.QuadPart)/frec;
			roundTripDelay=recvTime-headerData.remoteTime;

			indexRecvPacket=headerData.indexPacket;

			numRecvPackets++;

			numLostPackets=indexRecvPacket-numRecvPackets;
		}
		else{
			numDelPackets++;
		}
	}
	
	channelProp.numberRecvPackets=numRecvPackets;
	channelProp.numberLostPackets=numLostPackets;
	channelProp.numberDelPackets=numDelPackets;
	channelProp.roundTripDelay=roundTripDelay;

	for( UINT i=1; i<(UINT)SIZEHORIZON; i++){
		RecvPackets[(UINT)SIZEHORIZON-i]=RecvPackets[(UINT)SIZEHORIZON-i-1];
		LostPackets[(UINT)SIZEHORIZON-i]=LostPackets[(UINT)SIZEHORIZON-i-1];
		DelPackets[(UINT)SIZEHORIZON-i]=DelPackets[(UINT)SIZEHORIZON-i-1];
		RecvTime[(UINT)SIZEHORIZON-i]=RecvTime[(UINT)SIZEHORIZON-i-1];
	}

	QueryPerformanceCounter(&li);
	RecvTime[0]=double(li.QuadPart)/frec;

	RecvPackets[0]=channelProp.numberRecvPackets;
	LostPackets[0]=channelProp.numberLostPackets;
	DelPackets[0]=channelProp.numberDelPackets;
				
	channelProp.rateRecvPackets=double(RecvPackets[0]-RecvPackets[(UINT)SIZEHORIZON-1])*1000.0/(RecvTime[0]-RecvTime[(UINT)SIZEHORIZON-1]);
	channelProp.rateLostPackets=double(LostPackets[0]-LostPackets[(UINT)SIZEHORIZON-1])*1000.0/(RecvTime[0]-RecvTime[(UINT)SIZEHORIZON-1]);
	channelProp.rateDelPackets=double(DelPackets[0]-DelPackets[(UINT)SIZEHORIZON-1])*1000.0/(RecvTime[0]-RecvTime[(UINT)SIZEHORIZON-1]);

	//Memoria Compartida
	pfRedOut.numberSentPackets=channelProp.numberSentPackets;
	pfRedOut.numberRecvPackets=channelProp.numberRecvPackets;
	pfRedOut.numberLostPackets=channelProp.numberLostPackets;
	pfRedOut.numberDelPackets=channelProp.numberDelPackets;
	pfRedOut.rateSentPackets=(float)channelProp.rateSentPackets;
	pfRedOut.rateRecvPackets=(float)channelProp.rateRecvPackets;
	pfRedOut.rateLostPackets=(float)channelProp.rateLostPackets;
	pfRedOut.rateDelPackets=(float)channelProp.rateDelPackets;
	pfRedOut.roundTripDelay=(float)channelProp.roundTripDelay;
	pfRedOut.signalRead=signalRead;
	CopyMemory(&pfRedOut.dataMustBeReceived,&dataIn[0],sizeof(UINT));

	CopyMemory(pfDataOut.dataMemory,&dataIn[sizeof(UINT)],sizeData-sizeof(UINT));
	
	NetWriteShareMemory(MEMREDS1,&pfRedOut);
	WriteShareMemory(MEMDATOSS1,&pfDataOut);

	if(signalRead==0) signalRead=1;
	else signalRead=0;

}

void sendData(SOCKET sendSocket, char *protocol, char *data, int sizeData){
	int numBytes=0;
	char *buffer;
		
	if(flagCompleteConnection==true){
		
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		headerData.indexPacket=numSentPackets+1;
		headerData.localTime=double(li.QuadPart)/frec;
		headerData.remoteTime=remoteTime+(headerData.localTime-recvTime);
						
		if(strcmp(protocol,"TCP")==0){
			buffer=(char *)malloc(sizeof(HEADER_DATA)+sizeData);
			memset(buffer, 0, sizeof(HEADER_DATA)+sizeData);
			memcpy(&buffer[0], &headerData, sizeof(HEADER_DATA));
			memcpy(&buffer[sizeof(HEADER_DATA)], data, sizeData);
			numBytes=send(sendSocket, buffer, sizeof(HEADER_DATA)+sizeData, 0);
			free(buffer);
		}
		if(strcmp(protocol,"UDP")==0){
			UINT code=CODE_DATA;
			buffer=(char *)malloc(sizeof(UINT)+sizeof(HEADER_DATA)+sizeData);
			memset(buffer, 0, sizeof(UINT)+sizeof(HEADER_DATA)+sizeData);
			memcpy(&buffer[0], &code, sizeof(UINT));
			memcpy(&buffer[sizeof(UINT)], &headerData, sizeof(HEADER_DATA));
			memcpy(&buffer[sizeof(UINT)+sizeof(HEADER_DATA)], data, sizeData);
			numBytes=sendto(sendSocket, buffer, sizeof(UINT)+sizeof(HEADER_DATA)+sizeData, 0, (sockaddr *) &addrRemoteIn, sizeof(addrRemoteIn));
			free(buffer);
		}

		numSentPackets++;

		channelProp.numberSentPackets=numSentPackets;

		for( UINT i=1; i<(UINT)SIZEHORIZON; i++){
			SentPackets[(UINT)SIZEHORIZON-i]=SentPackets[(UINT)SIZEHORIZON-i-1];
			SentTime[(UINT)SIZEHORIZON-i]=SentTime[(UINT)SIZEHORIZON-i-1];
		}

		QueryPerformanceCounter(&li);
		SentTime[0]=double(li.QuadPart)/frec;

		SentPackets[0]=channelProp.numberSentPackets;
		channelProp.rateSentPackets=double(SentPackets[0]-SentPackets[(UINT)SIZEHORIZON-1])*1000.0/(SentTime[0]-SentTime[(UINT)SIZEHORIZON-1]);

	}
}

void CALLBACK timerProc(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5){

	static NET_VIRTUAL_DEVICE_IN pfRedIn={0};
	static VIRTUAL_DEVICE_IN pfDataIn={0};
	static bool prevSignalWrite=0;
	char *dataOut;

	NetReadShareMemory(MEMREDS1,&pfRedIn);

	if(pfRedIn.signalWrite!=prevSignalWrite){

		dataOut = (char *)malloc(sizeof(UINT)+pfRedIn.sizeMemoryOut);
		memset(dataOut,0,sizeof(UINT)+pfRedIn.sizeMemoryOut);

		ReadShareMemory(MEMDATOSS1,&pfDataIn);

		CopyMemory(&dataOut[0],&pfRedIn.sizeMemoryOut,sizeof(UINT));
		CopyMemory(&dataOut[sizeof(UINT)],pfDataIn.dataMemory,pfRedIn.sizeMemoryOut);

		sendData(SocketArray[0], protocol, dataOut, sizeof(UINT)+pfRedIn.sizeMemoryOut);

		free(dataOut);
	}

	prevSignalWrite=pfRedIn.signalWrite;

	return;
}


	