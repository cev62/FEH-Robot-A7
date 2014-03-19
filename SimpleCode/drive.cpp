#include "drive.h"

Drive::Drive(FEHMotor *left_in, FEHMotor *right_in, IO *io_in)
{
    left = left_in;
    right = right_in;
    io = io_in;
    TURN_MIN_POWER = Drive::TURN_MIN_POWER_FACTORY;
}

void Drive::SetDrive(int forward, int turn)
{
    int left_percent = forward + turn;
    int right_percent = forward - turn;

    if(left_percent > 100){ left_percent = 100; }
    if(left_percent < -100){ left_percent = -100; }
    if(right_percent > 100){ right_percent = 100; }
    if(right_percent < -100){ right_percent = -100; }

    left->SetPercent(-left_percent);
    right->SetPercent(right_percent);
}


void Drive::SetDriveLR(int left_percent, int right_percent)
{
    left->SetPercent(-left_percent);
    right->SetPercent(right_percent);
}

void Drive::SetDriveTime(int forward, int turn, float time)
{
    SetDrive(forward, turn);
    Sleep(time);
    SetDrive(0, 0);
}

void Drive::PushButton()
{
    SetDrive(50, 0);
    while(io->fr_switch->Value() == 1)
    {
        Sleep(IO::LOOP_TIMEOUT);
    }
    SetDrive(0, 0);
    Sleep(0.3);
    SetDriveTime(-100, 0, 0.15);
}

// Turns the robot a certain number of degrees, pivoting about the back left wheel. Currently a maximum of a 90 degree turn
// TODO: add failure timeout
void Drive::TurnAmount(int degrees, Drive::Side pivot) // degrees < 0 means right turn, degrees > 0 means left turn
{
    // For storing the current heading
    int curr_heading = io->rps->Heading();
    // For storing te initial heading
    int initial_heading = curr_heading;
    // Stores the target heading NOT on the [0-179] scale. This is literally just adding the degrees to the initial heading
    int raw_target_heading = initial_heading + degrees;
    // Sotres the target heading converted to the [0-179] scale that the RPS uses
    int target_heading = raw_target_heading;

    // Flags for telling if the robot is going to cross from 0 to 179 or vice versa.
    bool crossing_0_right = false, crossing_0_left = false, moving_left = degrees > 0, moving_right = degrees < 0;
    if (raw_target_heading > 179) {target_heading -= 180; crossing_0_left = true;}
    if (raw_target_heading < 0) {target_heading += 180; crossing_0_right = true;}

    while (true)
    {
        curr_heading = io->rps->Heading();

        // Get the current heading NOT on the [0,179] scale (ie. heading can be [-360,360])
        int raw_curr_heading = curr_heading;
        if(crossing_0_left && curr_heading < 90)
        {
            raw_curr_heading += 180;
        }
        if(crossing_0_right && curr_heading > 90)
        {
            raw_curr_heading -= 180;
        }

        // GEt the error. The
        int error = raw_curr_heading - raw_target_heading;

        if(moving_left)
        {
            if(pivot == Drive::LEFT){ SetDriveLR(0, TURN_MIN_POWER - Drive::P_TURN * error); }
            if(pivot == Drive::RIGHT){ SetDriveLR( (-1) * TURN_MIN_POWER + Drive::P_TURN * error, 0); }
        }
        if(moving_right)
        {
            if(pivot == Drive::LEFT){ SetDriveLR(0, (-1) * TURN_MIN_POWER - Drive::P_TURN * error); }
            if(pivot == Drive::RIGHT){ SetDriveLR(TURN_MIN_POWER + Drive::P_TURN * error, 0); }
        }

        if(error > 5) // Need to go more to the right
        {
            if (moving_right) {} // Already going right. Continue
            if (moving_left) // Moving to the left. Need to stop and change directions to correct for overshoot
            {
                SetDriveLR(0, 0);
                break;
            }
        }
        else if(error < -5) // Need to go more to the left
        {
            if (moving_left) {} // Already going left. Continue
            if (moving_right) // Moving to the right. Need to stop and change directions to correct for overshoot
            {
                SetDriveLR(0, 0);
                break;
            }
        }
        else
        {
            SetDriveLR(0, 0);
            break; //Success
        }

        Sleep(IO::LOOP_TIMEOUT);
    }

}

void Drive::TurnAngle(int degrees, Drive::Side direction, Drive::Side pivot) // degrees < 0 means right turn, degrees > 0 means left turn
{
    int curr_heading = io->rps->Heading();
    int amount_to_turn = 0;
    if(direction == LEFT)
    {
        if(degrees > curr_heading)
        {
            //TurnAmount(degrees - curr_heading, pivot);
            amount_to_turn = degrees - curr_heading;
        }
        else if(degrees < curr_heading)
        {
            //TurnAmount(degrees + 180 - curr_heading, pivot);
            amount_to_turn = degrees + 180 - curr_heading;
        }
    }
    if(direction == RIGHT)
    {
        if(degrees > curr_heading)
        {
            //TurnAmount(degrees - 180 - curr_heading, pivot);
            amount_to_turn = degrees - 180 - curr_heading;
        }
        else if(degrees < curr_heading)
        {
            //TurnAmount(degrees - curr_heading, pivot);
            amount_to_turn = degrees - curr_heading;
        }
    }

    // If the amount to turn is less than 90 degrees, TurnAmount can handle it, so just call the function
    if(amount_to_turn <= 90 && amount_to_turn >= -90)
    {
        TurnAmount(amount_to_turn, pivot);
    }
    // If the amount to turn is more than 90 degrees, TurnAmount can NOT handle it, so split the turn into two turns back to back
    else
    {
        TurnAmount(amount_to_turn / 2, pivot);
        TurnAmount(amount_to_turn / 2, pivot);
    }
}

// TODO: add failure timeout
void Drive::SquareToWallForward()
{
    while(true)
    {
        SetDriveLR(io->fl_switch->Value() ? 100 : 0, io->fr_switch->Value() ? 100 : 0);
        Sleep(IO::LOOP_TIMEOUT);
        if(!io->fl_switch->Value() && !io->fr_switch->Value())
        {
            break;
        }
    }
    SetDrive(0, 0);
}

// TODO: add failure timeout
void Drive::SquareToWallBackward()
{
    while(true)
    {
        SetDriveLR(io->bl_switch->Value() ? -100 : 0, io->br_switch->Value() ? -100 : 0);
        if(!io->bl_switch->Value() && !io->br_switch->Value())
        {
            break;
        }
        if(io->print_timer->IsTimeout())
        {
            io->lcd->Clear();
            io->lcd->Write("LEFT: ");
            io->lcd->Write(!io->bl_switch->Value());
            io->lcd->Write("RIGHT: ");
            io->lcd->Write(!io->br_switch->Value());
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
    SetDrive(0, 0);
}

// TODO: add timer
void Drive::DriveDist(int forward, float dist)
{
    int forward_sign = 1; // Deals with negative values of forward that break the p-controller
    if(forward < 0)
    {
        forward_sign = -1;
        forward = -forward;
    }
    float P_FORWARD = (forward - Drive::STRAIGHT_MIN_POWER) / Drive::RAMP_DOWN_DIST;
    SetDrive(forward, 0);
    io->left_encoder->ResetCounts();
    io->right_encoder->ResetCounts();
    float error = dist;

    while(true)
    {
        error = dist - io->GetDistTravelled();
        SetDrive(forward_sign * (STRAIGHT_MIN_POWER + P_FORWARD * error), 0);
        if(error < 0)
        {
            break;
        }
    }
    SetDrive(0, 0);
}

void Drive::TurnLeft90()
{

    int numcounts = 77;
    float motorPower = 100;
    io->ResetEncoders();

    while(io->left_encoder->Counts() < numcounts)
    {
        motorPower = -100 + (io->left_encoder->Counts()/numcounts)*40;
        SetDrive(motorPower, 100);
    }

    SetDrive(0, 0);
}
