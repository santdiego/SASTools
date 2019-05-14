#include "stdafx.h"
#include "Net.h"
#include "Mmtimer.h"
#include "ShareMemory.h"
#include "VirtualDevice.h"

#define MAX_LOADSTRING 100
#define MAX_BUFFER_IN sizeof(UINT)+MAXNUMBYTES
#define SIZEHORIZON 10

SOCKET SocketArray[2];  //SocketArray[0]:Connect1(Write), SocketArray[1]:Connect2(Read)
WSAEVENT EventArray[2];
extern char inIP[16], outIP[16], serverIP[16];
extern USHORT inPort, outPort, serverPort;
extern char protocol[4];
sockaddr_in addrRemoteIn;
sockaddr_in addrRemoteOut;
bool flagWaitForEvents;
bool flagCompleteConnection;
extern bool flagConnection;
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
bool firstOpenMemoryData=false;

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
	memset(SentPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(RecvPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(LostPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(DelPackets,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(SentTime,0,(UINT)SIZEHORIZON*sizeof(UINT));
	memset(RecvTime,0,(UINT)SIZEHORIZON*sizeof(UINT));
	remoteTime=0.0;
	recvTime=0.0;
	roundTripDelay=0.0;

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


DWORD WINAPI Connect(LPVOID lpParameter){

	TCHAR string[100];
	char *buffer;
	sockaddr_in inAddr;
	sockaddr_in outAddr;
	UINT port;
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

	loadIPandPort(&outAddr, outIP, outPort);
	loadIPandPort(&inAddr, inIP, inPort);
	loadIPandPort(&addrRemoteIn, serverIP, serverPort);
	
	if(strcmp(protocol,"TCP")==0){
		
		SocketArray[0] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
		if (SocketArray[0]  == INVALID_SOCKET) {
			_stprintf_s(string,sizeof(string), _T("(outSocket) socket function failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
		}

		if (bind(SocketArray[0], (sockaddr*) &outAddr, sizeof(outAddr)) == SOCKET_ERROR ) {
			//_stprintf_s(string, sizeof(string), _T("(outSocket) bind failed with error: %d"), WSAGetLastError());
			//MessageBox(hWnd, string, _T("Error at bind()"), MB_OK);
		}

		if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			_stprintf_s(string, sizeof(string), _T("(outSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
		}
	
		if(WSAEventSelect(SocketArray[0], EventArray[0], FD_CONNECT | FD_READ) == SOCKET_ERROR ) {
			_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
		}

	
		if(connect(SocketArray[0], (sockaddr *) &addrRemoteIn, sizeof(addrRemoteIn))==SOCKET_ERROR){
			if(WSAGetLastError()!=WSAEWOULDBLOCK){
				_stprintf_s(string, sizeof(string), _T("(outSocket) connect failed with error: %d"), WSAGetLastError());
				MessageBox(NULL, string, _T("Error at connect()"), MB_OK);
			}
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

					// Process FD_CONNECT notifications
					if (NetworkEvents.lNetworkEvents & FD_CONNECT)
					{
						if(NetworkEvents.iErrorCode[FD_CONNECT_BIT]!=0)
						{
							if(NetworkEvents.iErrorCode[FD_CONNECT_BIT]==WSAEADDRINUSE){

								if(Index==0){
									port=ntohs(outAddr.sin_port);
									port++;
									loadIPandPort(&outAddr, outIP, port);

									port=ntohs(inAddr.sin_port);
									port++;
									loadIPandPort(&inAddr, inIP, port);

									closesocket(SocketArray[0]);

									SocketArray[0] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
									if (SocketArray[0]  == INVALID_SOCKET) {
										_stprintf_s(string,sizeof(string), _T("(outSocket) socket function failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
									}

									if (bind(SocketArray[0], (sockaddr*) &outAddr, sizeof(outAddr)) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(outSocket) bind failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}

									if(WSAEventSelect(SocketArray[0], EventArray[0], FD_CONNECT | FD_READ) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
									}

	
									if(connect(SocketArray[0], (sockaddr *) &addrRemoteIn, sizeof(addrRemoteIn))==SOCKET_ERROR){
										if(WSAGetLastError()!=WSAEWOULDBLOCK){
											_stprintf_s(string, sizeof(string), _T("(outSocket) connect failed with error: %d"), WSAGetLastError());
											MessageBox(NULL, string, _T("Error at connect()"), MB_OK);
										}
									}
								}
								if(Index==1){

									port=ntohs(inAddr.sin_port);
									port++;
									loadIPandPort(&inAddr, inIP, port);
								
									closesocket(SocketArray[1]);

									SocketArray[1] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
									if (SocketArray[1]  == INVALID_SOCKET) {
										_stprintf_s(string,sizeof(string), _T("(outSocket) socket function failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
									}
								

									if (bind(SocketArray[1], (sockaddr*) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(outSocket) bind failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}

									if(WSAEventSelect(SocketArray[1], EventArray[1], FD_CONNECT | FD_READ) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
									}

	
									if(connect(SocketArray[1], (sockaddr *) &addrRemoteOut, sizeof(addrRemoteOut))==SOCKET_ERROR){
										if(WSAGetLastError()!=WSAEWOULDBLOCK){
											_stprintf_s(string, sizeof(string), _T("(outSocket) connect failed with error: %d"), WSAGetLastError());
											MessageBox(NULL, string, _T("Error at connect()"), MB_OK);
										}
									}

								}
							}
							if(NetworkEvents.iErrorCode[FD_CONNECT_BIT]==WSAECONNREFUSED){
								MessageBox(NULL,  _T("Conexión rechazada"), _T("Error al conectar"), MB_OK);
								flagWaitForEvents=false;
							}
							if((NetworkEvents.iErrorCode[FD_CONNECT_BIT]!=WSAEADDRINUSE)&&(NetworkEvents.iErrorCode[FD_CONNECT_BIT]!=WSAECONNREFUSED)){
								_stprintf_s(string, sizeof(string), _T("FD_CONNECT failed with error: %d"), NetworkEvents.iErrorCode[FD_CONNECT_BIT]);
								MessageBox(NULL, string, _T("Error at WSAEnumNetworkEvents()"), MB_OK);
							}
						}
						else
						{
							if(Index==0){

								if(WSAEventSelect(SocketArray[0], EventArray[0], FD_READ | FD_WRITE |FD_CLOSE) == SOCKET_ERROR ) {
									_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
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
							}

							if(Index==1){
								
								if(WSAEventSelect(SocketArray[0], EventArray[0], FD_WRITE | FD_CLOSE) == SOCKET_ERROR ) {
									_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
									MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
								}

								if(WSAEventSelect(SocketArray[1], EventArray[1], FD_READ | FD_CLOSE) == SOCKET_ERROR ) {
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
							if(flagCompleteConnection==false){
							
								if(Index==0){

									recv( SocketArray[0], (char *)&addrRemoteOut, sizeof(addrRemoteOut), 0);

									if ((SocketArray[1] = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) == INVALID_SOCKET) {
										_stprintf_s(string,sizeof(string), _T("(inSocket) socket function failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
									}

									if (bind(SocketArray[1], (sockaddr*) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(inSocket) bind failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}

									if ((EventArray[1] = WSACreateEvent()) == WSA_INVALID_EVENT)
									{
										_stprintf_s(string, sizeof(string), _T("(inSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
									}
	
									if(WSAEventSelect(SocketArray[1], EventArray[1],  FD_CONNECT ) == SOCKET_ERROR ) {
										_stprintf_s(string, sizeof(string), _T("(inSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
										MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
									}

									if(connect(SocketArray[1], (sockaddr *) &addrRemoteOut, sizeof(addrRemoteOut))==SOCKET_ERROR){
										if(WSAGetLastError()!=WSAEWOULDBLOCK){
											_stprintf_s(string, sizeof(string), _T("(inSocket) connect failed with error: %d"), WSAGetLastError());
											MessageBox(NULL, string, _T("Error at connect()"), MB_OK);
										}
									}

									EventTotal++;
								}
							}
							else{
								buffer=(char *)malloc(sizeof(HEADER_DATA)+MAX_BUFFER_IN);
								memset(buffer,0,sizeof(HEADER_DATA)+MAX_BUFFER_IN);
								recvData(SocketArray[1], "TCP", buffer, MAX_BUFFER_IN);
								free(buffer);
								firstData=true;
							}
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

					// Process FD_CLOSE notifications
					if (NetworkEvents.lNetworkEvents & FD_CLOSE)
					{
						flagWaitForEvents=false;
					}
				}  // end of else
			}  // end of for

		}  // end of while

		mmTimerEnd(timer);

		shutdown(SocketArray[0], SD_BOTH);
		shutdown(SocketArray[1], SD_BOTH);
	}
	
	if(strcmp(protocol,"UDP")==0){

		UINT code;
		UINT waitTime=2000; //in milliseconds

		if ((SocketArray[0] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) == INVALID_SOCKET) {
			_stprintf_s(string,sizeof(string), _T("(outAddr) socket function failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
		}

		if (bind(SocketArray[0], (sockaddr *) &outAddr, sizeof(outAddr)) == SOCKET_ERROR ) {
			//_stprintf_s(string, sizeof(string), _T("(inAddr) bind failed with error: %d"), WSAGetLastError());
			//MessageBox(hWnd, string, _T("Error at bind()"), MB_OK);
		}

		if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			_stprintf_s(string, sizeof(string), _T("(outSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
		}
	
		if(WSAEventSelect(SocketArray[0], EventArray[0], FD_READ | FD_WRITE) == SOCKET_ERROR ) {
			_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
			MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
		}

		code=CODE_CONNECT;
		buffer=(char *)malloc(sizeof(UINT));
		memset(buffer,0,sizeof(UINT));
		memcpy(&buffer[0], &code, sizeof(UINT));
		sendto(SocketArray[0], buffer, sizeof(UINT), 0, (sockaddr *)&addrRemoteIn, sizeof(addrRemoteIn));
		free(buffer);
		
		EventTotal++;

		flagWaitForEvents=true;

		while(flagWaitForEvents){

			Index=WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, (DWORD)waitTime, FALSE);

			if(Index==WSA_WAIT_TIMEOUT){
				if(flagCompleteConnection==false){
					
					flagWaitForEvents=false;
				}
			}
			else{
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
								if(recvfrom(SocketArray[Index], buffer, sizeof(UINT)+sizeof(HEADER_DATA)+MAX_BUFFER_IN, 0, (sockaddr *)NULL, NULL)!=SOCKET_ERROR){
								
									memcpy(&code, &buffer[0], sizeof(UINT));

									if(code==CODE_ACEPT){
				
										if(Index==0){

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
											else{

												if(WSAEventSelect(SocketArray[0], EventArray[0], FD_WRITE) == SOCKET_ERROR ) {
													_stprintf_s(string, sizeof(string), _T("(outSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
													MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
												}

												memcpy(&addrRemoteOut, &buffer[sizeof(UINT)], sizeof(addrRemoteOut));

												if ((SocketArray[1] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP )) == INVALID_SOCKET) {
													_stprintf_s(string,sizeof(string), _T("(inSocket) socket function failed with error: %d"), WSAGetLastError());
													MessageBox(NULL, string, _T("Error at socket()"), MB_OK);
												}

												if (bind(SocketArray[1], (sockaddr *) &inAddr, sizeof(inAddr)) == SOCKET_ERROR ) {
													//_stprintf_s(string, sizeof(string), _T("(inAddr) bind failed with error: %d"), WSAGetLastError());
													//MessageBox(hWnd, string, _T("Error at bind()"), MB_OK);
												}

												if ((EventArray[1] = WSACreateEvent()) == WSA_INVALID_EVENT)
												{
													_stprintf_s(string, sizeof(string), _T("(inSocket) WSACreateEvent() failed with error: %d"), WSAGetLastError());
													MessageBox(NULL, string, _T("Error at bind()"), MB_OK);
												}
	
												if(WSAEventSelect(SocketArray[1], EventArray[1], FD_READ | FD_WRITE) == SOCKET_ERROR ) {
													_stprintf_s(string, sizeof(string), _T("(inSocket) WSAEventSelect failed with error: %d"), WSAGetLastError());
													MessageBox(NULL, string, _T("Error at WSAEventSelect()"), MB_OK);
												}

												Sleep(10);

												code=CODE_CONNECT;
												free(buffer);
												buffer=(char *)malloc(sizeof(UINT));
												memset(buffer,0,sizeof(UINT));
												memcpy(&buffer[0], &code, sizeof(UINT));
												sendto(SocketArray[1], buffer, sizeof(UINT), 0, (sockaddr *)&addrRemoteOut, sizeof(addrRemoteOut));
												EventTotal++;

											}
										}

										if(Index==1){

											if(WSAEventSelect(SocketArray[1], EventArray[1], FD_READ) == SOCKET_ERROR ) {
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
									
										flagWaitForEvents=false;

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
			}
		}  // end of while

		mmTimerEnd(timer);

		code=CODE_CLOSE;
		buffer=(char *)malloc(sizeof(UINT));
		memset(buffer,0,sizeof(UINT));
		memcpy(&buffer[0], &code, sizeof(UINT));
		sendto(SocketArray[0], buffer, sizeof(UINT), 0, (sockaddr *)&addrRemoteIn, sizeof(addrRemoteIn));
		free(buffer);

	}

	hMainIcon = LoadIcon(hInst,(LPCTSTR)MAKEINTRESOURCE(IDI_DISCONNECT));
	nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
	if(flagCompleteConnection==false){
		LoadString(hInst, IDS_ERRORCONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
		LoadString(hInst, IDS_ERRORCONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
	}
	else{
		LoadString(hInst, IDS_DISCONNECTTITLE,nidApp.szInfoTitle,MAX_LOADSTRING);
		LoadString(hInst, IDS_DISCONNECTBODY,nidApp.szInfo,MAX_LOADSTRING);
	}
	LoadString(hInst, IDS_DISCONNECT,nidApp.szTip,MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_MODIFY, &nidApp);

	flagCompleteConnection=false;

	flagConnection=false;

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
	
	NetWriteShareMemory(MEMREDC1,&pfRedOut);
	WriteShareMemory(MEMDATOSC1,&pfDataOut);

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

	NetReadShareMemory(MEMREDC1,&pfRedIn);

	if(pfRedIn.signalWrite!=prevSignalWrite){

		dataOut = (char *)malloc(sizeof(UINT)+pfRedIn.sizeMemoryOut);
		memset(dataOut,0,sizeof(UINT)+pfRedIn.sizeMemoryOut);

		ReadShareMemory(MEMDATOSC1,&pfDataIn);

		CopyMemory(&dataOut[0],&pfRedIn.sizeMemoryOut,sizeof(UINT));
		CopyMemory(&dataOut[sizeof(UINT)],pfDataIn.dataMemory,pfRedIn.sizeMemoryOut);

		sendData(SocketArray[0], protocol, dataOut, sizeof(UINT)+pfRedIn.sizeMemoryOut);

		free(dataOut);
	}

	prevSignalWrite=pfRedIn.signalWrite;

	return;
}


	