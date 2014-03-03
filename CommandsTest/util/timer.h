#ifndef TIMER_H
#define TIMER_H

#include <FEHUtility.h>

class Timer
{
public:
    Timer(float timeout_in = 0.0);
    float GetTime();
    bool IsTimeout();
    void SetTimeout(float timeout_in);
    void Reset();
private:
    float start_time, timeout;
};

#endif // TIMER_H
