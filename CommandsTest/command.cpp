#include "command.h"
#include <FEHUtility.h>

FEHLCD* Command::lcd;
Script<Command>* Command::script;
Drive* Command::drive;
IO* Command::io;
Arm* Command::arm;

Command::Command()
{
    timer = new Timer();
    name = "[Command name]";
    has_been_initialized = false;
}

void Command::Init(FEHLCD *lcd_in, Drive *drive_in, IO *io_in, Arm *arm_in)
{
    lcd = lcd_in;
    drive = drive_in;
    io = io_in;
    arm = arm_in;
}

void Command::SetScript(Script<Command> *script_in)
{
    script = script_in;
}
