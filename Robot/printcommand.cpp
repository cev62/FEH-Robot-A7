#include "printcommand.h"

void PrintCommand::Init()
{
    timer->Reset();
    timer->SetTimeout(2.0);
}

void PrintCommand::Run()
{

}

bool PrintCommand::EndCondition()
{
    return timer->IsTimeout();
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
    lcd->WriteLine(message);
    lcd->Write("T: ");
    lcd->WriteLine(timer->GetTimeLeft());
}
