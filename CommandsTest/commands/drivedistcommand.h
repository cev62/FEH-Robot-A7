#ifndef DRIVEDISTCOMMAND_H
#define DRIVEDISTCOMMAND_H

#include "../command.h"
#include "../printcommand.h"

class DriveDistCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    int forward;
    float dist, error, STRAIGHT_MIN_POWER, P_STRAIGHT;
    DriveDistCommand(int forward_in, float dist_in) : Command()
    {
        name = "DriveDistCommand";
        forward = forward_in;
        dist = dist_in;
        STRAIGHT_MIN_POWER = 40;
        P_STRAIGHT = (forward - STRAIGHT_MIN_POWER) / 6.0;
        error = dist;
    }
};

#endif // DRIVEDISTCOMMAND_H
