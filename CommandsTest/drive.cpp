#include "drive.h"

Drive::Drive(FEHMotor *left_in, FEHMotor *right_in)
{
    left = left_in;
    right = right_in;
}

Drive::SetDrive(int forward, int turn)
{
    // Combine inputs to left and right percentages
    int left_percent = forward + turn;
    int right_percent = forward - turn;

    // Cap the values at +/- 100
    if(left_percent > 100){ left_percent = 100; }
    if(left_percent < -100){ left_percent = -100; }
    if(right_percent > 100){ right_percent = 100; }
    if(right_percent < -100){ right_percent = -100; }

    // Actuate motors
    left->SetPercent(-left_percent);
    right->SetPercent(right_percent);
}

Drive::SetDriveLR(int left_percent_in, int right_percent_in)
{
    // Set motor values directly
    left->SetPercent(left_percent_in);
    right->SetPercent(right_percent_in);
}
