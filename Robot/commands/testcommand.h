#ifndef TEST_H
#define TEST_H

#include "../command.h"

class TestCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    TestCommand() : Command()
    {
        name = "TestCommand";
    }
};

#endif // TEST_H
