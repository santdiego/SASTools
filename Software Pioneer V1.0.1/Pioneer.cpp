#include "Pioneer.h"
#include "VirtualDevice.h"
#include "resource.h"
#include "Device.h"

// The robot object, with which we must interact
ArRobot robot;
ArRobotPacket paquete;
ArSick sick;					//Se crea el objeto sick para manejar el laser
bool useSim;
extern HANDLE  hEventoLeerPID;	// Handle del evento LeerParamPID
extern STATE State;				//state data
int nTcpPort; 
extern bool memoria_abierta;
extern void ErrorHandler(LPTSTR lpszFunction);
extern NOTIFYICONDATA nidApp;

DWORD WINAPI HiloPioneer( LPVOID lpParam )
{
	ArTcpConnection tcpConn;		//Para conectarse al simulador (si esta activo)
	ArSerialConnection serConn;		//Para conectarse a través del puerto serie al robot
	ArSerialConnection laserCon;	//Laser connection

	// Inicialización del software Aria
	Aria::init();
	
	// First we see if we can open the tcp connection, if we can we'll
	// assume we're connecting to the sim, and just go on...  if we
	// can't open the tcp it means the sim isn't there, so just try the
	// robot

	// modify this next line if you're not using default tcp connection
    
	nTcpPort=0;
	useSim=false;

	while(nTcpPort<CANTIDAD_PIONEER)
	{
		tcpConn.setPort(NULL,8101+nTcpPort);
		if(tcpConn.internalOpen() == 0)
		{
			useSim=true;
			// we could get to the sim, so set the robots device connection to the sim
			//printf("Connecting to simulator through tcp.\n");
			robot.setDeviceConnection(&tcpConn);
			
			//----------------------
			// Inicializar la memoria compartida
			if(TRUE!=OpenShareMemory(TEXT(M_COMPARTIDA),nTcpPort))
			{
				ErrorHandler(TEXT(M_COMPARTIDA));
				Aria::shutdown();
				Aria::uninit();
				SendNotifyMessage(nidApp.hWnd,WM_COMMAND,ID_STOP,0);
				return 0;
			}
			else
				memoria_abierta=true;
			break;
		}
		else
			nTcpPort++;
	}

	if(!useSim)
	{
		// we couldn't get to the sim, so set the port on the serial
		// connection and then set the serial connection as the robots
		// device

		// modify the next line if you're not using the first serial port
		// to talk to your robot
		serConn.setPort();
		//printf(
		//"Could not connect to simulator, connecting to robot through serial.\n");
		robot.setDeviceConnection(&serConn);


		//----------------------
		// Inicializar la memoria compartida
		if(TRUE!=OpenShareMemory(TEXT(M_COMPARTIDA),0))
		{
			ErrorHandler(TEXT(M_COMPARTIDA));
			Aria::shutdown();
			Aria::uninit();
			SendNotifyMessage(nidApp.hWnd,WM_COMMAND,ID_STOP,0);
			return 0;
		}
		else
			memoria_abierta=true;

		nTcpPort = 0; //Por las dudas de que sea necesario después
	}

	if(!robot.blockingConnect())
	{
		Aria::shutdown();
		Aria::uninit();
		nTcpPort = -1;
		SendNotifyMessage(nidApp.hWnd,WM_COMMAND,ID_STOP,0);
		return 0;
	}
	else
	{
		StringCchPrintf( State.mensaje, 128, TEXT("Pioneer Nº%d activo"), nTcpPort+1);
		StringCchCopy(nidApp.szTip, ARRAYSIZE(nidApp.szTip), State.mensaje);
		Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
	}

	// If the robot has an Analog Gyro, this object will activate it, and 
	// if the robot does not automatically use the gyro to correct heading,
	// this object reads data from it and corrects the pose in ArRobot
	//ArAnalogGyro gyro(&robot);

	// Attach sonarDev to the robot so it gets data from it.
	//robot.addRangeDevice(&sonarDev);

	//Tiempo de muestreo a 50 ms por default
	robot.setCycleTime(DEFAULT_TIEMPO_MUESTREO_PIONEER);

	// Start the robot task loop running in a new background thread. The 'true' argument means if it loses
	// connection the task loop stops and the thread exits.
	robot.runAsync(true);

	// Sleep for a second so some messages from the initial responses
	// from robots and cameras and such can catch up
	ArUtil::sleep(100);

	// We need to lock the robot since we'll be setting up these modes
	// while the robot task loop thread is already running, and they 
	// need to access some shared data in ArRobot.
	robot.lock();

	///Máxima velocidad del robot a 700 m/s (El sonar no
	//funciona correctamente a velocidades mayores)
	robot.setTransVelMax(700.0);
	robot.setRotVelMax(140.0);
	//////////reseteamos la posición del robot a (0,0,0)///////
	robot.com(ArCommands::SETO);
	if(useSim)
		robot.com(ArCommands::SIM_RESET);
	////velocidades iniciales nulas///////
	robot.setVel(0.0);
	robot.setRotVel(0.0);
	//Habilitamos los encoders
	robot.comInt(ArCommands::ENCODER,2);
	robot.requestEncoderPackets();

	robot.addRangeDevice(&sick);
	sick.configureShort(useSim, ArSick::BAUD38400, ArSick::DEGREES180, ArSick::INCREMENT_ONE);
	laserCon.setPort(ArUtil::COM3);
	sick.setDeviceConnection(&laserCon);
	// now that we're connected to the robot, connect to the laser
	sick.runAsync();
	
	// connect to the laser
	if(!sick.asyncConnect())
	{
		Aria::shutdown();
		Aria::uninit();
		SendNotifyMessage(nidApp.hWnd,WM_COMMAND,ID_STOP,0);
		return 0;
	}

	// turn on the motors
	robot.comInt(ArCommands::ENABLE, 1);

	ArRobotConfigPacketReader configPack(&robot, false);

	robot.unlock();

	while(robot.isConnected())
	{
		WaitForSingleObject(hEventoLeerPID,INFINITE);
		
		if(robot.isConnected())
		{
			configPack.requestPacket();
			configPack.packetHandler(&paquete);
			State.iParametros[0] = configPack.getTransKP();
			State.iParametros[1] = configPack.getTransKV();
			State.iParametros[2] = configPack.getTransKI();
	
			State.iParametros[3] = configPack.getRotKP();
			State.iParametros[4] = configPack.getRotKV();
			State.iParametros[5] = configPack.getRotKI();
		}
		ResetEvent(hEventoLeerPID);
	}
	
	Aria::shutdown();
	Aria::uninit();
	SendNotifyMessage(nidApp.hWnd,WM_COMMAND,ID_STOP,0);
	return 0;
}