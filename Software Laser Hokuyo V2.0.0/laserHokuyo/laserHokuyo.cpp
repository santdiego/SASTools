#include "laserHokuyo.h"

using namespace qrk;

void readFile(char *port, USHORT *ts) {
	TCHAR nameFile[50];
	FILE *file;
	errno_t err;
	_stprintf_s(nameFile, sizeof(nameFile), _T("Config.txt"));
	err = _tfopen_s(&file, nameFile, _T("r"));
	if (err == 0)
	{
		fscanf_s(file, "%s\n%hd\n", port, 6, ts);
		fclose(file);
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

	readFile(comPort, &ts);
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

	std::thread HiloLaser(laser_func);

	CSharedStruct<LaserData>m_SharedData_Hokuyo(M_COMPARTIDA_HOKUYO);
	m_SharedData_Hokuyo.Acquire();
	std::cout << "Inicio de captura del láser - Pulse tecla F10 para finalizar programa" << std::endl;;
	while (!GetAsyncKeyState(VK_F10))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "Finalizando laserHokuyo..." << std::endl;;
	m_SharedData_Hokuyo.Release();
	// Stop the threads
	stop_signal = true;
	// main thread waits for the other thread to finish
	HiloLaser.join();
	CloseHandle(EventoTimer);

	// Delete all timers in the timer queue.
	if (!DeleteTimerQueue(TimerQueue))
		std::cout << "DeleteTimerQueue failed: " << GetLastError() << std::endl;

	timeEndPeriod(1); //Unset Windows Timer Resolution to 1ms (back to normal -> 15.6ms)

	return 0;
}

void CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	SetEvent(EventoTimer);
}


void laser_func()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	CSharedStruct<LaserData>m_SharedData_Hokuyo(M_COMPARTIDA_HOKUYO);
	m_SharedData_Hokuyo.Acquire();

	// Connects to the sensor
	Urg_driver urg;
	if (!urg.open(comPort, 115200, Urg_driver::Serial))
	{
		std::cout << "Urg_driver::open(): "
			<< comPort << ": " << urg.what() << std::endl;
		return;
	}
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-120), urg.deg2step(+120), 0);
	//std::cout << "Urg_driver::max_data_size(): " << urg.max_data_size() << std::endl;
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	std::vector<long> data;
	long time_stamp = 0;

	for (int i = 0; i < NUMREADINGS; ++i) {
		m_SharedData_Hokuyo->angle_rad[i] = (float)urg.index2rad(i);
		m_SharedData_Hokuyo->angle_deg[i] = (float)urg.index2deg(i);
	}

	while (!stop_signal)
	{
		WaitForSingleObject(EventoTimer, INFINITE);

		if (!urg.get_distance(data, &time_stamp)) {
			std::cout << "Urg_driver::get_distance(): " << urg.what() << std::endl;
			break;
		}
		std::copy(data.begin()+1, data.end(), m_SharedData_Hokuyo->distance_mm);
		ResetEvent(EventoTimer);
	}
	urg.stop_measurement();
	urg.close();
	m_SharedData_Hokuyo.Release();
	return;
}