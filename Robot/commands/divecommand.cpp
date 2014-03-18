#include "drivecommand.h"

void DriveCommand::Init()
{
    drive->SetDrive(forward, turn);
    timer->SetTimeout(time);
    timer->Reset();
}

void DriveCommand::Run()
{

}

bool DriveCommand::EndCondition()
{
    return timer->IsTimeout();
}

void DriveCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool DriveCommand::FailedCondition()
{
    return false;
}

void DriveCommand::Failure()
{

}

void DriveCommand::PrintStatus()
{
    lcd->Write("Forward: ");
    lcd->WriteLine(forward);
    lcd->Write("Turn: ");
    lcd->WriteLine(turn);
    lcd->Write("Time: ");
    lcd->WriteLine(timer->GetTimeLeft());
}
