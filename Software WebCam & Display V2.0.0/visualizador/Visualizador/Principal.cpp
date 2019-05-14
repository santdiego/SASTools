#include "Cabecera.h"
#include "VirtualDevice.h"
#include "Mmtimer.h"

//----------------------
// Create los Hilos.
HANDLE hHiloDescomprimir = NULL;
HANDLE hHiloVisualizar = NULL;

HANDLE gImagenRecibidaEvent;
HANDLE gImagenDescomprimidaEvent;

int iImageSize;
BOOL imagen_lista;
LPBYTE m_pData;
BITMAPINFO lpbmi;
BYTE bits_imagen_decomp[ANCHO_IMAGEN*ALTO_IMAGEN*3];
BYTE bits_imagen_decomp2[ANCHO_IMAGEN*ALTO_IMAGEN*3];
char recvbuf[TAM_MAX];
int iImgRec;
int iImgCom;
int iImgVis;
int iBytesImg;
float fFpsRec;
float fFpsCom;
float fFpsVis;
MMTIMER timer;
bool bImgProc=true;

DWORD WINAPI HiloVisualizar(LPVOID lpParameter);
DWORD WINAPI HiloDescomprimir(LPVOID lpParameter);
void ErrorHandler(LPTSTR lpszFunction);
void CierraTodo(void);
void EscribirConsola(void);
void gotoxy(int x, int y);
void CALLBACK timerProc(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5);

int wmain(void)
{
	SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);

	imagen_lista = TRUE;

	// Use an event object to track the TimerRoutine execution
    gImagenRecibidaEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == gImagenRecibidaEvent)
    {
		ErrorHandler(TEXT("CreateEvent failed"));
		CierraTodo();
        return 1;
    }

	gImagenDescomprimidaEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == gImagenDescomprimidaEvent)
    {
		ErrorHandler(TEXT("CreateEvent failed"));
		CierraTodo();
        return 1;
    }

	OpenShareMemory(TEXT(M_COMPARTIDA),0);
	OpenControlShareMemory(TEXT(M_COMPARTIDA),0);

	timer.uiPeriod = 1;
	timer.lpProc = timerProc;
	timer.dwData = (DWORD)NULL;

	if(!mmTimerStart(&timer)){
		MessageBox(NULL,TEXT("mmTimerStart human 1 function failed"),TEXT("Error"),MB_OK);
	}

	//Inicializo los hilos, los eventos y el Timer Multimedia
	DWORD dwHiloIdDescomprimir;
	DWORD dwHiloIdVisualizar;

	// *******Crea el hilo de para Enviar *********************//
	
	// *******Crea el hilo de para Descomprimir *********************//
	
	hHiloDescomprimir = CreateThread(
		NULL,				// pointer to security attributes
		0,					// initial thread stack size
		HiloDescomprimir,		// pointer to thread function
		(LPVOID)NULL,		// argument for new thread
		0,					// creation flags
		&dwHiloIdDescomprimir	// pointer to receive thread ID
		);
	if (hHiloDescomprimir == NULL)
	{
		ErrorHandler(TEXT("CreateThread:HiloDescomprimir"));
        CierraTodo();
        return 1;
	}

	// *******Crea el hilo de para Visualizar *********************//
	
	hHiloVisualizar = CreateThread(
		NULL,				// pointer to security attributes
		0,					// initial thread stack size
		HiloVisualizar,		// pointer to thread function
		(LPVOID)NULL,		// argument for new thread
		0,					// creation flags
		&dwHiloIdVisualizar	// pointer to receive thread ID
		);
	if (hHiloVisualizar == NULL)
	{
		ErrorHandler(TEXT("CreateThread:HiloVisualizar"));
        CierraTodo();
        return 1;
	}

	char index;

	//Tecla para finalizar
	do {
		EscribirConsola();
		index = getchar();
    } while (index != 'q');
	
    CierraTodo();
    return 0;
}

void gotoxy(int x, int y)
{ 
    COORD pos = {x, y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

void ErrorHandler(LPTSTR lpszFunction)
{ 
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

DWORD WINAPI HiloVisualizar(LPVOID lpParameter)
{
	clock_t timer0, timer;

	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
	
	//Mat im_bgr(Size(ANCHO_IMAGEN,ALTO_IMAGEN),CV_8UC3);

	namedWindow("Imagen recibida", CV_WINDOW_NORMAL);
	//namedWindow("Imagen recibida");

	//RESETEO EL CONTADOR DE LOS CUADROS POR SEGUNDO//
	iImgVis=0;
	timer0 = clock();

	for(;;)
	{
		if (WaitForSingleObject(gImagenDescomprimidaEvent, INFINITE) != WAIT_OBJECT_0)		
		{//Timer que espera hasta que se cuempla el tiempo establecido
			printf("WaitForSingleObject failed (%d)\n", GetLastError());
			CierraTodo();
			return 1;
		}
		ResetEvent(gImagenDescomprimidaEvent);

		Mat im_rgb(Size(ANCHO_IMAGEN,ALTO_IMAGEN),CV_8UC3,bits_imagen_decomp2);

		//cvtColor( im_rgb, im_bgr, CV_RGB2BGR);
		//flip(im_bgr,im_bgr,0);

		cvtColor( im_rgb, im_rgb, CV_RGB2BGR);
		flip(im_rgb,im_rgb,0);

		resize(im_rgb, im_rgb, Size(), 640.0/ANCHO_IMAGEN, 480.0/ALTO_IMAGEN);

		/*if(CV_WINDOW_AUTOSIZE == getWindowProperty("Imagen recibida", CV_WND_PROP_AUTOSIZE))
			setWindowProperty("Imagen recibida", CV_WND_PROP_AUTOSIZE, CV_WINDOW_NORMAL);  // NO FUNCIONA, NECESITA QT*/

		imshow("Imagen recibida", im_rgb);

		cvWaitKey(1);

		iImgVis++;
		timer = clock() - timer0;

		fFpsVis = (float)iImgVis * (float)CLOCKS_PER_SEC / (float)timer;

		if((double)timer > (1.0 * (double)CLOCKS_PER_SEC))
		{
			iImgVis = 0;
			timer0 = clock();
		}
		imagen_lista = TRUE;
		//SetEvent(gDoneEvent);
	}
}

DWORD WINAPI HiloDescomprimir(LPVOID lpParameter)
{
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);

	clock_t timer0, timer;

	//clock_t start, finish;
	//double  duration;

	lpbmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbmi.bmiHeader.biWidth = ANCHO_IMAGEN;
	lpbmi.bmiHeader.biHeight = ALTO_IMAGEN;
	lpbmi.bmiHeader.biPlanes = 1;
	lpbmi.bmiHeader.biBitCount = 24;
	lpbmi.bmiHeader.biCompression = BI_RGB;
	lpbmi.bmiHeader.biSizeImage = 0;
	lpbmi.bmiHeader.biXPelsPerMeter = 0;
	lpbmi.bmiHeader.biYPelsPerMeter = 0;
	lpbmi.bmiHeader.biClrUsed = 0;
	lpbmi.bmiHeader.biClrImportant = 0;

	HANDLE decompDIB;
	LPBITMAPINFOHEADER imagen_decomp;

	//RESETEO EL CONTADOR DE LOS CUADROS POR SEGUNDO//
	iImgCom=0;
	timer0 = clock();

	for(;;){
		if (WaitForSingleObject(gImagenRecibidaEvent, INFINITE) != WAIT_OBJECT_0)		
		{//Timer que espera hasta que se cuempla el tiempo establecido
			printf("WaitForSingleObject failed (%d)\n", GetLastError());
			CierraTodo();
			return 1;
		}
		ResetEvent(gImagenRecibidaEvent);	
		//start = clock();

		//iImageSize2=iImageSize;

		m_pData = new BYTE[iImageSize];
		//copiamos los datos a otro buffer
		memcpy(m_pData, recvbuf, iImageSize);

		bImgProc=true;

		//descomprimimos....
		decompDIB=ICImageDecompress(NULL,0,(LPBITMAPINFO)m_pData,(LPVOID)(m_pData + sizeof (BITMAPINFOHEADER)),&lpbmi);
		delete[] m_pData;

		if (decompDIB != NULL){
		imagen_decomp=(LPBITMAPINFOHEADER)GlobalLock(decompDIB);

		memcpy(bits_imagen_decomp,(LPVOID)((BYTE *)imagen_decomp + sizeof (BITMAPINFOHEADER)),lpbmi.bmiHeader.biWidth*lpbmi.bmiHeader.biHeight*3);
		GlobalFree(decompDIB);

		if(imagen_lista)
		{
			//iImageSize3=iImageSize2;
			memcpy(bits_imagen_decomp2,bits_imagen_decomp,lpbmi.bmiHeader.biWidth*lpbmi.bmiHeader.biHeight*3);
			imagen_lista = FALSE;
			SetEvent(gImagenDescomprimidaEvent);
		}

		iImgCom++;
		timer = clock() - timer0;

		fFpsCom = (float)iImgCom * (float)CLOCKS_PER_SEC / (float)timer;

		if((double)timer > (1.0 * (double)CLOCKS_PER_SEC))
		{
			iImgCom = 0;
			timer0 = clock();
		}
		}
	}
	return 0;
}

void EscribirConsola(void)
{
	system("cls");
	gotoxy(0,0);
	printf(" ================================= \n");
	printf("|  Programa para recibir imagenes |\n");
	printf("|    FPS = Cuadros por segundo    |\n");
	printf(" ================================= \n\n");
	printf("Ingrese 'q' para finalizar\n\n");
	printf("Tamano en bytes x imagen -> %d\n",(int)((float)iBytesImg/(float)iImgRec));
	printf("Imagenes recibidas x seg      = %.2f\n",fFpsRec);
	printf("Imagenes descomprimidas x seg = %.2f\n",fFpsCom);
	printf("Imagenes visualizadas x seg   = %.2f\n",fFpsVis);
	gotoxy(0,20);
}

void CierraTodo()
{
	mmTimerEnd(timer);

	CloseShareMemory();
	CloseControlShareMemory();

	if (hHiloDescomprimir != NULL)
		CloseHandle(hHiloDescomprimir);
	if (hHiloVisualizar != NULL)
		CloseHandle(hHiloVisualizar);

	CloseHandle(gImagenRecibidaEvent);
	CloseHandle(gImagenDescomprimidaEvent);

    // Delete all timers in the timer queue.
    /*if (!DeleteTimerQueue(hTimerQueue))
		ErrorHandler(TEXT("DeleteTimerQueue failed"));*/
}


void CALLBACK timerProc(UINT Dummy1, UINT Dummy2, DWORD dwUser,DWORD Dummy4, DWORD Dummy5){

	static VIRTUAL_DEVICE_CONTROL pfControlIn={0};
	static VIRTUAL_DEVICE_ENTRADA pfDataIn={0};
	static bool prevSignalRead=0;
	static	clock_t timer0, timer;

	ReadControlShareMemory(&pfControlIn);

	if((pfControlIn.signalRead!=prevSignalRead)&&(bImgProc==true)){

		bImgProc=false;

		ReadShareMemory(&pfDataIn);

		iImageSize=pfDataIn.iCantBytesImagenComp;
		CopyMemory(recvbuf,pfDataIn.ucImagenComp,iImageSize);

		SetEvent(gImagenRecibidaEvent);

		iImgRec++;
		iBytesImg+=iImageSize;
		timer = clock() - timer0;

		fFpsRec = (float)iImgRec * (float)CLOCKS_PER_SEC / (float)timer;

		if((double)timer > (1.0 * (double)CLOCKS_PER_SEC))
		{
			EscribirConsola(); //Actualizo valores en la consola
			//RESETEO EL CONTADOR DE LOS CUADROS POR SEGUNDO//
			iImgRec = 0;
			iBytesImg=0;
			timer0 = clock();
		}
	}

	prevSignalRead=pfControlIn.signalRead;

	return;
}

