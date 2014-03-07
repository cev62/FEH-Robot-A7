#ifndef SQUAREFORWARDTOWALLCOMMAND_H
#define SQUAREFORWARDTOWALLCOMMAND_H

#include "../command.h"
#include "../printcommand.h"

class SquareForwardToWallCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    float time;
    SquareForwardToWallCommand(float time_in) : Command()
    {
        name = "SuareForwardToWallCommand";
        time = time_in;
    }
};

#endif // SQUAREFORWARDTOWALLCOMMAND_H
