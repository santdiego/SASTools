#include "triangleApp.h"
#include "VirtualDevice.h"

static VIRTUAL_DEVICE_SALIDA ImagenComprimida;
HANDLE hShare;
int orgSize;
int compSize;

BOOL mostrar_imagenes;

BOOL WriteShareMemory(LPBITMAPINFOHEADER ImagenComp, int CantBytes);
//// CAMBIOS 26/29/2013 /////

//The first device we want to open
int dev = 0;


//empty constructor
triangleApp::triangleApp(){

}

//destructor
triangleApp::~triangleApp(){
	CloseShareMemory();
	ICClose(hicopen);
}

void triangleApp::init(){

	//optional static function to list devices
	//for silent listDevices use listDevices(true);
	int numDevices = videoInput::listDevices();	
	
	if(numDevices > 1)
	{
		do
			printf("Ingrese el numero de dispositivo a usar: ");
		while (1 != scanf("%d", &dev));
	}

	//uncomment for silent setup
	//videoInput::setVerbose(false);

	//we allocate our openGL texture objects
	//we give them a ma size of 1024 by 1024 pixels
	//IT  = new imageTexture(2048,2048, GL_RGB);   
	/*IT  = new imageTexture(640,480, GL_RGB);
	IT2  = new imageTexture(640,480, GL_RGB);*/
	IT  = new imageTexture(ANCHO_IMAGEN,ALTO_IMAGEN, GL_RGB);
	IT2  = new imageTexture(ANCHO_IMAGEN,ALTO_IMAGEN, GL_RGB);
	/*IT  = new imageTexture(320,240, GL_RGB);
	IT2  = new imageTexture(320,240, GL_RGB);*/
	//IT2 = new imageTexture(2048,2048, GL_RGB);  

	//by default we use a callback method
	//this updates whenever a new frame
	//arrives if you are only ocassionally grabbing frames
	//you might want to set this to false as the callback caches the last
	//frame for performance reasons. 
	VI.setUseCallback(true);

	//try and setup device with id 0 and id 1
	//if only one device is found the second 
	//setupDevice should return false

	//if you want to capture at a different frame rate (default is 30) 
	//specify it here, you are not guaranteed to get this fps though.
	VI.setIdealFramerate(dev, 30);

	//we can specifiy the dimensions we want to capture at
	//if those sizes are not possible VI will look for the next nearest matching size	
	//VI.setupDevice(dev,   320, 240, VI_COMPOSITE); 
	//VI.setupDevice(dev,   640, 480, VI_COMPOSITE);
	VI.setupDevice(dev,   ANCHO_IMAGEN, ALTO_IMAGEN, VI_COMPOSITE);
	//VI.setupDevice(dev+1, ANCHO_IMAGEN, ALTO_IMAGEN, VI_COMPOSITE);	
	
	//once the device is setup you can try and
	//set the format - this is useful if your device
	//doesn't remember what format you set it to
	//VI.setFormat(dev, VI_NTSC_M);					//optional set the format

	//we allocate our buffer based on the number
	//of pixels in each frame - this will be width * height * 3
	frame = new unsigned char[VI.getSize(dev)];
	frame2 = new unsigned char[VI.getSize(dev)];
	//frame2 = new unsigned char[VI.getSize(dev+1)];
 
	//// CAMBIOS 26/29/2013 /////
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

	structcam.cbSize=sizeof(COMPVARS);
	structcam.cbState=0;
	structcam.dwFlags=0;
	structcam.fccHandler=NULL;
	structcam.fccType=0;
	structcam.hic=NULL;
	structcam.lDataRate=0;
	structcam.lFrame=0;
	structcam.lKey=0;
	structcam.lKeyCount=0;
	structcam.lpbiIn=NULL;
	structcam.lpbiOut=NULL;
	structcam.lpBitsOut=NULL;
	structcam.lpBitsPrev=NULL;
	structcam.lpState=NULL;
	structcam.lQ=0;

	lQuality=10000;

	if (!ICCompressorChoose(NULL,NULL,&lpbmi,NULL,&structcam,
		"Elija el Compresor para la imagen"))
		MessageBox(NULL,TEXT("Error en elegir el Compresor"),NULL,NULL);
	else
	{
		hicopen=ICOpen(structcam.fccType,structcam.fccHandler,ICMODE_FASTCOMPRESS);
		lQuality=structcam.lQ;
	}
	orgSize=VI.getSize(dev);
	compSize=0;

	mostrar_imagenes = TRUE;

	OpenShareMemory(TEXT(M_COMPARTIDA),0);
	//// CAMBIOS 26/29/2013 /////
}

void triangleApp::idle(){

	Sleep(10);
	BYTE* bits_imagen_comp;
	//check to see if we have got a new frame
	if( VI.isFrameNew(dev) )	
	{
		//we get the pixels by passing in out buffer which gets filled
		VI.getPixels(dev,frame, true);			

		//we then load them into our texture
		IT->loadImageData(frame, VI.getWidth(dev), VI.getHeight(dev),GL_RGB);

		//// CAMBIOS 26/29/2013 /////
		lSize=200000;
		
		compDIB=ICImageCompress(hicopen,0,&lpbmi,frame,NULL,lQuality,&lSize);
		imagen_comp=(LPBITMAPINFOHEADER)GlobalLock(compDIB);
		
		orgSize=VI.getSize(dev);
		//compSize=(int)lSize;
		compSize=(int)(lSize + sizeof (BITMAPINFOHEADER));

		if(compSize < TAM_MAX)
			WriteShareMemory(imagen_comp, compSize);
		
		
		if(mostrar_imagenes){
		//LEO IMAGEN EN MEMORIA COMPARTIDA////
		ReadShareMemory(&ImagenComprimida);

		// Allocate memory.
		//BYTE* bits_imagen_comp = new BYTE[ lSize ];
		//ZeroMemory( bits_imagen_comp, lSize );
		bits_imagen_comp = (BYTE*)malloc(ImagenComprimida.iCantBytesImagenComp);
		ZeroMemory( bits_imagen_comp, ImagenComprimida.iCantBytesImagenComp );

		//memcpy(bits_imagen_comp,(LPVOID)((BYTE *)ImagenArDrone.ucImagenComp + sizeof (BITMAPINFOHEADER)),ImagenArDrone.iCantBytesImagenComp);
		memcpy(bits_imagen_comp,(LPVOID)((BYTE *)ImagenComprimida.ucImagenComp),ImagenComprimida.iCantBytesImagenComp);

		decompDIB=ICImageDecompress(NULL,0,(LPBITMAPINFO)ImagenComprimida.ucImagenComp,bits_imagen_comp,&lpbmi);
		GlobalFree(compDIB);
		imagen_decomp=(LPBITMAPINFOHEADER)GlobalLock(decompDIB);

		memcpy(frame2,(LPVOID)((BYTE *)imagen_decomp + sizeof (BITMAPINFOHEADER)),VI.getSize(dev));

		IT2->loadImageData(frame2, VI.getWidth(dev), 	VI.getHeight(dev), GL_RGB);
		}
		GlobalFree(decompDIB);
		free(bits_imagen_comp);
		//// CAMBIOS 26/29/2013 /////
	}
	
	//check to see if we have got a new frame
	/*if( VI.isFrameNew(dev+1) )						
	{	
		//here we are directly return the pixels into our texture
		//use VI.getWidth getHeight etc so that you don't get a crash
   		IT2->loadImageData(VI.getPixels(dev+1, true), VI.getWidth(dev+1), 	VI.getHeight(dev+1), GL_RGB); 
	}*/
}


void triangleApp::draw(){
  
	setupScreen();
	IT->renderTexture(0, 0, VI.getWidth(dev), VI.getHeight(dev));
	if(mostrar_imagenes){
	IT2->renderTexture(VI.getWidth(dev), 0, VI.getWidth(dev), VI.getHeight(dev));}
}

void triangleApp::keyDown  (char c){

	//some options hooked up to key commands
	if(c=='S')VI.showSettingsWindow(dev);
	//if(c=='D')VI.showSettingsWindow(dev+1);

	if(c=='R')VI.restartDevice(dev);
	//if(c=='T')VI.restartDevice(dev+1);

	/*if(c == '1')VI.setVideoSettingCameraPct(0, VI.propExposure, 0.1f, 2f);
	if(c == '2')VI.setVideoSettingCameraPct(0, VI.propExposure, 0.9f, 2f);	
	
	if(c == '5')VI.setVideoSettingFilterPct(0, VI.propWhiteBalance, 0.12f, 2f);
	if(c == '6')VI.setVideoSettingFilterPct(0, VI.propWhiteBalance, 0.88f, 2f);*/


	if(c=='L'){
	//// CAMBIOS 26/29/2013 /////
	structcam.cbSize=sizeof(COMPVARS);
	structcam.cbState=0;
	structcam.dwFlags=0;
	structcam.fccHandler=NULL;
	structcam.fccType=0;
	structcam.hic=NULL;
	structcam.lDataRate=0;
	structcam.lFrame=0;
	structcam.lKey=0;
	structcam.lKeyCount=0;
	structcam.lpbiIn=NULL;
	structcam.lpbiOut=NULL;
	structcam.lpBitsOut=NULL;
	structcam.lpBitsPrev=NULL;
	structcam.lpState=NULL;
	structcam.lQ=0;

	if (!ICCompressorChoose(NULL,NULL,&lpbmi,NULL,&structcam,
		"Elija el Compresor para la imagen"))
		MessageBox(NULL,TEXT("Error en elegir el Compresor"),NULL,NULL);
	else
	{
		hicopen=ICOpen(structcam.fccType,structcam.fccHandler,ICMODE_FASTCOMPRESS);
		lQuality=structcam.lQ;
	}
	//// CAMBIOS 26/29/2013 /////
	}

	if(c=='C')
	{
		if(mostrar_imagenes)
			mostrar_imagenes = FALSE;
		else
			mostrar_imagenes = TRUE;
	}

	if(c=='Q')
	{
		VI.stopDevice(dev);
		//VI.stopDevice(dev+1);
	}
}

void triangleApp::mouseMove( float x, float y ){

}

void triangleApp::mouseDrag( float x, float y ){

}

void triangleApp::mouseDown( float x, float y, int button ){

}

void triangleApp::mouseUp  ( float x, float y, int button ){

}

