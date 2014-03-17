#include "setboxcommand.h"

void SetBoxCommand::Init()
{
    box->SetDegree(degree);
    timer->SetTimeout(timeout);
    timer->Reset();
}

void SetBoxCommand::Run()
{

}

bool SetBoxCommand::EndCondition()
{
    return timer->IsTimeout();
}

void SetBoxCommand::Finish()
{

}

bool SetBoxCommand::FailedCondition()
{
    return false;
}

void SetBoxCommand::Failure()
{

}

void SetBoxCommand::PrintStatus()
{
    lcd->Write("Degree: ");
    lcd->WriteLine(degree);
    lcd->Write("Time: ");
    lcd->WriteLine(timer->GetTimeLeft());
}


