#ifndef DRIVECOMMAND_H
#define DRIVECOMMAND_H

#include "../command.h"

class DriveCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    int forward, turn;
    float time;
    DriveCommand(int forward_in, int turn_in, float time_in) : Command()
    {
        forward = forward_in;
        turn = turn_in;
        time = time_in;
    }
};

#endif // DRIVECOMMAND_H
