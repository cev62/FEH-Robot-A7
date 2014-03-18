#ifndef SETARMCOMMAND_H
#define SETARMCOMMAND_H

#include "../command.h"

class SetArmCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    int degree;
    float timeout;
    SetArmCommand(int degree_in, float timeout_in) : Command()
    {
        name = "SetArmCommand";
        degree = degree_in;
        timeout = timeout_in;
    }
};

#endif // SETARMCOMMAND_H
