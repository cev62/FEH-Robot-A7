#ifndef PRINTCOMMAND_H
#define PRINTCOMMAND_H

#include <command.h>
#include <FEHUtility.h>

class PrintCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();
    char *message;
    PrintCommand(char *message_in) : Command()
    {
        message = message_in;
    }

};

#endif // PRINTCOMMAND_H
