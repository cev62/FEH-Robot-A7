#include "drivedistcommand.h"

void DriveDistCommand::Init()
{
    drive->SetDrive(forward, 0);
    io->left_encoder->ResetCounts();
    io->right_encoder->ResetCounts();
    timer->Reset();
    timer->SetTimeout(dist / (2.75 * 3.14) * 1.5 ); // dist / dist-per-rev * rev-per-sec (~1 in this case) * 1.5 (factor of safety)
}

void DriveDistCommand::Run()
{

}

bool DriveDistCommand::EndCondition()
{
    return io->GetDistTravelled() > dist;
}

void DriveDistCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool DriveDistCommand::FailedCondition()
{
    return timer->IsTimeout();
}

void DriveDistCommand::Failure()
{
    script->AddSequential(new PrintCommand("Encoder driving timed out..."));
    script->MergeQueue();
}

void DriveDistCommand::PrintStatus()
{
    lcd->Write("Forward: ");
    lcd->WriteLine(forward);
    lcd->Write("Left side: ");
    lcd->WriteLine(io->left_encoder->Counts());
    lcd->Write("Right side: ");
    lcd->WriteLine(io->right_encoder->Counts());
    lcd->Write("Timeout: ");
    lcd->WriteLine(timer->GetTimeLeft());
}

