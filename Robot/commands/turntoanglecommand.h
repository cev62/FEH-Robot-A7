#ifndef TURNTOANGLECOMMAND_H
#define TURNTOANGLECOMMAND_H

#include "../command.h"
#include "../drive.h"
#include "../io.h"
#include "turnamountcommand.h"

class TurnToAngleCommand : public Command
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
    Drive::Side direction, pivot;
    TurnToAngleCommand(int degrees_in, Drive::Side direction_in, Drive::Side pivot_in) : Command()
    {
        name = "TurnToAngleCommand";
        degrees = degrees_in;
        direction = direction_in;
        pivot = pivot_in;
    }
};

#endif // TURNTOANGLECOMMAND_H
