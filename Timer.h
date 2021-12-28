#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <SDL.h>

class Timer{
public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    Uint32 getTicks();
    bool isStarted();
    bool isPaused();

private:
    //Время, когда таймер был запущен
    Uint32 mStartTicks;
    //Время, когда таймер был остановлен
    Uint32 mPausedTicks;
    //Статус таймера
    bool mPaused;
    bool mStarted;
};

#endif // TIMER_H_INCLUDED
