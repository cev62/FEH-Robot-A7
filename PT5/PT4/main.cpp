#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHWONKA.h>

#include "control.h"
#include "drive.h"

// Function prototypes
void Competition();
void PT1();
void PT2();
void PT2_RPS();
void PT3();
void PT4();
void PT4_absolute_turns();
void PT5();
void PT6();
void PT7();

//typedef struct Constants
//{
//    const int cds_cell_threshhold = 0.5;
//} Constants;

// Variable Declarations
ButtonBoard *button_board;
AnalogInputPin *cds_cell, *optosensor;
DigitalInputPin *right_switch, *left_switch, *arm_switch;
FEHMotor *left, *right;
FEHServo *arm;
Control *control;
Drive *drive;
FEHLCD *lcd;
FEHWONKA RPS;
int button_presses;

const int ARM_STORE = 45;
const int ARM_APPROACH_SKID = 165;
const int ARM_PICKUP_SKID = 90;
const int ARM_SENSE_PIN = 120;
const int ARM_APPROACH_PIN = 130;
const int ARM_PULL_PIN = 90;

int main(void)
{
    // Variable Initializations
    button_board = new ButtonBoard(FEHIO::Bank3);
    cds_cell = new AnalogInputPin(FEHIO::P0_0);
    optosensor = new AnalogInputPin(FEHIO::P0_4);
    left_switch = new DigitalInputPin(FEHIO::P0_2);
    right_switch = new DigitalInputPin(FEHIO::P0_1);
    arm_switch = new DigitalInputPin(FEHIO::P0_3);
    left = new FEHMotor(FEHMotor::Motor0);
    right = new FEHMotor(FEHMotor::Motor1);
    arm = new FEHServo(FEHServo::Servo0);
    arm->SetMin(500);
    arm->SetMax(2431);
    lcd = &LCD;
    control = new Control(lcd, button_board, cds_cell);
    drive = new Drive(left, right, right_switch, left_switch, &RPS);

    while(true)
    {

    lcd->Clear( FEHLCD::Black );
    lcd->SetFontColor( FEHLCD::White );

    // BEGIN SCRIPT
    PT5();
    }

    return 0;
}

void PT5(){

    //RPS.InitializeMenu(); //call the region config menu
    //RPS.Enable(); //enable the RPS

    control->WaitForMiddleButton();
    control->WaitForStartLight();

    // Drive to PIN
    arm->SetDegree(ARM_STORE);
    drive->SetDriveTime(100, 0, 2.0);
    Sleep(1.0);
    arm->SetDegree(ARM_SENSE_PIN);
    // Line follow until the pin is sensed

    float GRAY_VALUE = optosensor->Value();
    float DELTA_THRESH = 0.3;
    float THRESHOLD = 1.0;
    float ADD_TO_OPPOSITE_MOTOR = 0;
    float start_time = TimeNow();
    while(arm_switch->Value() && TimeNow() - 8.0 < start_time)
    {
        float value = optosensor->Value();
        //bool isOnLine = value < THRESHOLD; // Old way
        bool isOnLine = value < GRAY_VALUE - DELTA_THRESH; // New delta way
        lcd->Write(isOnLine ? "ON " : "OFF");
        lcd->Write(value);
        lcd->Write("    ");
        lcd->Write(GRAY_VALUE - DELTA_THRESH);
        if(isOnLine)
        {
            // Need to go right
            drive->SetDriveLR(65, ADD_TO_OPPOSITE_MOTOR);
        }
        else
        {
            // Need to go left
            drive->SetDriveLR(ADD_TO_OPPOSITE_MOTOR, 65);
        }
        Sleep(0.010);
    }
    drive->SetDrive(0, 0);
    Sleep(1.0);
    arm->SetDegree(ARM_PULL_PIN);

    drive->TurnAngle(85, Drive::LEFT, Drive::LEFT);
    arm->SetDegree(ARM_APPROACH_SKID);
    Sleep(1.0);
    drive->SetDriveTime(70, 0, 1.0);
    Sleep(1.0);
    arm->SetDegree(ARM_PICKUP_SKID);
    drive->SetDriveTime(-100, 0, 0.3);
    Sleep(1.0);
    drive->SetDriveTime(100, 0, 0.5);
    drive->SetDriveTime(-100, 0, 0.3);
    Sleep(3.0);
    drive->TurnAngle(0, Drive::LEFT, Drive::LEFT);
    drive->TurnAngle(90, Drive::LEFT, Drive::LEFT);



    // Drive down ramp
    //drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(1.0);
    drive->SetDriveTime(100, 0, 2.0);
}

void PT5_acceleration_line_following(){

    RPS.InitializeMenu(); //call the region config menu
    RPS.Enable(); //enable the RPS

    control->WaitForMiddleButton();
    control->WaitForStartLight();
    bool previous, isOnLine = false;
    int accel = 0;

    // Drive to PIN
    arm->SetDegree(ARM_STORE);
    drive->SetDriveTime(100, 0, 2.0);
    Sleep(1.0);
    arm->SetDegree(ARM_SENSE_PIN);
    // Line follow until the pin is sensed

    float THRESHOLD = 1.0;
    while(arm_switch->Value())
    {
        float value = optosensor->Value();
        previous = isOnLine;
        isOnLine = value < THRESHOLD;
        if(accel < 50)
            accel++;
        if(previous != isOnLine)
            accel = 0;

        lcd->Write(isOnLine ? "ON " : "OFF");
        lcd->WriteLine(value);
        if(isOnLine)
        {
            // Need to go right
            drive->SetDriveLR(50+accel, 0);
        }
        else
        {
            // Need to go left
            drive->SetDriveLR(0, 50+accel);
        }
        Sleep(0.010);
    }
    drive->SetDrive(0, 0);
    Sleep(1.0);
    drive->SetDriveTime(-70, 0, 0.5);
    arm->SetDegree(ARM_APPROACH_PIN);
    Sleep(1.0);
    drive->SetDriveTime(70, 0, 0.25);
    Sleep(1.0);
    arm->SetDegree(ARM_PULL_PIN);
    //drive->SetDriveTime(-100, 0, 0.5); // Already in position

    // Drive down ramp
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    drive->SetDriveTime(100, 0, 2.0);
}

void PT4_absolute_turns()
{
    RPS.InitializeMenu(); //call the region config menu
    RPS.Enable(); //enable the RPS

    lcd->WriteLine("FEH Team A7: P.T. 5");
    lcd->Write("# of button pushes: ");
    button_presses = RPS.Oven();
    lcd->WriteLine(button_presses);
    control->WaitForMiddleButton();
    control->WaitForStartLight();

    // Press Button
    drive->SetDriveTime(-100, 0, 0.5);
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    lcd->WriteLine(RPS.Heading());
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->SetDriveTime(100,0,0.37);
    Sleep(1.0);
    lcd->WriteLine("Turn 2");
    lcd->WriteLine(RPS.Heading());
    drive->TurnAngle(0, Drive::LEFT, Drive::LEFT); // BROKEN
    lcd->WriteLine(RPS.Heading());
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->SetDriveTime(50, 0, 1.0);
    //for(int i = 0; i < button_presses; i++)
    //{
        drive->PushButton();
    //}

    // Drive to Switch
    drive->SetDriveTime(-100, 0, 0.9);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();
    Sleep(0.5);
    drive->TurnAngle(120, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();
    Sleep(0.5);

    // Drive to PIN
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SetDriveTime(50, 0, 6.0);
    drive->SetDriveTime(0, -50, 2.0);
}

void PT4()
{
    RPS.InitializeMenu(); //call the region config menu
    RPS.Enable(); //enable the RPS

    lcd->WriteLine("FEH Team A7: P.T. 4");
    lcd->Write("# of button pushes: ");
    button_presses = RPS.Oven();
    lcd->WriteLine(button_presses);
    control->WaitForMiddleButton();
    control->WaitForStartLight();

    // Press Button
    drive->SetDriveTime(-100, 0, 0.5);
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->TurnAmount(-90, Drive::LEFT);
    lcd->WriteLine(RPS.Heading());
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->SetDriveTime(100,0,0.37);
    Sleep(1.0);
    lcd->WriteLine("Turn 2");
    lcd->WriteLine(RPS.Heading());
    drive->TurnAmount(90, Drive::LEFT);
    lcd->WriteLine(RPS.Heading());
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->SetDriveTime(50, 0, 1.0);
    //for(int i = 0; i < button_presses; i++)
    //{
        drive->PushButton();
    //}

    // Drive to Switch
    drive->SetDriveTime(-100, 0, 0.9);
    Sleep(0.5);
    drive->TurnAmount(-90, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();
    Sleep(0.5);
    drive->TurnAmount(30, Drive::RIGHT);
    Sleep(0.5);
    drive->TurnAmount(-30, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();
    Sleep(0.5);

    // Drive to PIN
    drive->TurnAmount(-90, Drive::LEFT);
    Sleep(0.5);
    drive->SetDriveTime(50, 0, 6.0);
    drive->SetDriveTime(0, -50, 2.0);
}

void PT1()
{
    lcd->WriteLine("FEH Team A7: P.T. 1");
    control->WaitForMiddleButton();

    drive->SetDriveTime(100, 0, 1.75);
    drive->SetDriveTime(100, -100, 1.2);
    drive->SetDriveTime(100, 0, 1.75);
}

void PT2()
{
    lcd->WriteLine("FEH Team A7: P.T. 2");
    control->WaitForMiddleButton();
    control->WaitForStartLight();

    drive->SetDriveTime(-100, 0, 1.0);
    Sleep(0.3);
    drive->SetDriveTime(0, 100, 0.7);
    Sleep(0.3);
    drive->SetDriveTime(100, 0, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(0, -100, 0.7);
    Sleep(0.3);
    drive->PushButton();
    //drive->SetDriveTime(-100, 0, 0.75);
}

void PT2_RPS()
{
    /*RPS.InitializeMenu(); //call the region config menu
    RPS.Enable(); //enable the RPS

    lcd->WriteLine("FEH Team A7: P.T. 2 RPS");
    lcd->Write("# of button pushes: ");
    button_presses = RPS.Oven();
    lcd->WriteLine(button_presses);
    control->WaitForMiddleButton();
    control->WaitForStartLight();

    drive->SetDriveTime(-100, 0, 1.0);
    Sleep(1.0);
    lcd->WriteLine(RPS.Heading());
    drive->TurnRightAboutLeft(90);
    lcd->WriteLine(RPS.Heading());
    Sleep(1.0);
    drive->SetDriveTime(100,0,0.37);
    Sleep(1.0);
    drive->TurnAboutLeft(90);
    Sleep(1.0);
    drive->PushButton();
    Sleep(0.5);
    drive->TurnAboutLeft(12);
    Sleep(0.5);
    drive->PushButton();
    Sleep(0.5);
    drive->TurnRightAboutLeft(15);
    Sleep(0.5);
    drive->PushButton();
    /*for(int i = 0; i < button_presses; i++)
    {
        drive->PushButton();
    }*/
    //drive->SetDriveTime(-100, 0, 0.75);*/
}


