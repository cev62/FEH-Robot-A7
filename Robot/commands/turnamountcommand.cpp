#include "turnamountcommand.h"

void TurnAmountCommand::Init()
{
    timer->SetTimeout(4.0);
    timer->Reset();

    // For storing the current heading
    curr_heading = io->rps->Heading();
    // For storing te initial heading
    initial_heading = curr_heading;
    // Stores the target heading NOT on the [0-179] scale. This is literally just adding the degrees to the initial heading
    raw_target_heading = initial_heading + degrees;
    // Sotres the target heading converted to the [0-179] scale that the RPS uses
    target_heading = raw_target_heading;

    // Flags for telling if the robot is going to cross from 0 to 179 or vice versa.
    crossing_0_right = false;
    crossing_0_left = false;
    moving_left = degrees > 0;
    moving_right = degrees < 0;
    if (raw_target_heading > 179) {target_heading -= 180; crossing_0_left = true;}
    if (raw_target_heading < 0) {target_heading += 180; crossing_0_right = true;}
}

void TurnAmountCommand::Run()
{
    curr_heading = io->rps->Heading();

    // Get the current heading NOT on the [0,179] scale (ie. heading can be [-360,360])
    raw_curr_heading = curr_heading;
    if(crossing_0_left && curr_heading < 90)
    {
        raw_curr_heading += 180;
    }
    if(crossing_0_right && curr_heading > 90)
    {
        raw_curr_heading -= 180;
    }

    // GEt the error. The
    error = raw_curr_heading - raw_target_heading;

    if(moving_left)
    {
        if(pivot == Drive::LEFT){ drive->SetDriveLR(0, drive->TURN_MIN_POWER - drive->P_TURN * error); }
        if(pivot == Drive::RIGHT){ drive->SetDriveLR(-drive->TURN_MIN_POWER + drive->P_TURN * error, 0); }
    }
    if(moving_right)
    {
        if(pivot == Drive::LEFT){ drive->SetDriveLR(0, -drive->TURN_MIN_POWER - drive->P_TURN * error); }
        if(pivot == Drive::RIGHT){ drive->SetDriveLR(drive->TURN_MIN_POWER + drive->P_TURN * error, 0); }
    }
}

bool TurnAmountCommand::EndCondition()
{
    if(error > 5) // Need to go more to the right
    {
        if (moving_right) { return false; } // Already going right. Continue
        if (moving_left) // Moving to the left. Need to stop and change directions to correct for overshoot
        {
            return true;
        }
    }
    else if(error < -5) // Need to go more to the left
    {
        if (moving_left) { return false; } // Already going left. Continue
        if (moving_right) // Moving to the right. Need to stop and change directions to correct for overshoot
        {
            return true;
        }
    }
    else
    {
        return true; //Success: error is close to 0
    }
}

void TurnAmountCommand::Finish()
{
    drive->SetDrive(0, 0);
}

bool TurnAmountCommand::FailedCondition()
{
    return timer->IsTimeout(); // TODO: add failure mode for when the robot overshoots, and implement it in failure
}

void TurnAmountCommand::Failure()
{
    drive->SetDrive(0, 0);

    // Testing adding failure commands to the script
    script->AddSequential(new PrintCommand("FIX 111"));
    script->AddSequential(new PrintCommand("FIX 222"));
    script->MergeQueue();
}

void TurnAmountCommand::PrintStatus()
{

}
