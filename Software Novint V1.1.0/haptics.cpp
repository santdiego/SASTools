#include "haptics.h"
#include <windows.h>
#include <math.h>

extern bool haptic_device;

#include <strsafe.h>
#include "resource.h"
extern HWND hwndJoy1;

// Continuous servo callback function
HDLServoOpExitCode ContactCB(void* pUserData)
{
    // Get pointer to haptics object
    HapticsClass* haptics = static_cast< HapticsClass* >( pUserData );

    // Get current state of haptic device
    hdlToolPosition(haptics->m_positionServo);
    hdlToolButton(&(haptics->m_buttonServo));
	hdlToolButtons(&(haptics->m_buttonsServo));

	haptics->FiltroX(haptics->m_positionServo[0],&haptics->m_positionFiltServo[0],1);//0.125
	haptics->FiltroY(haptics->m_positionServo[1],&haptics->m_positionFiltServo[1],1);
	haptics->FiltroZ(haptics->m_positionServo[2],&haptics->m_positionFiltServo[2],1);

    // Estimated value of the velocity from position
	haptics->FiltroDerivadorX(haptics->m_positionFiltServo[0],&haptics->m_velocityServo[0]);
	haptics->FiltroDerivadorY(haptics->m_positionFiltServo[1],&haptics->m_velocityServo[1]);
	haptics->FiltroDerivadorZ(haptics->m_positionFiltServo[2],&haptics->m_velocityServo[2]);

	haptics->FiltroX1(haptics->m_velocityServo[0],&haptics->m_velocityServo[0],0.01);
	haptics->FiltroY1(haptics->m_velocityServo[1],&haptics->m_velocityServo[1],0.01);
	haptics->FiltroZ1(haptics->m_velocityServo[2],&haptics->m_velocityServo[2],0.01);

    // Send forces to device
    hdlSetToolForce(haptics->m_forceServo);

    // Make sure to continue processing
    return HDL_SERVOOP_CONTINUE;
}

void HapticsClass::Saturation(double *pfX, double *pfY , double *pfZ, double fMax)
{
	if(*pfX > fMax)
		*pfX = fMax;
	if(*pfX < -fMax)
		*pfX = -fMax;
	if(*pfY > fMax)
		*pfY = fMax;
	if(*pfY < -fMax)
		*pfY = -fMax;
	if(*pfZ > fMax)
		*pfZ = fMax;
	if(*pfZ < -fMax)
		*pfZ = -fMax;
}

void HapticsClass::FiltroX(double pfX,double *pfXf, double a)
{

static double fX[2] = {0.0, 0.0}; 

fX[0] = pfX;

*pfXf = a*fX[0]+(1-a)*fX[1];

fX[1] = *pfXf;
}

void HapticsClass::FiltroY(double pfX,double *pfXf, double a)
{

static double fX[2] = {0.0, 0.0}; 

fX[0] = pfX;

*pfXf = a*fX[0]+(1-a)*fX[1];

fX[1] = *pfXf;
}

void HapticsClass::FiltroZ(double pfX,double *pfXf, double a)
{

static double fX[2] = {0.0, 0.0}; 

fX[0] = pfX;

*pfXf = a*fX[0]+(1-a)*fX[1];

fX[1] = *pfXf;
}

void HapticsClass::FiltroX1(double pfX,double *pfXf, double a)
{

static double fX[2] = {0.0, 0.0}; 

fX[0] = pfX;

*pfXf = a*fX[0]+(1-a)*fX[1];

fX[1] = *pfXf;
}

void HapticsClass::FiltroY1(double pfX,double *pfXf, double a)
{

static double fX[2] = {0.0, 0.0}; 

fX[0] = pfX;

*pfXf = a*fX[0]+(1-a)*fX[1];

fX[1] = *pfXf;
}

void HapticsClass::FiltroZ1(double pfX,double *pfXf, double a)
{

static double fX[2] = {0.0, 0.0}; 

fX[0] = pfX;

*pfXf = a*fX[0]+(1-a)*fX[1];

fX[1] = *pfXf;
}

void HapticsClass::FiltroDerivadorX(double pfX,double *pfXf)
{

static double fX[9];

fX[0] = pfX;

*pfXf = 1000.0*0.0625*(fX[1]+fX[2]+fX[3]+fX[4]-fX[5]-fX[6]-fX[7]-fX[8]);

fX[8] = fX[7];
fX[7] = fX[6];
fX[6] = fX[5];
fX[5] = fX[4];
fX[4] = fX[3];
fX[3] = fX[2];
fX[2] = fX[1];
fX[1] = fX[0];
}

void HapticsClass::FiltroDerivadorY(double pfX,double *pfXf)
{


static double fX[9];

fX[0] = pfX;

*pfXf = 1000.0*0.0625*(fX[1]+fX[2]+fX[3]+fX[4]-fX[5]-fX[6]-fX[7]-fX[8]);

fX[8] = fX[7];
fX[7] = fX[6];
fX[6] = fX[5];
fX[5] = fX[4];
fX[4] = fX[3];
fX[3] = fX[2];
fX[2] = fX[1];
fX[1] = fX[0];
}

void HapticsClass::FiltroDerivadorZ(double pfX,double *pfXf)
{

static double fX[9];

fX[0] = pfX;

*pfXf = 1000.0*0.0625*(fX[1]+fX[2]+fX[3]+fX[4]-fX[5]-fX[6]-fX[7]-fX[8]);   //(1/0.001)*(1/16)=62.5

fX[8] = fX[7];
fX[7] = fX[6];
fX[6] = fX[5];
fX[5] = fX[4];
fX[4] = fX[3];
fX[3] = fX[2];
fX[2] = fX[1];
fX[1] = fX[0];
}

// On-demand synchronization callback function
HDLServoOpExitCode GetStateCB(void* pUserData)
{
    // Get pointer to haptics object
    HapticsClass* haptics = static_cast< HapticsClass* >( pUserData );

    // Call the function that copies data between servo side 
    // and client side
   // haptics->synch();
    // Only do this once.  The application will decide when it
    // wants to do it again, and call CreateServoOp with
    // bBlocking = true
    return HDL_SERVOOP_EXIT;
}

// Constructor--just make sure needed variables are initialized.
HapticsClass::HapticsClass()
    : m_deviceHandle(HDL_INVALID_HANDLE),
      m_servoOp(HDL_INVALID_HANDLE),
      m_inited(false)
{
    for (int i = 0; i < 3; i++)
        m_positionServo[i] = 0;
}

// Destructor--make sure devices are uninited.
HapticsClass::~HapticsClass()
{
    uninit();
}

void HapticsClass::init(int dispositivo)
{
    HDLError err = HDL_NO_ERROR;

    // Passing "DEFAULT" or 0 initializes the default device based on the
    // [DEFAULT] section of HDAL.INI.   The names of other sections of HDAL.INI
    // could be passed instead, allowing run-time control of different devices
    // or the same device with different parameters.  See HDAL.INI for details.
    //m_deviceHandle = hdlInitNamedDevice("DEFAULT");

	if(hdlCountDevices()>1){
		m_deviceHandle = hdlInitIndexedDevice(dispositivo);
	}else{
		m_deviceHandle = hdlInitNamedDevice("DEFAULT");
	}
	
    testHDLError(TEXT("hdlInitDevice"));

    if (m_deviceHandle == HDL_INVALID_HANDLE)
    {
        MessageBox(NULL,TEXT("Could not open device"), TEXT("Device Failure"), MB_OK);
        haptic_device=FALSE;
    }else{

		haptic_device=TRUE;
	}


	if(haptic_device==TRUE){
    // Now that the device is fully initialized, start the servo thread.
    // Failing to do this will result in a non-funtional haptics application.
    hdlStart();
    testHDLError(TEXT("hdlStart"));

    // Set up callback function
    m_servoOp = hdlCreateServoOp(ContactCB, this, bNonBlocking);
    if (m_servoOp == HDL_INVALID_HANDLE)
    {
        MessageBox(NULL,TEXT("Invalid servo op handle"),TEXT("Device Failure"), MB_OK);
    }
    testHDLError(TEXT("hdlCreateServoOp"));

    // Make the device current.  All subsequent calls will
    // be directed towards the current device.
    hdlMakeCurrent(m_deviceHandle);
    testHDLError(TEXT("hdlMakeCurrent"));

    // Get the extents of the device workspace.
    // Used to create the mapping between device and application coordinates.
    // Returned dimensions in the array are minx, miny, minz, maxx, maxy, maxz
    //                                      left, bottom, far, right, top, near)
    // Right-handed coordinates:
    //   left-right is the x-axis, right is greater than left
    //   bottom-top is the y-axis, top is greater than bottom
    //   near-far is the z-axis, near is greater than far
    // workspace center is (0,0,0)
    //hdlDeviceWorkspace(m_workspaceDims);
    //testHDLError("hdlDeviceWorkspace");

	changeParametros = false;

    m_inited = true;
	}
}

// uninit() undoes the setup in reverse order.  Note the setting of
// handles.  This prevents a problem if uninit() is called
// more than once.
void HapticsClass::uninit()
{
    if (m_servoOp != HDL_INVALID_HANDLE)
    {
        hdlDestroyServoOp(m_servoOp);
        m_servoOp = HDL_INVALID_HANDLE;
    }
    hdlStop();
    if (m_deviceHandle != HDL_INVALID_HANDLE)
    {
        hdlUninitDevice(m_deviceHandle);
        m_deviceHandle = HDL_INVALID_HANDLE;
    }
    m_inited = false;
}

// This is a simple function for testing error returns.  A production
// application would need to be more sophisticated than this.
void HapticsClass::testHDLError(LPCTSTR str)
{
    HDLError err = hdlGetError();
    if (err != HDL_NO_ERROR)
    {
        MessageBox(NULL, str, TEXT("HDAL ERROR"), MB_OK);
        abort();
    }
}

// This is the entry point used by the application to synchronize
// data access to the device.  Using this function eliminates the 
// need for the application to worry about threads.
void HapticsClass::synchFromServo()
{
    hdlCreateServoOp(GetStateCB, this, bBlocking);
}

// GetStateCB calls this function to do the actual data movement.
void HapticsClass::synch()
{
    // m_positionApp is set in cubeContact().
    //m_buttonApp = m_buttonServo;
}



// Here is where the heavy calculations are done.  This function is
// called from ContactCB to calculate the forces based on current
// cursor position and cube dimensions.  A simple spring model is
// used.
void HapticsClass::SetForce(double fuerza[3])
{
	double forceServoAux[3] = {0.0,0.0,0.0};
	double DampingAux[3];
	//TCHAR aux_mensaje[32];

    // Skip the whole thing if not initialized
    if (!m_inited) return;


	double k_zonaZ = 100.0;
	double k_zonaXY = 25.0;
	
	/*if(m_positionServo[2]>0.0)
	{
		DampingAux[0]=m_Damping[0]*1.0/cosh(k_zonaZ*m_positionServo[2])*1.0/cosh(k_zonaXY*m_positionServo[1])*1.0/cosh(k_zonaXY*m_positionServo[0]);
		DampingAux[1]=m_Damping[1]*1.0/cosh(k_zonaZ*m_positionServo[2])*1.0/cosh(k_zonaXY*m_positionServo[1])*1.0/cosh(k_zonaXY*m_positionServo[0]);
		DampingAux[2]=m_Damping[2]*1.0/cosh(k_zonaZ*m_positionServo[2])*1.0/cosh(k_zonaXY*m_positionServo[1])*1.0/cosh(k_zonaXY*m_positionServo[0]);
	}
	else
	{
		DampingAux[0]=m_Damping[0]*1.0/cosh(k_zonaXY*m_positionServo[1])*1.0/cosh(k_zonaXY*m_positionServo[0]);
		DampingAux[1]=m_Damping[1]*1.0/cosh(k_zonaXY*m_positionServo[1])*1.0/cosh(k_zonaXY*m_positionServo[0]);
		DampingAux[2]=m_Damping[2]*1.0/cosh(k_zonaXY*m_positionServo[1])*1.0/cosh(k_zonaXY*m_positionServo[0]);
	}*/
	
	DampingAux[0]=m_Damping[0];
	DampingAux[1]=m_Damping[1];
	DampingAux[2]=m_Damping[2];
	
	/*StringCchPrintf( aux_mensaje, 128, TEXT("%2.4f"), (float)DampingAux[0]);
	SetDlgItemText(hwndJoy1,IDC_BUTTONS,aux_mensaje);*/

	forceServoAux[0]=fuerza[0]-m_Spring[0]*100.0*m_positionServo[0]-DampingAux[0]*100.0*m_velocityServo[0];
	forceServoAux[1]=fuerza[1]-m_Spring[1]*100.0*m_positionServo[1]-DampingAux[1]*100.0*m_velocityServo[1];
	forceServoAux[2]=fuerza[2]-m_Spring[2]*100.0*m_positionServo[2]-DampingAux[2]*100.0*m_velocityServo[2];

	Saturation(&forceServoAux[0],&forceServoAux[1],&forceServoAux[2],9.5);
	m_forceServo[0]=forceServoAux[0];
	m_forceServo[1]=forceServoAux[1];
	m_forceServo[2]=forceServoAux[2];
}

// Interface function to get current position
void HapticsClass::getPosition(double pos[3])
{
    pos[0] = 100.0*m_positionServo[0];
    pos[1] = 100.0*m_positionServo[1];
    pos[2] = 100.0*m_positionServo[2];
}

// Interface function to get current velocity
void HapticsClass::getVelocity(double vel[3])
{
    vel[0] = 100.0*m_velocityServo[0];
    vel[1] = 100.0*m_velocityServo[1];
    vel[2] = 100.0*m_velocityServo[2];
}

// Interface function to get current buttons state
void HapticsClass::getButtons(bool bot[4])
{
	if(m_buttonsServo & HDL_BUTTON_1)
		bot[0]=true;
	else
		bot[0]=false;

	if(m_buttonsServo & HDL_BUTTON_2)
		bot[1]=true;
	else
		bot[1]=false;
    
	if(m_buttonsServo & HDL_BUTTON_3)
		bot[2]=true;
	else
		bot[2]=false;

	if(m_buttonsServo & HDL_BUTTON_4)
		bot[3]=true;
	else
		bot[3]=false;
}

// Interface function to get button state.  Only one button is used
// in this application.
bool HapticsClass::isButtonDown()
{
    return m_buttonServo;
}

// For this application, the only device status of interest is the
// calibration status.  A different application may want to test for
// HDAL_UNINITIALIZED and/or HDAL_SERVO_NOT_STARTED
bool HapticsClass::isDeviceCalibrated()
{
    unsigned int state = hdlGetState();

    return ((state & HDAL_NOT_CALIBRATED) == 0);
}

void HapticsClass::setSpring(double *K)
{
	m_Spring[0]=K[0];
	m_Spring[1]=K[1];
	m_Spring[2]=K[2];
}


void HapticsClass::setDamping(double *B)
{
	m_Damping[0]=B[0];
	m_Damping[1]=B[1];
	m_Damping[2]=B[2];
}
	
