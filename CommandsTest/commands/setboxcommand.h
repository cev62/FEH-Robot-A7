#ifndef SETBOXCOMMAND_H
#define SETBOXCOMMAND_H

#include "../command.h"

class SetBoxCommand : public Command
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
    SetBoxCommand(int degree_in, float timeout_in) : Command()
    {
        name = "SetBoxCommand";
        degree = degree_in;
        timeout = timeout_in;
    }
};

#endif // SETBoxCOMMAND_H
