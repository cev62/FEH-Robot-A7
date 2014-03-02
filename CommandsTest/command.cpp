#include "command.h"
#include <FEHUtility.h>

FEHLCD* Command::lcd;

void Command::Init(FEHLCD *lcd_in)
{
    lcd = lcd_in;
}

/*void Command::Init()
{
    lcd->WriteLine("Default Command");
    Sleep(1.0);
}

void Command::Run()
{

}

bool Command::EndCondition()
{
    return true;
}

void Command::Finish()
{

}

bool Command::FailedCondition()
{
    return false;
}

void Command::Failure()
{

}
*/
