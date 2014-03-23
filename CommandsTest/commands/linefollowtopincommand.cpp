#include "linefollowtopincommand.h"

void LineFollowToPinCommand::Init()
{
    drive->SetDrive(100, 0);
    timer->SetTimeout(6.0);
    timer->Reset();

    gray_value = io->optosensor->Value();
}

void LineFollowToPinCommand::Run()
{
    value = io->optosensor->Value();
    bool is_on_line = value < gray_value - DELTA_THRESH;
    if(is_on_line)
    {
        // Need to go right
        drive->SetDriveLR(65, 30);
    }
    else
    {
        // Need to go left
        drive->SetDriveLR(30, 55);
    }
}

bool LineFollowToPinCommand::EndCondition()
{
    return !io->arm_switch->Value();
}

void LineFollowToPinCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool LineFollowToPinCommand::FailedCondition()
{
    return timer->IsTimeout();
}

void LineFollowToPinCommand::Failure()
{
    script->AddSequential(new SetArmCommand(-1, 1.0));
    script->AddSequential(new DriveCommand(-100, 0, 0.75));
    script->AddSequential(new DriveCommand(0, 100, 0.25));
    script->AddSequential(new SetArmCommand(120, 1.0));
    script->AddSequential(new LineFollowToPinCommand());
}

void LineFollowToPinCommand::PrintStatus()
{
    lcd->Write("Is on line: ");
    lcd->WriteLine(is_on_line);
    lcd->Write("Time: ");
    lcd->WriteLine(timer->GetTimeLeft());
}

