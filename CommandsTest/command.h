#ifndef COMMAND_H
#define COMMAND_H

#include <util/script.h>
#include <FEHLCD.h>
#include <FEHUtility.h>

class Command
{
public:
    Command() {}
    virtual void Init() = 0;
    virtual void Run() = 0;
    virtual bool EndCondition() = 0;
    virtual bool FailedCondition() = 0;
    virtual void Finish() = 0;
    virtual void Failure() = 0;
    virtual void PrintStatus() = 0;

    //static Drive *drive;
    static FEHLCD *lcd;
    static Script *script;
    static void Init(FEHLCD *lcd_in);
    static void SetScript(Script *script_in);
private:
};

#endif // COMMAND_H
