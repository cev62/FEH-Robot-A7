#include "drive.h"

Drive::Drive(FEHMotor *left_in, FEHMotor *right_in, IO *io_in)
{
    left = left_in;
    right = right_in;
    io = io_in;
    TURN_MIN_POWER = Drive::TURN_MIN_POWER_FACTORY;
    timer = new Timer();
    curr_angle = 0.0;
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
    timer->Reset();
    timer->SetTimeout(2.0);
    SetDrive(50, 0);
    while(io->fr_switch->Value() == 1)
    {
        if(timer->IsTimeout())
        {
            break;
        }
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
    // If we don't have RPS, dump it off to the encoder turn function
    if(!io->is_rps_enabled || !io->IsRPSGood())
    {
        SetDrive(0, 0);
        EncoderTurn(-degrees, pivot);
        return;
    }

    timer->Reset();
    timer->SetTimeout(5.0);

    // For storing the current heading
    int curr_heading = io->rps_heading;
    int prev_heading = curr_heading;
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
        prev_heading = curr_heading;

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
                //if(pivot == Drive::LEFT) {SetDriveLR(0, -TURN_MIN_POWER);}
                //if(pivot == Drive::RIGHT) {SetDriveLR(TURN_MIN_POWER, 0);}
                SetDrive(0, 0);
                break;
            }
        }
        else if(error < -5) // Need to go more to the left
        {
            if (moving_left) {} // Already going left. Continue
            if (moving_right) // Moving to the right. Need to stop and change directions to correct for overshoot
            {
                //if(pivot == Drive::LEFT) {SetDriveLR(0, TURN_MIN_POWER);}
                //if(pivot == Drive::RIGHT) {SetDriveLR(-TURN_MIN_POWER, 0);}
                SetDrive(0, 0);
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
    curr_angle = (int)(curr_angle + degrees + 180) % 180;
    Sleep(0.25);
}

void Drive::TurnAngle(int degrees, Drive::Side direction, Drive::Side pivot) // degrees < 0 means right turn, degrees > 0 means left turn
{
    timer->Reset();
    timer->SetTimeout(1.0);
    int curr_heading;

    if(io->is_rps_enabled)
    {
        if(!io->IsRPSGood())
        {
            Sleep(1.0);
            if(!io->IsRPSGood())
            {
                curr_heading = curr_angle;
            }
            else
            {
                curr_heading = io->rps_heading;
            }
        }
        else
        {
            curr_heading = io->rps_heading;
        }
    }
    else
    {
        curr_heading = curr_angle;
    }

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

    Timer *twitch_detector = new Timer();
    twitch_detector->Reset();
    int retry_counts = 0;

    // If the amount to turn is less than 90 degrees, TurnAmount can handle it, so just call the function
    if(amount_to_turn <= 90 && amount_to_turn >= -90)
    {
        while(twitch_detector->GetTime() < 0.25)
        {
            twitch_detector->Reset();
            TurnAmount(amount_to_turn, pivot);
            retry_counts++;
            if(retry_counts >= 3)
            {
                break;
            }
        }
    }
    // If the amount to turn is more than 90 degrees, TurnAmount can NOT handle it, so split the turn into two turns back to back
    else
    {
        while(twitch_detector->GetTime() < 0.25)
        {
            twitch_detector->Reset();
            TurnAmount(amount_to_turn / 2, pivot);
            TurnAmount(amount_to_turn / 2, pivot);
            retry_counts++;
            if(retry_counts >= 3)
            {
                break;
            }
        }
    }
}

void Drive::SquareToWallForward()
{
    timer->Reset();
    timer->SetTimeout(5.0);
    while(true)
    {
        SetDriveLR(io->fl_switch->Value() ? 100 : 0, io->fr_switch->Value() ? 100 : 0);
        //SetDrive(100, (io->fl_switch->Value() ? 0 : -100) + (io->fr_switch->Value() ? 0 : 100));
        Sleep(IO::LOOP_TIMEOUT);
        if(!io->fl_switch->Value() && !io->fr_switch->Value())
        {
            break;
        }
        if(timer->IsTimeout())
        {
            SetDrive(0, 0);
            Sleep(0.3);
            return;
        }
    }
    SetDrive(0, 0);
    Sleep(0.3);
    if(io->fl_switch->Value() || io->fr_switch->Value())
    {
        SquareToWallForward();
    }
}

// TODO: add failure timeout
void Drive::SquareToWallBackward()
{
    timer->Reset();
    timer->SetTimeout(5.0);
    while(true)
    {
        SetDriveLR(io->bl_switch->Value() ? -100 : 0, io->br_switch->Value() ? -100 : 0);
        //SetDrive(-100, (io->bl_switch->Value() ? 0 : 100) + (io->br_switch->Value() ? 0 : -100));
        if(!io->bl_switch->Value() && !io->br_switch->Value())
        {
            break;
        }
        if(timer->IsTimeout())
        {
            SetDrive(0, 0);
            Sleep(0.3);
            return;
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
    SetDrive(0, 0);
    Sleep(0.3);
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
    timer->Reset();
    timer->SetTimeout(10.0);
    Timer *left_timer = new Timer();
    left_timer->SetTimeout(2.0);
    while(true)
    {
        if(io->IsOnLinePin())
        {
            // Need to go right
            SetDriveLR(65, 30);
            left_timer->Reset();
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
        if(timer->IsTimeout())
        {
            SetDriveTime(-100, 0, 0.6);
            EncoderTurn(30, Drive::LEFT);
            TurnToLine();
            continue;
        }
        if(left_timer->IsTimeout())
        {
            SetDriveLR(-65, -30);
            Sleep(2.0);
            EncoderTurn(40, Drive::LEFT);
            TurnToLine();
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
}

void Drive::LineFollowSkid()
{
    timer->Reset();
    timer->SetTimeout(2.5);
    while(true)
    {
        if(!io->IsOnLinePin())
        {
            // Need to go right
            SetDriveLR(65, 30);
        }
        else
        {
            // Need to go left
            SetDriveLR(30, 65);
        }
        if(timer->IsTimeout())
        {
            SetDrive(0, 0);
            Sleep(0.3);
            return;
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
}

void Drive::TurnToLine()
{
    timer->Reset();
    timer->SetTimeout(5.0);
    while(true)
    {
        if(!io->IsOnLinePin())
        {
            // Need to go left
            SetDriveLR(0, 70);
        }
        else
        {
            SetDrive(0, 0);
            Sleep(0.3);
            return;
        }
        if(timer->IsTimeout())
        {
            SetDrive(0, 0);
            Sleep(0.3);
            return;
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
}


void Drive::EncoderTurn(float angle, Drive::Side pivot)
{
    io->ResetEncoders();
    FEHEncoder *encoder;
    float motor_power = 0;
    float motor_direction = 1;
    float correction_factor = /*0.77*/0.77 / 0.55;

    if(angle < 0 && pivot == Drive::LEFT)
    {
        // Left Turn about LEFT
        encoder = io->right_encoder;
        motor_direction = 1;
    }

    if(angle < 0 && pivot == Drive::RIGHT)
    {
        // Left Turn about RIGHT
        encoder = io->left_encoder;
        motor_direction = -1;
    }

    if(angle > 0 && pivot == Drive::LEFT)
    {
        // Right Turn about LEFT
        encoder = io->right_encoder;
        motor_direction = -1;
    }

    if(angle > 0 && pivot == Drive::RIGHT)
    {
        // Right Turn about RIGHT
        encoder = io->left_encoder;
        motor_direction = 1;
    }

    if(angle < 0){ angle = -angle; }
    float target_counts = angle*IO::INCHES_PER_DEGREE*IO::COUNTS_PER_INCH * correction_factor;
    if(pivot == Drive::RIGHT)
    {
        target_counts = target_counts * 1.0;//0.9;
    }

    while(true)
    {
        motor_power = TURN_MIN_POWER + (target_counts - encoder->Counts()) * Drive::P_TURN;
        if(pivot == Drive::LEFT)
        {
            SetDriveLR(0, motor_direction * motor_power);
        }
        if(pivot == Drive::RIGHT)
        {
            SetDriveLR(motor_direction * motor_power, 0);
        }
        Sleep(IO::LOOP_TIMEOUT);
        if(encoder->Counts() > target_counts)
        {
            break;
        }
    }
    SetDrive(0, 0);

    curr_angle = (int)(curr_angle - angle + 180) % 180;
}
