#include "command.h"
#include <FEHUtility.h>

FEHLCD* Command::lcd;
Script<Command>* Command::script;
Drive* Command::drive;

Command::Command()
{
    timer = new Timer();
    name = "[Command name]";
}

void Command::Init(FEHLCD *lcd_in, Drive *drive_in)
{
    lcd = lcd_in;
    drive = drive_in;
}

void Command::SetScript(Script<Command> *script_in)
{
    script = script_in;
}
