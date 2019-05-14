// Copyright 2007 Novint Technologies, Inc. All rights reserved.
// Available only under license from Novint Technologies, Inc.

// Make sure this header is included only once
#ifndef HAPTICS_H
#define HAPTICS_H

#include <hdl/hdl.h>
#include <hdlu/hdlu.h>
#include <windows.h>

// Blocking values
const bool bNonBlocking = false;
const bool bBlocking = true;

class HapticsClass 
{

// Define callback functions as friends
friend HDLServoOpExitCode ContactCB(void *data);
friend HDLServoOpExitCode GetStateCB(void *data);

public:
    // Constructor
    HapticsClass();

    // Destructor
    ~HapticsClass();

    // Initialize
    void init(int);

    // Clean up
    void uninit();

    // Get position
    void getPosition(double*);

	 // Get velocity
    void getVelocity(double*);

	// Get buttons
    void getButtons(bool*);

    // Set the force XYZ
    void SetForce(double*);

	void setSpring(double*);
	void setDamping(double*);

    // Get state of device button
    bool isButtonDown();

    // synchFromServo() is called from the application thread when it wants to exchange
    // data with the HapticClass object.  HDAL manages the thread synchronization
    // on behalf of the application.
    void synchFromServo();

    // Get ready state of device.
    bool isDeviceCalibrated();

	bool changeParametros;

private:
    // Move data between servo and app variables

	void FiltroDerivadorX(double pfX,double *pfXf);
	void FiltroDerivadorY(double pfX,double *pfXf);
	void FiltroDerivadorZ(double pfX,double *pfXf);

	void FiltroX(double pfX,double *pfXf, double a);
	void FiltroY(double pfX,double *pfXf, double a);
	void FiltroZ(double pfX,double *pfXf, double a);

	void FiltroX1(double pfX,double *pfXf, double a);
	void FiltroY1(double pfX,double *pfXf, double a);
	void FiltroZ1(double pfX,double *pfXf, double a);

	void Saturation(double *pfX, double *pfY , double *pfZ, double fMax);

    void synch();

    // Check error result; display message and abort, if any
    void testHDLError(LPCTSTR str);

    // Nothing happens until initialization is done
    bool m_inited;
    
    // Variables used only by servo thread
    double m_positionServo[3];
	double m_positionFiltServo[3]; //posición del servo después de un filtro pasa bajo
	double m_velocityServo[3];
    bool   m_buttonServo;
	int    m_buttonsServo;
    double m_forceServo[3];


    // Handle to device
    HDLDeviceHandle m_deviceHandle;

    // Handle to Contact Callback 
    HDLServoOpExitCode m_servoOp;

    // Damping
    double m_Damping[3];
	// Spring
    double m_Spring[3];
};

#endif // HAPTICS_H