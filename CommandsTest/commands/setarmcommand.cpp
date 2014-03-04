#include "setarmcommand.h"

void SetArmCommand::Init()
{
    if(degree == -1) // ARM_STOP from main.cpp
    {
        arm->Off();
    }
    else
    {
        arm->SetDegree(degree);
    }
    timer->SetTimeout(timeout);
    timer->Reset();
}

void SetArmCommand::Run()
{

}

bool SetArmCommand::EndCondition()
{
    return timer->IsTimeout();
}

void SetArmCommand::Finish()
{

}

bool SetArmCommand::FailedCondition()
{
    return false;
}

void SetArmCommand::Failure()
{

}

void SetArmCommand::PrintStatus()
{
    lcd->Write("Degree: ");
    lcd->WriteLine(degree);
    lcd->Write("Time: ");
    lcd->WriteLine(timer->GetTimeLeft());
}

