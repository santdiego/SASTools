#ifndef _TRIANGLE_APP

#include <stdio.h>
#include "glfw.h"
#include "simpleApp.h"
#include "imageTexture.h"
#include "videoInput.h"
#include <vfw.h>

//// CAMBIOS 03/10/2013 /////
#define ANCHO_IMAGEN	320
#define ALTO_IMAGEN		240
//// CAMBIOS 03/10/2013 /////

class triangleApp : public simpleApp{
	
	public:
		triangleApp();

		// Destructor
		~triangleApp();

		virtual void idle();
		virtual void init();
		virtual void draw();
		virtual void keyDown  (char c);
		virtual void mouseMove( float x, float y );
		virtual void mouseDrag( float x, float y );
		virtual void mouseDown( float x, float y, int button );
		virtual void mouseUp  ( float x, float y, int button );
		

		imageTexture * IT;
		imageTexture * IT2;
		videoInput     VI;

		unsigned char * frame;
		unsigned char * frame2;

		//// CAMBIOS 26/29/2013 /////
		COMPVARS structcam;
		HIC hicopen;
		HANDLE compDIB;
		HANDLE decompDIB;
		LONG lSize;
		LONG lQuality;
		BITMAPINFO lpbmi;
		LPBITMAPINFOHEADER imagen_comp;
		LPBITMAPINFOHEADER imagen_decomp;
		//// CAMBIOS 26/29/2013 /////
		
};

#endif	// _triangle_APP