#ifndef TIMER_H
#define TIMER_H

#include <FEHUtility.h>

class Timer
{
public:
    Timer();
    float GetTime();
    void Reset();
private:
    float start_time;
};

#endif // TIMER_H
