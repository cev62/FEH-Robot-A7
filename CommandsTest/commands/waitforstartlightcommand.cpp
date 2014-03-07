#include "waitforstartlightcommand.h"

void WaitForStartLightCommand::Init()
{
    curr_value = io->cds_cell->Value();
    initial_value = curr_value;
}

void WaitForStartLightCommand::Run()
{
    curr_value = io->cds_cell->Value();
}

bool WaitForStartLightCommand::EndCondition()
{
    return curr_value - initial_value < -0.375;
}

void WaitForStartLightCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool WaitForStartLightCommand::FailedCondition()
{
    return io->ButtonBoardGetButton(IO::RIGHT);
}

void WaitForStartLightCommand::Failure()
{

}

void WaitForStartLightCommand::PrintStatus()
{
    lcd->Write("CdS cell value: ");
    lcd->WriteLine(curr_value);
    lcd->Write("Threshhold: ");
    lcd->WriteLine(initial_value - 0.375);
}

