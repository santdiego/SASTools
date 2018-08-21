#ifndef ITIMER_H
#define ITIMER_H
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono;

const double microsPerClkTic{
    1.0E6 * system_clock::period::num / system_clock::period::den };

enum timerPrecision{milli_Seconds,micro_Seconds,nano_Seconds};

class ITimer
{
public:
    ITimer();
    ~ITimer();
    //int	interval() const;
    //std::chrono::milliseconds	intervalAsDuration() const;
    bool	isActive() const;
    bool	isSingleShot() const;
    int     remainingTime() const;
    //std::chrono::milliseconds	remainingTimeAsDuration() const;
    void	setInterval(int msec);
    void	setIntervalns(int nsec);
    void	setIntervalus(int usec);
    void	setSingleShot(bool singleShot);
    //void	setTimerType(Qt::TimerType atype);
    //void	start(std::chrono::milliseconds msec);
    //int     timerId() const;
    //SLOTS
    void	start(int msec);
    void	start();
    void	stop();
    //SIGNALS
    //void    timeout();
    void	onTimeout(std::function<void()> fcall);
    void	onTimeout(std::function<void(void*)> fcall, void *inarg);
private:
     milliseconds intervalPeriodMillis;
     microseconds intervalPeriodMicros;
     nanoseconds intervalPeriodNanos;
     steady_clock::time_point currentStartTime;
     steady_clock::time_point nextStartTime;
     void loop();
     timerPrecision pres;
     bool _stop;
     bool _active;
     bool _singleShot;
     std::thread thr;
     std::function<void()> pf;
     std::function<void( void*)> pff;
     void *arg;
};

#endif // ITIMER_H
