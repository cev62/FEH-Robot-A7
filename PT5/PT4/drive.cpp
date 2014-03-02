#include "drive.h"

Drive::Drive(FEHMotor *left_in, FEHMotor *right_in, DigitalInputPin *right_switch_in, DigitalInputPin *left_switch_in, FEHWONKA *rps_in)
{
    left = left_in;
    right = right_in;
    right_switch = right_switch_in;
    left_switch = left_switch_in;
    rps = rps_in;
    pTurn = 20.0 / 45.0;
    turn_min_power = 60;
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
    while(right_switch->Value() == 1)
    {
        Sleep(0.010);
    }
    SetDrive(0, 0);
    Sleep(0.3);
    SetDriveTime(-100, 0, 0.25);
}

// Turns the robot a certain number of degrees, pivoting about the back left wheel. Currently a maximum of a 90 degree turn

void Drive::TurnAmount(int degrees, Pivot pivot) // degrees < 0 means right turn, degrees > 0 means left turn
{
    // For storing the current heading
    int curr_heading = rps->Heading();
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
        curr_heading = rps->Heading();

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
            if(pivot == LEFT){ SetDriveLR(0, /*70*/turn_min_power - pTurn * error); }
            if(pivot == RIGHT){ SetDriveLR(/*-70*/-turn_min_power + pTurn * error, 0); }
        }
        if(moving_right)
        {
            if(pivot == LEFT){ SetDriveLR(0, /*-70*/-turn_min_power - pTurn * error); }
            if(pivot == RIGHT){ SetDriveLR(/*70*/turn_min_power + pTurn * error, 0); }
        }

        if(error > 5) // Need to go more to the right
        {
            if (moving_right) {} // Already going right. Continue
            if (moving_left) // Moving to the left. Need to stop and change directions to correct for overshoot
            {
                /*SetDriveRL(0, 0);
                Sleep(0.5);
                moving_left = false;
                moving_right = true;*/
                SetDriveLR(0, 0);
                break;
            }
        }
        else if(error < -5) // Need to go more to the left
        {
            if (moving_left) {} // Already going left. Continue
            if (moving_right) // Moving to the right. Need to stop and change directions to correct for overshoot
            {
                /*SetDriveRL(0, 0);
                Sleep(0.5);
                moving_left = true;
                moving_right = false;*/
                SetDriveLR(0, 0);
                break;
            }
        }
        else
        {
            SetDriveLR(0, 0);
            break; //Success
        }

        Sleep(0.010);
    }

}

void Drive::TurnAngle(int degrees, Pivot direction, Pivot pivot) // degrees < 0 means right turn, degrees > 0 means left turn
{
    int curr_heading = rps->Heading();
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

void Drive::SquareToWallForward()
{
    while(right_switch->Value() == 1 && left_switch->Value() == 1)
    {
        SetDriveLR(left_switch->Value() == 1 ? 100 : 0, right_switch->Value() == 1 ? 100 : 0);
        Sleep(.020);
    }
}
