#include "drivecommand.h"

void DriveCommand::Init()
{
    lcd->Write("HI");
    drive->SetDrive(forward, turn);
    //timer->SetTimeout(time);
    timer->Reset();
}

void DriveCommand::Run()
{

}

bool DriveCommand::EndCondition()
{
    return timer->GetTime() > time;
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
    lcd->Write("Drive F: ");
    lcd->Write(forward);
    lcd->Write(", T: ");
    lcd->Write(turn);
}
