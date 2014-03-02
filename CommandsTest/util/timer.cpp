#include <timer.h>

Timer::Timer()
{
    start_time = TimeNow();
}

float Timer::GetTime()
{
    return TimeNow() - start_time;
}

void Timer::Reset()
{
    start_time = TimeNow();
}
