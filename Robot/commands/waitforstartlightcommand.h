#ifndef WAITFORSTARTLIGHT_H
#define WAITFORSTARTLIGHT_H

#include "../command.h"

class WaitForStartLightCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    float curr_value, initial_value;

    WaitForStartLightCommand() : Command()
    {
        name = "WaitForStartLightCommand";
    }
};

#endif // WAITFORSTARTLIGHT_H
