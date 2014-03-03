#include "turnamountcommand.h"

void TurnAmountCommand::Init()
{
    timer->SetTimeout(5.0);
    timer->Reset();
}

void TurnAmountCommand::Run()
{

}

bool TurnAmountCommand::EndCondition()
{
    return timer->IsTimeout();
}

void TurnAmountCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool TurnAmountCommand::FailedCondition()
{
    return false;
}

void TurnAmountCommand::Failure()
{

}

void TurnAmountCommand::PrintStatus()
{

}
