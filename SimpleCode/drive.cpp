#include "drive.h"

Drive::Drive(FEHMotor *left_in, FEHMotor *right_in, IO *io_in)
{
    left = left_in;
    right = right_in;
    io = io_in;
    TURN_MIN_POWER = Drive::TURN_MIN_POWER_FACTORY;
    timer = new Timer();
    coord_pid = new PIDController(Drive::COORD_PID_P, Drive::COORD_PID_I, Drive::COORD_PID_D);
    pid_mode = Drive::OFF;
}

void Drive::SetDrive(int forward, int turn)
{
    int left_percent, right_percent;
    if(pid_mode == Drive::OFF)
    {
        left_percent = forward + turn;
        right_percent = forward - turn;
    }
    else if(pid_mode == Drive::X || pid_mode == Drive::Y)
    {
        float pid_turn;
        if(!io->IsRPSGood())
        {
            Sleep(1.0);
            if(!io->IsRPSGood())
            {
                pid_mode = Drive::OFF;
            }
            else
            {
                io->lcd->Clear(FEHLCD::Black);
            }
        }
        if(pid_mode == Drive::Y)
        {
            pid_turn = coord_pid->GetOutput(io->rps_y);
        }
        else
        {
            pid_turn = coord_pid->GetOutput(io->rps_x);
        }
        int left_percent = forward + pid_turn;
        int right_percent = forward - pid_turn;
        io->lcd->Write("Pid Turn: ");
        io->lcd->WriteLine(pid_turn);
    }

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

void Drive::DriveUntilFL()
{
    SetDrive(100, 0);
    while(io->fl_switch->Value() == 1)
    {
        Sleep(IO::LOOP_TIMEOUT);
    }
    SetDrive(0, 0);
    Sleep(0.3);
}

// Turns the robot a certain number of degrees, pivoting about the back left wheel. Currently a maximum of a 90 degree turn
// TODO: add failure timeout
void Drive::TurnAmount(int degrees, Drive::Side pivot) // degrees < 0 means right turn, degrees > 0 means left turn
{
    // If we don't have RPS, dump it off to the encoder turn function
    if(!io->IsRPSGood())
    {
        SetDrive(0, 0);
        io->lcd->WriteLine("INIT");
        io->lcd->WriteLine(degrees);
        EncoderTurn(degrees, pivot);
        return;
    }

    timer->Reset();
    timer->SetTimeout(5.0);

    // For storing the current heading
    int curr_heading = io->rps_heading;
    // For storing te initial heading
    int initial_heading = curr_heading;
    // Stores the target heading NOT on the [0-179] scale. This is literally just adding the degrees to the initial heading
    int raw_target_heading = initial_heading + degrees;
    // Sotres the target heading converted to the [0-179] scale that the RPS uses
    int target_heading = raw_target_heading;

    int error = degrees;

    // Flags for telling if the robot is going to cross from 0 to 179 or vice versa.
    bool crossing_0_right = false, crossing_0_left = false, moving_left = degrees > 0, moving_right = degrees < 0;
    if (raw_target_heading > 179) {target_heading -= 180; crossing_0_left = true;}
    if (raw_target_heading < 0) {target_heading += 180; crossing_0_right = true;}

    while (true)
    {

        // Failure condition if it times out
        if(timer->IsTimeout())
        {
            SetDrive(0, 0);
            return;
        }

        // If we don't have RPS, dump it off to the encoder turn function
        if(!io->IsRPSGood())
        {
            // Because of the place in the function, error will always have been set
            // when the RPS was good, so this call to Encoder turn should have the
            // right angle
            SetDrive(0, 0);
            io->lcd->WriteLine("INSIDE LOOP");
            io->lcd->WriteLine(error);
            EncoderTurn(error, pivot);
            return;
        }

        // TODO: Change this so that we KNOW that rps is good
        curr_heading = io->rps_heading;

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
        error = raw_curr_heading - raw_target_heading;

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
    Sleep(0.5);
    if(io->fl_switch->Value() || io->fr_switch->Value())
    {
        SquareToWallForward();
    }
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
    Sleep(0.5);
    if(io->bl_switch->Value() || io->br_switch->Value())
    {
        SquareToWallBackward();
    }
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

void Drive::LineFollowPin()
{
    while(true)
    {
        if(io->IsOnLinePin())
        {
            // Need to go right
            SetDriveLR(65, 30);
        }
        else
        {
            // Need to go left
            SetDriveLR(30, 55);
        }

        if(!io->arm_switch->Value())
        {
            SetDrive(0, 0);
            return;
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
}

void Drive::EncoderTurn(float angle, Drive::Side pivot)
{

    float countsPerDegree = .777777777;
    float inches_per_degree = 9.0 / 90;
    float motorPower;
    io->ResetEncoders();

    if(pivot == Drive::LEFT)
    {
        if(angle < 0)
        {
            angle *= -1;
            io->lcd->Write("Turn Left  ");
            io->lcd->WriteLine(countsPerDegree*angle);
            while(io->right_encoder->Counts() < (countsPerDegree*angle*0.9))
            {
                motorPower = -100 + ((io->right_encoder->Counts()/(countsPerDegree*angle*0.9))*40);
                SetDriveLR(0, -motorPower);
            }
        }
        else
        {
            io->lcd->Write("Turn Right ");
            io->lcd->WriteLine(countsPerDegree*angle);
            while(io->right_encoder->Counts() < (countsPerDegree*angle))
            {
                io->lcd->Clear();
                io->lcd->WriteLine("Counts");
                io->lcd->WriteLine(io->right_encoder->Counts());
                motorPower = -100 + ((io->right_encoder->Counts()/(countsPerDegree*angle))*40);
                SetDriveLR(0, motorPower);
            }
        }
    }
    else
    {
        if(angle < 0)
        {
            angle *= -1;
            io->lcd->Write("Turn Left  ");
            io->lcd->WriteLine(countsPerDegree*angle);
            while(io->left_encoder->Counts() < (countsPerDegree*angle*0.9))
            {
                motorPower = -100 + ((io->left_encoder->Counts()/(countsPerDegree*angle*0.9))*40);
                SetDriveLR(motorPower, 0);
            }
        }
        else
        {
            io->lcd->Write("Turn Right ");
            io->lcd->WriteLine(countsPerDegree*angle);
            while(io->left_encoder->Counts() < (countsPerDegree*angle))
            {
                io->lcd->Clear();
                io->lcd->WriteLine("Counts");
                io->lcd->WriteLine(io->left_encoder->Counts());
                motorPower = -100 + ((io->left_encoder->Counts()/(countsPerDegree*angle))*40);
                SetDriveLR(-motorPower, 0);
            }
        }
    }

    io->lcd->Clear(FEHLCD::Black);
    SetDrive(0, 0);
}
