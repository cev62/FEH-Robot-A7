#ifndef DRIVE_H
#define DRIVE_H

#include <FEHMotor.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHWONKA.h>

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
    float P_TURN;
    float TURN_MIN_POWER;
private:
    FEHMotor *left, *right;
};

#endif // DRIVE_H
