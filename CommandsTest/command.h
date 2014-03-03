#ifndef COMMAND_H
#define COMMAND_H

#include "util/script.h"
#include <FEHLCD.h>
#include <FEHUtility.h>
#include "util/timer.h"
#include "drive.h"
#include "io.h"
#include "arm.h"

class Command
{
public:
    Command();
    virtual void Init() = 0;
    virtual void Run() = 0;
    virtual bool EndCondition() = 0;
    virtual bool FailedCondition() = 0;
    virtual void Finish() = 0;
    virtual void Failure() = 0;
    virtual void PrintStatus() = 0;

    char* name;
    bool has_been_initialized;

    static FEHLCD *lcd;
    static Script<Command> *script;
    static Drive *drive;
    static IO *io;
    static Arm *arm;
    static void Init(FEHLCD *lcd_in, Drive *drive_in, IO *io_in, Arm *arm_in);
    static void SetScript(Script<Command> *script_in);

protected:
    Timer *timer;
};

#endif // COMMAND_H
