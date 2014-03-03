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
    float dist;
    DriveDistCommand(int forward_in, float dist_in) : Command()
    {
        name = "DriveDistCommand";
        forward = forward_in;
        dist = dist_in;
    }
};

#endif // DRIVEDISTCOMMAND_H
