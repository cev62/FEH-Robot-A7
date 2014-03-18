#include "turntoanglecommand.h"

void TurnToAngleCommand::Init()
{
    int curr_heading = io->rps->Heading();
    int amount_to_turn = 0;
    if(direction == Drive::LEFT)
    {
        if(degrees > curr_heading)
        {
            amount_to_turn = degrees - curr_heading;
        }
        else if(degrees < curr_heading)
        {
            amount_to_turn = degrees + 180 - curr_heading;
        }
    }
    if(direction == Drive::RIGHT)
    {
        if(degrees > curr_heading)
        {
            amount_to_turn = degrees - 180 - curr_heading;
        }
        else if(degrees < curr_heading)
        {
            amount_to_turn = degrees - curr_heading;
        }
    }

    // If the amount to turn is less than 90 degrees, TurnAmount can handle it, so just call the function
    if(amount_to_turn <= 90 && amount_to_turn >= -90)
    {
        script->AddSequential(new TurnAmountCommand(amount_to_turn, pivot));
    }
    // If the amount to turn is more than 90 degrees, TurnAmount can NOT handle it, so split the turn into two turns back to back
    else
    {
        script->AddSequential(new TurnAmountCommand(amount_to_turn / 2, pivot));
        script->AddSequential(new TurnAmountCommand(amount_to_turn / 2, pivot));
    }
    script->MergeQueue();
}

void TurnToAngleCommand::Run()
{

}

void TurnToAngleCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool TurnToAngleCommand::EndCondition()
{
    return true;
}

bool TurnToAngleCommand::FailedCondition()
{
    return false;
}

void TurnToAngleCommand::Failure()
{

}

void TurnToAngleCommand::PrintStatus()
{

}
