#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHWONKA.h>
#include <FEHServo.h>
#include <FEHMotor.h>

#include "timer.h"
#include "io.h"
#include "drive.h"

// Function prototypes
void RunScript(char* script);
void pt7();
void pt7_bonus();
void test();
void comp();
void individual_comp();
void skid();
void coord_pid_test();
void encoderTest();
void chiller();
void drive_down_ramp();

// Variable Declarations
ButtonBoard *button_board;
AnalogInputPin *cds_cell, *optosensor;
DigitalInputPin *fl_switch, *fr_switch, *bl_switch, *br_switch, *arm_switch;
FEHMotor *left, *right;
FEHServo *arm, *box;
FEHEncoder *left_encoder, *right_encoder;
IO *io;
Drive *drive;
FEHLCD *lcd;
FEHWONKA RPS, *rps;
Timer *print_timer, *timer;

int button_presses, script_position, num_scripts;
char **scripts;

int main(void)
{
    button_board = new ButtonBoard(FEHIO::Bank3);
    cds_cell = new AnalogInputPin(FEHIO::P0_0);
    fr_switch = new DigitalInputPin(FEHIO::P0_1);
    fl_switch = new DigitalInputPin(FEHIO::P0_2);
    arm_switch = new DigitalInputPin(FEHIO::P0_3);
    optosensor = new AnalogInputPin(FEHIO::P0_4);
    left_encoder = new FEHEncoder(FEHIO::P1_0);
    right_encoder = new FEHEncoder(FEHIO::P1_1);
    br_switch = new DigitalInputPin(FEHIO::P2_0);
    bl_switch = new DigitalInputPin(FEHIO::P2_1);

    left = new FEHMotor(FEHMotor::Motor0);
    right = new FEHMotor(FEHMotor::Motor1);
    arm = new FEHServo(FEHServo::Servo0);
    box = new FEHServo(FEHServo::Servo1);
    arm->SetMin(500);
    arm->SetMax(2431);
    box->SetMin(500);
    box->SetMax(2500);

    lcd = &LCD;
    lcd->Clear( FEHLCD::Black );
    lcd->SetFontColor( FEHLCD::White );
    rps = &RPS;
    print_timer = new Timer();
    timer = new Timer();

    io = new IO(print_timer, button_board, lcd, rps, left_encoder, right_encoder, fl_switch, fr_switch, bl_switch, br_switch, arm_switch, optosensor, cds_cell);
    drive = new Drive(left, right, io);

    num_scripts = 11;
    scripts = new char*[num_scripts];
    scripts[0] = "comp";
    scripts[8] = "skid";
    scripts[1] = "individual comp";
    scripts[2] = "pt7_bonus";
    scripts[3] = "pt7";
    scripts[4] = "test";
    scripts[5] = "encoder test";
    scripts[6] = "coord pid test";
    scripts[7] = "chiller";
    scripts[9] = "drive down ramp";
    scripts[10] = "Toggle RPS"; //must be last script in array
    script_position = 0;
    io->is_rps_enabled = true;

    // Script Chooser
    while(true)
    {
        io->Update();
        if(io->ButtonBoardGetPressedEvent(IO::MIDDLE))
        {
            // Middle Button pressed: run the current selected script
            RunScript(scripts[script_position]);
            continue;
        }
        if(io->ButtonBoardGetPressedEvent(IO::LEFT))
        {
            // Left button pressed: move the script selector down
            script_position -= 1;
            if(script_position < 0)
            {
                script_position += num_scripts;
            }
        }
        if(io->ButtonBoardGetPressedEvent(IO::RIGHT))
        {
            // Right button pressed: move the script selector up
            script_position += 1;
            if(script_position > num_scripts - 1)
            {
                script_position -= num_scripts;
            }
        }
        if(print_timer->IsTimeout())
        {
            // Print status of script choosing
            lcd->Clear(FEHLCD::Black);
            lcd->Write("Selected: ");
            lcd->WriteLine(scripts[script_position]);
            lcd->WriteLine("Side Buttons --> choose");
            lcd->WriteLine("Middle button --> run");
            lcd->Write("RPS is: ");
            lcd->WriteLine(io->is_rps_enabled ? "Enabled" : "Disabled");
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
    return 0;
}

void RunScript(char *script)
{

    // TODO: Use function pointers :)

    // Conditionally init RPS
    if(io->is_rps_enabled && script != "Toggle RPS")
    {
        // Sleep to let the middle button become un-pressed
        Sleep(1.0);
        rps->InitializeMenu(); //call the region config menu
        rps->Enable(); //enable the RPS
    }

    if(script == "pt7"){ pt7(); }
    else if(script == "pt7_bonus"){ pt7_bonus(); }
    else if(script == "test"){ test(); }
    else if(script == "individual comp"){ individual_comp(); }
    else if(script == "comp"){ comp(); }
    else if(script == "skid"){ skid(); }
    else if(script == "chiller"){ chiller(); }
    else if(script == "encoder test") { encoderTest(); }
    else if(script == "coord pid test") { coord_pid_test(); }
    else if(script == "drive down ramp") { drive_down_ramp(); }
    else if(script == "Toggle RPS"){ io->is_rps_enabled = !io->is_rps_enabled; }
    else {lcd->Clear(FEHLCD::Gray); lcd->WriteLine("Invalid Script"); Sleep(10.0);}
}

void comp()
{
    // Init
    io->WaitForStartLight();
    arm->SetDegree(IO::ARM_STORE);
    box->SetDegree(IO::BOX_STORE);
    io->num_button_pushes_required = rps->Oven();
    lcd->Write("Buttons: ");
    lcd->Write(io->num_button_pushes_required);

    // Drive to button
    //drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    // Encoder turn because it doesn't have to be precise and it won't fail
    // and ram into the charge zone button like an rps turn might
    drive->EncoderTurn(-90, Drive::RIGHT);

    Sleep(0.3);
    drive->SquareToWallForward();
    drive->DriveDist(-100, 0.5);
    Sleep(0.3);
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(100, 3);

    // Push button multiple times
    drive->PushButton();
    int times_attempted_press = 1;
    while(rps->OvenPressed() < io->num_button_pushes_required && times_attempted_press < 6)
    {
        drive->PushButton();
        times_attempted_press++;
    }

    // Drive to Switch but do not flip
    drive->DriveDist(-100, 16);
    Sleep(0.3);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallForward();
    io->InitializeLineFollowingPin();

    // Drive to PIN
    drive->DriveDist(-100, 2.5);
    Sleep(0.3);
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    arm->SetDegree(IO::ARM_SENSE_PIN);
    Sleep(0.3);
    drive->DriveDist(100, 1.5);
    drive->TurnToLine();

    // Sense the CHUTE
    drive->LineFollowPin();
    Sleep(0.5);

    // Pull Pin
    arm->SetDegree(IO::ARM_PULL_PIN);
    drive->DriveDist(-100, 6);
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    Sleep(1.0);
    arm->SetDegree(IO::ARM_SENSE_PIN);

    // Drive to skid
    drive->SetDriveTime(0, 50, 0.3);
    drive->LineFollowPin();
    arm->SetDegree(IO::ARM_STORE);
    Sleep(0.5);
    drive->SetDriveTime(100, -100, 0.5);
    drive->TurnToLine();
    //drive->SetDriveTime(100, -100, 0.3);
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    Sleep(1.0);
    drive->LineFollowSkid();

    // Pick up skid
    arm->SetDegree(IO::ARM_PICKUP_SKID);
    Sleep(0.3);
    drive->DriveDist(-100, 2);
    Sleep(1.5);
    drive->SetDriveTime(100, 0, 0.5);
    Sleep(0.5);
    drive->DriveDist(-100, 2);
    Sleep(0.5);
    //drive->SetDriveTime(0, 50, 0.5);

    // Drive down ramp
    drive->SquareToWallBackward();
    drive->DriveDist(100, 6.75);
    Sleep(0.3);
    drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
    //drive->EncoderTurn(-90, Drive::RIGHT);
    Sleep(0.3);
    drive->SetDriveLR(-60, -40);
    Sleep(1.2);
    //drive->DriveDist(100, 15.5);
    drive->DriveDist(100, 8);
    Sleep(0.3);
    drive->EncoderTurn(25, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(100, 8);
    Sleep(0.3);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    //drive->EncoderTurn(90, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallBackward();

    // Read scoop light
    io->InitializeScoopLight();
    drive->EncoderTurn(90, Drive::RIGHT);
    drive->SquareToWallForward();
    drive->DriveDist(-100, 13.25);
    Sleep(0.3);
    drive->EncoderTurn(-90, Drive::RIGHT);
    drive->SquareToWallBackward();
    drive->DriveDist(100, 9);
    Sleep(0.3);
    io->ReadScoopLight();

    // Drive to chiller and deposit skid
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallBackward();
    drive->DriveDist(100, 10);
    drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    Sleep(0.3);
    drive->SquareToWallBackward();

    // Deposit Scoop if RIGHT counter
    if(io->counter == IO::RIGHT_COUNTER)
    {
        // RIGHT Counter
        box->SetDegree(IO::BOX_DUMP);
        Sleep(0.75);
        box->SetDegree(IO::BOX_STORE);
    }

    arm->SetDegree(IO::ARM_APPROACH_SKID);
    drive->DriveDist(100, 5);
    drive->SetDriveTime(0, -70, 0.2);
    drive->DriveDist(100, 5);
    drive->SetDriveTime(-100, 100, 0.5);
    drive->SquareToWallBackward();
    arm->SetDegree(IO::ARM_STORE);
    drive->SetDriveTime(100, 0, 1.25);
    drive->EncoderTurn(-15, Drive::RIGHT);
    drive->SquareToWallBackward();

    // Drive to in shop
    drive->EncoderTurn(90, Drive::RIGHT);
    drive->SquareToWallForward();

    if(io->counter == IO::LEFT_COUNTER)
    {
        // LEFT Counter
        drive->DriveDist(-100, 7);
        drive->EncoderTurn(-90, Drive::RIGHT);
        drive->SquareToWallBackward();
        box->SetDegree(IO::BOX_DUMP);
        Sleep(0.75);
        box->SetDegree(IO::BOX_STORE);
        drive->EncoderTurn(90, Drive::RIGHT);
        drive->SquareToWallForward();
    }

    // Square up on walls and go up ramp

    drive->DriveDist(-100, 13.25);
    Sleep(0.3);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallBackward();

    // Drive up Ramp
    box->SetDegree(IO::BOX_DUMP);
    drive->DriveDist(100, 34);
    box->SetDegree(IO::BOX_STORE);

    // Use coord pid to drive the robot directly in from of the  switch
    timer->Reset();
    timer->SetTimeout(5.0);
    while(true)
    {
        if(!io->fl_switch->Value() || !io->fr_switch->Value())
        {
            drive->SquareToWallForward();
            break;
        }
        if(io->IsRPSGood())
        {
            io->lcd->Clear(FEHLCD::Black);
            //drive->SetDrive(0, drive->coord_pid->GetOutput(io->rps_x));
            drive->SetDrive(60, (io->X_COORD_FLIP_SWITCH - io->rps_x) * 100.0 / 10.0 + (io->rps_heading - 90) * 100 / 110);
        }
        else
        {
            drive->SetDrive(50, 0);
        }
        if(timer->IsTimeout())
        {
            drive->SetDrive(0, 0);
            drive->SquareToWallForward();
            break;
        }
        Sleep(IO::LOOP_TIMEOUT);
    }

    // Bunch of small turns to flip switch
    drive->SetDriveTime(-100, -100, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(100, -100, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(-100, -100, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(100, -100, 0.25);
    Sleep(0.3);

    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->EncoderTurn(90, Drive::LEFT);
    drive->EncoderTurn(-90, Drive::RIGHT);
    drive->SquareToWallBackward();
    drive->EncoderTurn(-90, Drive::LEFT);

    // Navigate to charge zone

    drive->SquareToWallForward();
    drive->SetDriveTime(100, 0, 0.25);

}

void individual_comp()
{
    // Init
    io->WaitForStartLight();
    arm->SetDegree(IO::ARM_STORE);
    box->SetDegree(IO::BOX_STORE);
    io->num_button_pushes_required = rps->Oven();
    lcd->Write("Buttons: ");
    lcd->Write(io->num_button_pushes_required);

    // Drive to button
    //drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    // Encoder turn because it doesn't have to be precise and it won't fail
    // and ram into the charge zone button like an rps turn might
    drive->EncoderTurn(-90, Drive::RIGHT);

    Sleep(0.3);
    drive->SquareToWallForward();
    drive->DriveDist(-100, 0.5);
    Sleep(0.5);
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(100, 3);

    // Push button multiple times
    drive->PushButton();
    int times_attempted_press = 1;
    while(rps->OvenPressed() < io->num_button_pushes_required && times_attempted_press < 6)
    //for(int i = 1; i < io->num_button_pushes_required; i++)
    {
        drive->PushButton();
        times_attempted_press++;
    }

    // Drive to Switch but do not flip
    drive->DriveDist(-100, 16);
    Sleep(0.3);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallForward();
    io->InitializeLineFollowingPin();

    // Drive to PIN
    drive->DriveDist(-100, 2.5);
    Sleep(0.3);
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    arm->SetDegree(IO::ARM_SENSE_PIN);
    Sleep(0.3);
    drive->DriveDist(100, 1.5);
    drive->TurnToLine();

    // Sense the CHUTE
    drive->LineFollowPin();
    Sleep(0.5);

    // Pull Pin
    arm->SetDegree(IO::ARM_PULL_PIN);
    drive->DriveDist(-100, 6);
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    Sleep(1.0);
    arm->SetDegree(IO::ARM_SENSE_PIN);

    // Drive to skid
    drive->SetDriveTime(0, 50, 0.3);
    drive->LineFollowPin();
    arm->SetDegree(IO::ARM_STORE);
    Sleep(0.5);
    drive->SetDriveTime(100, -100, 0.5);
    drive->TurnToLine();
    //drive->SetDriveTime(100, -100, 0.3);
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    Sleep(1.0);
    drive->LineFollowSkid();

    // Pick up skid
    arm->SetDegree(IO::ARM_PICKUP_SKID);
    Sleep(0.3);
    drive->DriveDist(-100, 2);
    Sleep(1.5);
    drive->SetDriveTime(100, 0, 0.5);
    Sleep(0.5);
    drive->DriveDist(-100, 2);
    Sleep(0.5);
    //drive->SetDriveTime(0, 50, 0.5);

    // Drive down ramp
    drive->SquareToWallBackward();
    drive->DriveDist(100, 6.75);
    Sleep(0.3);
    drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
    //drive->EncoderTurn(-90, Drive::RIGHT);
    Sleep(0.3);
    drive->SetDriveLR(-60, -40);
    Sleep(1.2);
    //drive->DriveDist(100, 15.5);
    drive->DriveDist(100, 8);
    Sleep(0.3);
    drive->EncoderTurn(25, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(100, 8);
    Sleep(0.3);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    //drive->EncoderTurn(90, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallBackward();

    // Read scoop light
    io->InitializeScoopLight();
    drive->DriveDist(100, 9);
    Sleep(1.0);
    io->ReadScoopLight();

    // Drive to chiller and deposit skid
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallBackward();
    drive->DriveDist(100, 10);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
    drive->SquareToWallBackward();

    // Deposit Scoop if RIGHT counter
    if(io->counter == IO::RIGHT_COUNTER)
    {
        // RIGHT Counter
        box->SetDegree(IO::BOX_DUMP);
        Sleep(1.0);
        box->SetDegree(IO::BOX_STORE);
    }

    arm->SetDegree(IO::ARM_APPROACH_SKID);
    drive->DriveDist(100, 5);
    drive->SetDriveTime(0, -70, 0.2);
    drive->DriveDist(100, 5);
    drive->SetDriveTime(-100, 100, 0.5);
    drive->SquareToWallBackward();
    arm->SetDegree(IO::ARM_STORE);
    drive->SetDriveTime(100, 0, 1.25);
    drive->EncoderTurn(-15, Drive::RIGHT);
    drive->SquareToWallBackward();

    // Drive to in shop
    drive->DriveDist(100, 8);
    Sleep(0.5);
    drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
    drive->SquareToWallForward();
    drive->SquareToWallBackward();

    if(io->counter == IO::LEFT_COUNTER)
    {
        // LEFT Counter
        drive->DriveDist(100, 13);
        Sleep(0.5);
        drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
        Sleep(0.5);
        drive->SquareToWallBackward();
        box->SetDegree(IO::BOX_DUMP);
        Sleep(1.0);
        box->SetDegree(IO::BOX_STORE);
        drive->DriveDist(100, 8);
        Sleep(0.5);
        drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
        Sleep(0.5);
        drive->SquareToWallBackward();
    }

    // Square up on walls and go up ramp

    drive->DriveDist(100, 19.0);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallBackward();

    // Drive up Ramp
    drive->DriveDist(100, 34);

    // Use coord pid to drive the robot directly in from of the  switch
    timer->Reset();
    timer->SetTimeout(5.0);
    while(true)
    {
        if(!io->fl_switch->Value() || !io->fr_switch->Value())
        {
            drive->SquareToWallForward();
            break;
        }
        if(io->IsRPSGood())
        {
            io->lcd->Clear(FEHLCD::Black);
            //drive->SetDrive(0, drive->coord_pid->GetOutput(io->rps_x));
            drive->SetDrive(60, (io->X_COORD_FLIP_SWITCH - io->rps_x) * 100.0 / 10.0 + (io->rps_heading - 90) * 100 / 110);
        }
        else
        {
            drive->SetDrive(50, 0);
        }
        if(timer->IsTimeout())
        {
            drive->SetDrive(0, 0);
            drive->SquareToWallForward();
            break;
        }
        Sleep(IO::LOOP_TIMEOUT);
    }

    // Bunch of small turns to flip switch
    drive->SetDriveTime(-100, -100, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(100, -100, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(-100, -100, 0.25);
    Sleep(0.3);
    drive->SetDriveTime(100, -100, 0.25);
    Sleep(0.3);

    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(-100, 7);
    drive->TurnAngle(0, Drive::LEFT, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(100, 8);
    drive->TurnAngle(10, Drive::LEFT, Drive::LEFT);
    Sleep(0.3);

    // Navigate to charge zone

    drive->SquareToWallForward();
    drive->SetDriveTime(100, 0, 0.25);

}

void coord_pid_test()
{
    // Drive down ramp, holding angle
    // Use coord pid to drive the robot down ramp
    while(true)
    {
        if(!io->bl_switch->Value() || !io->br_switch->Value())
        {
            drive->SquareToWallBackward();
            break;
        }
        if(io->IsRPSGood())
        {
            io->lcd->Clear(FEHLCD::Black);
            //drive->SetDrive(0, drive->coord_pid->GetOutput(io->rps_x));
            drive->SetDrive(-60, -(io->X_COORD_DRIVE_RAMP - io->rps_x) * 100.0 / 3.0 + (io->rps_heading - 90) * 100 / 70);
        }
        else
        {
            drive->SetDrive(-50, 0);
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
}

void drive_down_ramp()
{
    // Drive down ramp
    drive->SquareToWallBackward();
    drive->DriveDist(100, 6.5);
    Sleep(0.3);
    //drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
    drive->EncoderTurn(-90, Drive::RIGHT);
    Sleep(0.3);
    drive->SetDriveLR(-60, -40);
    Sleep(1.2);
    //drive->DriveDist(100, 15.5);
    drive->DriveDist(100, 8);
    Sleep(0.3);
    drive->EncoderTurn(25, Drive::LEFT);
    Sleep(0.3);
    drive->DriveDist(100, 8);
    Sleep(0.3);
    //drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    drive->EncoderTurn(90, Drive::LEFT);
    Sleep(0.3);
    drive->SquareToWallBackward();
}

void chiller()
{
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    drive->DriveDist(100, 5);
    drive->SetDriveTime(0, -70, 0.2);
    drive->DriveDist(100, 5);
    drive->SetDriveTime(-100, 100, 0.5);
    drive->SquareToWallBackward();
    Sleep(1.0);
    arm->SetDegree(IO::ARM_STORE);
    Sleep(1.0);
    drive->SetDriveTime(100, 0, 1.75);
    drive->SquareToWallBackward();
}

void pt7()
{
    lcd->WriteLine("PT7");

    // drive from start light to ramp
    /*drive->DriveDist(100, 35);
    Sleep(1.0);
    drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    Sleep(1.0);*/

    // Drive down ramp past scoop light
    box->SetDegree(IO::BOX_STORE);
    drive->SquareToWallBackward();
    Sleep(1.0);

    // In the shop
    drive->TURN_MIN_POWER = Drive::TURN_MIN_POWER_SHOP;

    io->InitializeScoopLight();
    Sleep(1.0);
    drive->DriveDist(100, 9);
    Sleep(1.0);
    io->ReadScoopLight();
    Sleep(1.0);
    if(io->counter == IO::LEFT_COUNTER)
    {
        drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
        Sleep(1.0);
        drive->DriveDist(100, 2.5);
        Sleep(1.0);
        drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
        Sleep(1.0);
        drive->SquareToWallBackward();
        box->SetDegree(IO::BOX_DUMP);
        Sleep(2.0);
        box->SetDegree(IO::BOX_STORE);
    }
    else // RIGHT COUNTER
    {
        drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
        Sleep(1.0);
        drive->DriveDist(-100, 3);
        Sleep(1.0);
        drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
        Sleep(1.0);
        drive->SquareToWallBackward();
        box->SetDegree(IO::BOX_DUMP);
        Sleep(2.0);
        box->SetDegree(IO::BOX_STORE);
    }

    // Square up on side wall and in front of ramp
    drive->DriveDist(100, 7);
    Sleep(1.0);
    drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
    Sleep(1.0);
    drive->SquareToWallBackward();
    Sleep(1.0);
    drive->DriveDist(100, 18.5);
    Sleep(1.0);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(1.0);
    drive->SquareToWallBackward();
    Sleep(1.0);

    // drive back up ramp
    drive->DriveDist(100, 34);
    Sleep(1.0);

    // Turn toward charge zone
    drive->TurnAngle(5, Drive::LEFT, Drive::LEFT);
    Sleep(1.0);

    // Drive to charge zone
    drive->SquareToWallForward();
    // Ram into charge zone pusher
    drive->SetDriveTime(100, 0, 0.25);

}

void pt7_bonus()
{
    arm->SetDegree(IO::ARM_PULL_PIN);
    Sleep(1.0);
    drive->TurnAngle(120, Drive::LEFT, Drive::RIGHT);
    Sleep(1.0);
    drive->TurnAngle(90, Drive::RIGHT, Drive::RIGHT);
    Sleep(1.0);
    arm->SetDegree(IO::ARM_STORE);

    // All the rest
    pt7();
}

void test()
{
    while(true)
    {
        lcd->Clear();
        lcd->WriteLine(io->IsRPSGood());
        lcd->Write("X: ");
        lcd->Write(io->rps->X());
        lcd->Write(", Y: ");
        lcd->Write(io->rps->Y());
        lcd->Write(", Head: ");
        lcd->WriteLine(io->rps->Heading());
        lcd->Write("fl_sw: ");
        lcd->Write(io->fl_switch->Value());
        lcd->Write(",fr_sw: ");
        lcd->WriteLine(io->fr_switch->Value());
        lcd->Write("bl_sw: ");
        lcd->Write(io->bl_switch->Value());
        lcd->Write(",br_sw: ");
        lcd->WriteLine(io->br_switch->Value());
        lcd->Write("arm_sw: ");
        lcd->WriteLine(io->arm_switch->Value());
        lcd->Write("RPS: ");

        lcd->Write("left_encoder: ");
        lcd->WriteLine(io->left_encoder->Counts());
        lcd->Write("Right_encoder: ");
        lcd->WriteLine(io->right_encoder->Counts());
        lcd->Write("cds: ");
        lcd->Write(io->cds_cell->Value());
        lcd->Write(",opto: ");
        lcd->Write(io->optosensor->Value());
        Sleep(IO::PRINT_TIMEOUT);

        io->Update();
        if(io->ButtonBoardGetButton(IO::RIGHT))
        {
            return;
        }
    }
}

void encoderTest()
{
    drive->EncoderTurn(90, Drive::LEFT);
    Sleep(5.0);
    drive->EncoderTurn(-90, Drive::RIGHT);
    /*drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(1.0);
    drive->TurnAngle(90, Drive::LEFT, Drive::LEFT);
    Sleep(5.0);

    drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
    Sleep(1.0);
    drive->TurnAngle(90, Drive::RIGHT, Drive::RIGHT);*/
}

void skid()
{
    arm->SetDegree(IO::ARM_SENSE_PIN);
    io->InitializeLineFollowingPin();
    drive->LineFollowPin();
    arm->SetDegree(IO::ARM_STORE);
    Sleep(0.5);
    drive->SetDriveTime(100, -100, 0.5);
    drive->TurnToLine();
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    Sleep(1.0);
    drive->LineFollowSkid();
    arm->SetDegree(IO::ARM_PICKUP_SKID);
    Sleep(0.3);
    drive->SetDriveTime(-100, 0, 0.5);
}
