#ifndef DRIVE_H
#define DRIVE_H

#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHWONKA.h>

#include "io.h"

class Drive
{
public:

    static const float P_TURN= 20.0 / 45.0;
    static const float TURN_MIN_POWER_FACTORY = 50; // 50 for factory, 60 for shop;
    static const float TURN_MIN_POWER_SHOP = 60; // 50 for factory, 60 for shop;

    static const float STRAIGHT_MIN_POWER = 60;
    static const float RAMP_DOWN_DIST = 6.0;

    typedef enum
    {
        LEFT = 0,
        RIGHT
    } Side;

    Drive(FEHMotor *left_in, FEHMotor *right_in, IO *io_in);
    void SetDrive(int forward, int turn);
    void SetDriveLR(int left_percent_in, int right_percent_in);
    void SetDriveTime(int forward, int turn, float time);
    void PushButton();
    void SquareToWallForward();
    void SquareToWallBackward();
    void TurnAmount(int degrees, Drive::Side pivot);
    void TurnAngle(int degrees, Drive::Side direction, Drive::Side pivot);
    void DriveDist(int forward, float dist);
    void EncoderTurn(int angle);

    float TURN_MIN_POWER;

    FEHMotor *left, *right;
    IO *io;
};

#endif // DRIVE_H
