#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <vfw.h>
#include <time.h>
#include <tchar.h>
#include <strsafe.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#pragma comment(lib,"opencv_core249.lib")
#pragma comment(lib,"opencv_highgui249.lib")
#pragma comment(lib,"opencv_imgproc249.lib")

using namespace cv;
using namespace std;

#pragma comment(lib, "Vfw32.lib")

#define ANCHO_IMAGEN					320
#define ALTO_IMAGEN						240