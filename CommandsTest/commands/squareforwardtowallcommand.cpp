#include "squareforwardtowallcommand.h"

void SquareForwardToWallCommand::Init()
{
    drive->SetDrive(0, 0);
    timer->SetTimeout(time);
    timer->Reset();
}

void SquareForwardToWallCommand::Run()
{
    drive->SetDriveLR(io->left_switch->Value() ? 100 : 0, io->right_switch->Value() ? 100 : 0);
}

bool SquareForwardToWallCommand::EndCondition()
{
    return !io->left_switch->Value() && !io->right_switch->Value();
}

void SquareForwardToWallCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool SquareForwardToWallCommand::FailedCondition()
{
    return timer->IsTimeout();
}

void SquareForwardToWallCommand::Failure()
{
    drive->SetDrive(0, 0);
    script->AddSequential(new PrintCommand("Squaring to wall FAILED"));
    script->MergeQueue();
}

void SquareForwardToWallCommand::PrintStatus()
{
    lcd->Write("Left switch pressed: ");
    lcd->WriteLine(!io->left_switch->Value());
    lcd->Write("Right switch pressed: ");
    lcd->WriteLine(!io->right_switch->Value());
    lcd->Write("Time: ");
    lcd->WriteLine(timer->GetTimeLeft());
}

