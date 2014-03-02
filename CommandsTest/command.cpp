#include "command.h"
#include <FEHUtility.h>

FEHLCD* Command::lcd;
Script Command::script;

void Command::Init(FEHLCD *lcd_in)
{
    lcd = lcd_in;
}

void Command::SetScript(Scipt *script_in)
{
    script = script_in;
}
