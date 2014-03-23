#include "drivedistcommand.h"

void DriveDistCommand::Init()
{
    drive->SetDrive(forward, 0);
    io->left_encoder->ResetCounts();
    io->right_encoder->ResetCounts();
    timer->Reset();
    timer->SetTimeout(dist / (2.75 * 3.14) / ((float)forward / 100.0) * 2.5 ); // dist / dist-per-rev * rev-per-sec (~1 in this case) / forward_power * 1.5 (factor of safety)
}

void DriveDistCommand::Run()
{
    drive->SetDrive(STRAIGHT_MIN_POWER + P_STRAIGHT * error, 0);
}

bool DriveDistCommand::EndCondition()
{
    error = dist - io->GetDistTravelled();
    return error < 0;
}

void DriveDistCommand::Finish()
{
    drive->SetDrive(0, 0);
    PrintStatus();
    Sleep(10.0);
}

bool DriveDistCommand::FailedCondition()
{
    return timer->IsTimeout();
}

void DriveDistCommand::Failure()
{
    drive->SetDrive(0, 0);
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
    lcd->Write("Dist Travelled: ");
    lcd->WriteLine(io->GetDistTravelled());
    lcd->Write("TargetCounts: ");
    lcd->WriteLine(io->COUNTS_PER_INCH * dist);
    lcd->Write("Timeout: ");
    lcd->WriteLine(timer->GetTimeLeft());
}

