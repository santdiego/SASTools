#include "itimer.h"
#include <iostream>
using namespace std;
ITimer::ITimer() :thr()
{
    _active=false;
    _singleShot=false;
    pf=nullptr;
    pff=nullptr;
    currentStartTime=steady_clock::time_point(steady_clock::now());
    nextStartTime=steady_clock::time_point(currentStartTime);

}
ITimer::~ITimer(){
    stop();
}

void	ITimer::onTimeout( std::function<void()> fcall ){
    pf=fcall;
}

void	ITimer::onTimeout(std::function<void(void*)> fcall,void* inarg){
    arg=inarg;
    pff=fcall;
}

void	ITimer::start(int msec){
    intervalPeriodMillis=std::chrono::milliseconds(msec);
    //recargar periodo
    start();
}

void	ITimer::start(){
    thr= std::thread(&ITimer::loop,this);
    thr.detach();
    _stop=false;
    _active=true;
}

void	ITimer::setInterval(int msec){
    intervalPeriodMillis=std::chrono::milliseconds(msec);
    pres=milli_Seconds;
}

void	ITimer::setIntervalus(int usec){
    intervalPeriodMicros=std::chrono::microseconds(usec);
    pres=micro_Seconds;
}

void	ITimer::setIntervalns(int nsec){
    intervalPeriodNanos=std::chrono::nanoseconds(nsec);
    pres=nano_Seconds;
}

void	ITimer::setSingleShot(bool singleShot){
    _singleShot=singleShot;
}

void	ITimer::stop(){
    _stop=true;
    while(_active==true){
        std::this_thread::sleep_until(system_clock::now()+std::chrono::milliseconds(10));
    }
}

bool	ITimer::isActive() const{
    return _active;
}

bool	ITimer::isSingleShot() const{
    return _singleShot;
}

int     ITimer::remainingTime() const{

    return (duration_cast<microseconds>(nextStartTime-steady_clock::now()).count())/microsPerClkTic;

}

void ITimer::loop(){
    while(!_stop){
        _active=true;
        switch (pres) {
        case micro_Seconds:
            nextStartTime = steady_clock::now() + intervalPeriodMicros;
            break;
        case nano_Seconds:
            nextStartTime = steady_clock::now() + intervalPeriodNanos;
            break;
        case milli_Seconds:
            nextStartTime = steady_clock::now() + intervalPeriodMillis;
            break;
        default:
            nextStartTime = steady_clock::now() + intervalPeriodMillis;
            break;
        }

        std::this_thread::sleep_until(nextStartTime);

        if (pf)
            (pf)();//call user function
        else if(pff)
            (pff)(arg);
        if(_singleShot)
            break;
    }
    _active=false;
}
