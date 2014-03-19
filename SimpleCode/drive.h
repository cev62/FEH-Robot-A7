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
    void TurnAmount(int degrees, Pivot pivot);
    void TurnAngle(int degrees, Pivot direction, Pivot pivot);

    FEHMotor *left, *right;
    IO *io;
    int forward, turn;
    float pTurn;
    float turn_min_power;
};

#endif // DRIVE_H
