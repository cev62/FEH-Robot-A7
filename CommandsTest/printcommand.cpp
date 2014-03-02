#include <printcommand.h>

void PrintCommand::Init()
{
    lcd->WriteLine(message);
    Sleep(1.0);
}

void PrintCommand::Run()
{

}

bool PrintCommand::EndCondition()
{
    return true;
}

void PrintCommand::Finish()
{

}

bool PrintCommand::FailedCondition()
{
    return false;
}

void PrintCommand::Failure()
{

}

void PrintCommand::PrintStatus()
{

}
