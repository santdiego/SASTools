#include "Util.h"

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

void readFile(char *port, _u32 *baudrate, _u8 *ts, _u16 *mode, _u16 *rpm) {
	char nameFile[50] = "";
	FILE *file;
	errno_t err;
	sprintf_s(nameFile, size_t(sizeof(nameFile)),"Config.txt");
	err = fopen_s(&file, nameFile, "r");
	if (err == 0)
	{
		fscanf_s(file, "%s", port, 14);
		fscanf_s(file, "\n%" SCNu32, baudrate);
		fscanf_s(file, "\n%" SCNu8, ts);
		fscanf_s(file, "\n%" SCNu16, mode);
		fscanf_s(file, "\n%" SCNu16, rpm);
		fclose(file);
	}
}

void ctrlc(int)
{
	//printf("SIGINT detected\n");
	ctrl_c_pressed = 1;
}

void window_cross(int)
{
	//printf("SIGBREAK detected\n");
	window_cross_pressed = 1;
}
using namespace rp::standalone::rplidar;

bool checkRPLIDARHealth(RPlidarDriver * drv)
{
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;


    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        } else {
            return true;
        }

    } else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

int main(void)
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	setlocale(LC_ALL, "spanish");//Habilitar UNICODE en consola

	stop_signal = false;

	HANDLE Timer;
	HANDLE TimerQueue;

	EventoTimer = CreateEvent(
		NULL,				// no security attributes
		TRUE,				// manual-reset event
		NULL,				// initial state is signaled
		NULL				// object name
	);
	if (EventoTimer == NULL)
	{
		std::cout << "Error CreateEvent EventoTimer" << std::endl;;
		return FALSE;
	}

	timeBeginPeriod(1); //Set Windows Timer Resolution to 1ms

	readFile(comPort, &baudrate, &ts, &mode, &rpm);
	///////////////////////////////////////////////////////
	// Create the timer queue.
	TimerQueue = CreateTimerQueue();
	if (NULL == TimerQueue)
	{
		std::cout << "CreateTimerQueue failed: " << GetLastError() << std::endl;
		return FALSE;
	}

	// Set a timer to call the timer routine in TO miliseconds.
	if (!CreateTimerQueueTimer(&Timer, TimerQueue,
		(WAITORTIMERCALLBACK)TimerRoutine, NULL, 0, ts, WT_EXECUTEDEFAULT))
	{
		std::cout << "CreateTimerQueueTimer failed: " << GetLastError() << std::endl;
		return FALSE;
	}

	signal(SIGINT, ctrlc);
	signal(SIGBREAK, window_cross);
	std::cout << "Inicio de captura del láser - Pulse Ctrl+C para finalizar programa\n\n" << std::endl;
	std::thread HiloLaser(laser_func);
	while ((ctrl_c_pressed == 0)&&(window_cross_pressed == 0))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	// Stop the threads
	stop_signal = true;
	// main thread waits for the other thread to finish
	HiloLaser.join();
	std::cout << "Finalizando laserRPLIDAR...\n" << std::endl;
	CloseHandle(EventoTimer);
	// Delete all timers in the timer queue.
	if (!DeleteTimerQueue(TimerQueue))
		std::cout << "DeleteTimerQueue failed: " << GetLastError() << std::endl;

	timeEndPeriod(1); //Unset Windows Timer Resolution to 1ms (back to normal -> 15.6ms)
	std::cout << "laserRPLIDAR Finalizado OK" << std::endl;
	Sleep(500);
	return 0;
}

void CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	SetEvent(EventoTimer);
}

void laser_func()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	u_result     op_result;
	CSharedStruct<LaserData>m_SharedData_RPLIDAR(M_COMPARTIDA_RPLIDAR);
	m_SharedData_RPLIDAR.Acquire();
	rplidar_response_measurement_node_hq_t nodes[MAXREADINGS];
	std::vector<RplidarScanMode> scanModes;
	size_t   count;

	// create the driver instance
	RPlidarDriver * drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
	if (!drv) {
		fprintf(stderr, "insufficent memory, exit\n");
		exit(-2);
	}

	rplidar_response_device_info_t devinfo;
	bool connectSuccess = false;
	// make connection...
	if (!drv)
		drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
	if (IS_OK(drv->connect(comPort, baudrate)))
	{
		op_result = drv->getDeviceInfo(devinfo);

		if (IS_OK(op_result))
		{
			connectSuccess = true;
		}
		else
		{
			delete drv;
			drv = NULL;
		}
	}
	if (!connectSuccess)
	{
		fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
			, comPort);
		goto on_finished;
	}
	// print out the device serial number, firmware and hardware version number..
	printf("RPLIDAR S/N: ");
	for (int pos = 0; pos < 16; ++pos) {
		printf("%02X", devinfo.serialnum[pos]);
	}

	printf("\n"
		"Firmware Ver: %d.%02d\n"
		"Hardware Rev: %d\n"
		, devinfo.firmware_version >> 8
		, devinfo.firmware_version & 0xFF
		, (int)devinfo.hardware_version);

	// check health...
	if (!checkRPLIDARHealth(drv)) {
		goto on_finished;
	}

	drv->getAllSupportedScanModes(scanModes);

	for (int i = 0; i < scanModes.size(); i++)
	{
		std::cout << "scanModes[" << i <<"].id = " << scanModes[i].id << std::endl;
		std::cout << "scanModes[" << i <<"].max_distance = " << scanModes[i].max_distance << std::endl;
		std::cout << "scanModes[" << i <<"].scan_mode = " << scanModes[i].scan_mode << std::endl;
		std::cout << "scanModes[" << i <<"].us_per_sample = " << scanModes[i].us_per_sample << std::endl << std::endl;
	}

	drv->startMotor();
	drv->startScanExpress(false, scanModes[mode].id); //DenseBoost = 1
	op_result = drv->setLidarSpinSpeed(rpm);
	if (IS_OK(op_result))
		std::cout << "drv->setLidarSpinSpeed OK" << std::endl << std::endl;
	else
		std::cout << "drv->setLidarSpinSpeed ERROR --- op_result = " << op_result << std::endl << std::endl;

	// fetech result and copy to shared memory...
	while (!stop_signal) 
	{
		WaitForSingleObject(EventoTimer, INFINITE);
		count = _countof(nodes);
		op_result = drv->grabScanDataHq(nodes, count, 0);
		if (IS_OK(op_result)) {
			//count = _countof(nodes);
			drv->ascendScanData(nodes, count);
			/*m_SharedData_RPLIDAR->angle_deg[(int)count] = (float)(nodes[0].angle_z_q14 * 90.f / (1 << 14));//angle_in_degrees
			m_SharedData_RPLIDAR->distance_m[(int)count] = (float)(nodes[0].dist_mm_q2 / 1000.f / (1 << 2));//distance_in_meters
			m_SharedData_RPLIDAR->quality[(int)count] = nodes[0].quality;
			for (int pos = 1; pos < (int)count; ++pos) {
				m_SharedData_RPLIDAR->angle_deg[pos-1] = (float)(nodes[pos].angle_z_q14 * 90.f / (1 << 14));//angle_in_degrees
				m_SharedData_RPLIDAR->distance_m[pos-1] = (float)(nodes[pos].dist_mm_q2 / 1000.f / (1 << 2));//distance_in_meters
				m_SharedData_RPLIDAR->quality[pos-1] = nodes[pos].quality;
			}*/
			for (int pos = 0; pos < (int)count; ++pos) {
				m_SharedData_RPLIDAR->angle_deg[pos] = (float)(nodes[pos].angle_z_q14 * 90.f / (1 << 14));//angle_in_degrees
				m_SharedData_RPLIDAR->distance_m[pos] = (float)(nodes[pos].dist_mm_q2 / 1000.f / (1 << 2));//distance_in_meters
				m_SharedData_RPLIDAR->quality[pos] = nodes[pos].quality;
			}
			m_SharedData_RPLIDAR->count = (uint16_t)count;
			std::cout << "count = " << (uint16_t)count << std::endl;
		}
		ResetEvent(EventoTimer);
	}
	drv->stop();
	drv->stopMotor();
	// done!
on_finished:
	RPlidarDriver::DisposeDriver(drv);
	drv = NULL;
	m_SharedData_RPLIDAR.Release();
}
