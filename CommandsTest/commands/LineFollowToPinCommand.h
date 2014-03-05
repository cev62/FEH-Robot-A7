#ifndef LINEFOLLOWTOPINCOMMAND_H
#define LINEFOLLOWTOPINCOMMAND_H

#include "../command.h"
#include "drivecommand.h"

class LineFollowToPinCommand : public Command
{
public:
    void Init();
    void Run();
    bool EndCondition();
    bool FailedCondition();
    void Finish();
    void Failure();
    void PrintStatus();

    float gray_value, value, DELTA_THRESH;
    bool is_on_line;

    LineFollowToPinCommand() : Command()
    {
        name = "LineFollowToPinCommand";
        DELTA_THRESH = 0.3;
        is_on_line = false;
    }
};

#endif // LINEFOLLOWTOPINCOMMAND_H
