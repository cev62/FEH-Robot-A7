#ifndef DRIVE_H
#define DRIVE_H

#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHWONKA.h>

const float pTurn = 20.0 / 45.0;
const float TURN_MIN_POWER = 60;

class Drive
{
public:
    typedef enum
    {
        LEFT = 0,
        RIGHT
    } Side;
    Drive(FEHMotor *left_in, FEHMotor *right_in);
    void SetDrive(int forward, int turn);
    void SetDriveLR(int left_percent_in, int right_percent_in);
private:
    FEHMotor *left, *right;
};

#endif // DRIVE_H
