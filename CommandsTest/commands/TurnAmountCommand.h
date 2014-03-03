#ifndef TURNAMOUNTCOMMAND_H
#define TURNAMOUNTCOMMAND_H

#include "../command.h"
#include "../drive.h"

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
    TurnAmountCommand(int degrees_in, Drive::Side pivot_in) : Command()
    {
        name = "TurnAmountCommand";
        degrees = degrees_in;
        pivot = pivot_in;
    }
};

#endif // TURNAMOUNTCOMMAND_H
