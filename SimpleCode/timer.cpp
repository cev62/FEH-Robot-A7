#include "timer.h"

Timer::Timer(float timeout_in)
{
    start_time = TimeNow();
    timeout = timeout_in;
}

float Timer::GetTime()
{
    return TimeNow() - start_time;
}


float Timer::GetTimeLeft()
{
    return timeout - GetTime();
}

void Timer::Reset()
{
    start_time = TimeNow();
}

void Timer::SetTimeout(float timeout_in)
{
    timeout = timeout_in;
}

bool Timer::IsTimeout()
{
    return GetTime() > timeout;
}

