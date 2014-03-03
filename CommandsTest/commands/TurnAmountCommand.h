#ifndef TURNAMOUNTCOMMAND_H
#define TURNAMOUNTCOMMAND_H

#include "../command.h"
#include "../drive.h"
#include "../printcommand.h"

// Turns the robot a certain number of degrees, pivoting about the back left wheel. Currently a maximum of a 90 degree turn

class TurnAmountCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    int degrees;
    Drive::Side pivot;

    // degrees < 0 means right turn, degrees > 0 means left turn
    TurnAmountCommand(int degrees_in, Drive::Side pivot_in) : Command()
    {
        name = "TurnAmountCommand";
        degrees = degrees_in;
        pivot = pivot_in;
    }

    int curr_heading, raw_curr_heading, initial_heading, target_heading, raw_target_heading, error;
    bool crossing_0_left, crossing_0_right, moving_left, moving_right;
};

#endif // TURNAMOUNTCOMMAND_H
