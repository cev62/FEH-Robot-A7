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
    } Pivot;
    Drive(FEHMotor *left_in, FEHMotor *right_in, DigitalInputPin *right_switch_in, DigitalInputPin *left_switch_in, FEHWONKA *rps_in);
    void SetDrive(int forward, int turn);
    void SetDriveLR(int left_percent_in, int right_percent_in);
    void SetDriveTime(int forward, int turn, float time);
    void PushButton();
    void SquareToWallForward();
    void SquareToWallBackward();
    void TurnAmount(int degrees, Pivot pivot);
    void TurnAngle(int degrees, Pivot direction, Pivot pivot);

private:
    FEHMotor *left, *right;
    DigitalInputPin *right_switch, *left_switch;
    int forward, turn;
    FEHWONKA *rps;
    float pTurn;
    float turn_min_power;
};

#endif // DRIVE_H
