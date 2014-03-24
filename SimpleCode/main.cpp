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
void encoderTest();

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
Timer *print_timer;

int button_presses, script_position, num_scripts;
bool is_rps_enabled;
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

    io = new IO(print_timer, button_board, lcd, rps, left_encoder, right_encoder, fl_switch, fr_switch, bl_switch, br_switch, arm_switch, optosensor, cds_cell);
    drive = new Drive(left, right, io);

    num_scripts = 6;
    scripts = new char*[num_scripts];
    scripts[0] = "comp";
    scripts[1] = "pt7_bonus";
    scripts[2] = "pt7";
    scripts[3] = "test";
    scripts[4] = "encoder test";
    scripts[5] = "Toggle RPS"; //must be last script in array
    script_position = 0;
    is_rps_enabled = true;

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
            lcd->WriteLine(is_rps_enabled ? "Enabled" : "Disabled");
        }
        Sleep(IO::LOOP_TIMEOUT);
    }
    return 0;
}

void RunScript(char *script)
{

    // TODO: Use function pointers :)

    // Conditionally init RPS
    if(is_rps_enabled && script != "Toggle RPS")
    {
        // Sleep to let the middle button become un-pressed
        Sleep(1.0);
        rps->InitializeMenu(); //call the region config menu
        rps->Enable(); //enable the RPS
    }

    if(script == "pt7"){ pt7(); }
    if(script == "pt7_bonus"){ pt7_bonus(); }
    if(script == "test"){ test(); }
    if(script == "comp"){ comp(); }
    if(script == "encoder test") { encoderTest(); }
    if(script == "Toggle RPS"){ is_rps_enabled = !is_rps_enabled; }
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
        lcd->WriteLine(io->IsRPSGood());
        lcd->Write("X: ");
        lcd->Write(io->rps->X());
        lcd->Write(", Y: ");
        lcd->Write(io->rps->Y());
        lcd->Write(", Head: ");
        lcd->WriteLine(io->rps->Heading());
        lcd->Write("left_encoder: ");
        lcd->WriteLine(io->left_encoder->Counts());
        lcd->Write("Right_encoder: ");
        lcd->WriteLine(io->right_encoder->Counts());
        lcd->Write("cds: ");
        lcd->Write(io->cds_cell->Value());
        lcd->Write(",opto: ");
        lcd->Write(io->optosensor->Value());
        Sleep(IO::PRINT_TIMEOUT);
    }
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
    drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
    drive->SquareToWallForward();
    Sleep(0.5);
    drive->DriveDist(-100, 0.5);
    Sleep(0.5);
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);

    // Push button multiple times
    drive->PushButton();
    int times_attempted_press = 1;
    //while(rps->OvenPressed() < io->num_button_pushes_required && times_attempted_press < 6)
    for(int i = 1; i < io->num_button_pushes_required; i++)
    {
        drive->PushButton();
        times_attempted_press++;
    }

    // Drive to Switch
    drive->DriveDist(-100, 15);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();

    // Flip Switch
    /*drive->TurnAngle(120, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();*/
    io->InitializeLineFollowingPin();

    // Drive to PIN
    drive->DriveDist(-100, 2.5);
    Sleep(0.5);
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->DriveDist(100, 3);
    arm->SetDegree(IO::ARM_SENSE_PIN);
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
    Sleep(0.3);
    //drive->DriveDist(100, 0.5);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::LEFT, Drive::LEFT);
    arm->SetDegree(IO::ARM_APPROACH_SKID);
    Sleep(1.0);
    drive->SetDriveTime(70, 0, 1.0);

    // Pick up skid
    arm->SetDegree(IO::ARM_PICKUP_SKID);
    Sleep(0.3);
    drive->DriveDist(-100, 2);
    Sleep(1.5);
    drive->SetDriveTime(100, 0, 0.5);
    Sleep(0.5);
    drive->DriveDist(-100, 2);
    Sleep(0.5);

    // Drive down ramp
    drive->TurnAngle(0, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
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
    drive->DriveDist(100, 9);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::LEFT, Drive::RIGHT);
    Sleep(0.5);
    drive->SquareToWallBackward();

    // Deposit Scoop if RIGHT counter
    if(io->counter == IO::RIGHT_COUNTER)
    {
        // RIGHT Counter
        box->SetDegree(IO::BOX_DUMP);
        Sleep(2.0);
        box->SetDegree(IO::BOX_STORE);
    }

    arm->SetDegree(IO::ARM_APPROACH_SKID);
    drive->DriveDist(100, 10);
    Sleep(1.0);
    drive->DriveDist(-100, 6);
    Sleep(1.0);
    arm->SetDegree(IO::ARM_STORE);
    Sleep(1.0);
    drive->SetDriveTime(100, 0, 1.0);
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
        drive->DriveDist(100, 9);
        Sleep(0.5);
        drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
        Sleep(0.5);
        drive->SquareToWallBackward();
        box->SetDegree(IO::BOX_DUMP);
        Sleep(2.0);
        box->SetDegree(IO::BOX_STORE);
        drive->DriveDist(100, 8);
        Sleep(0.5);
        drive->TurnAngle(0, Drive::LEFT, Drive::RIGHT);
        Sleep(0.5);
        drive->SquareToWallBackward();
    }

    // Square up on walls and go up ramp

    drive->DriveDist(100, 18.5);
    Sleep(0.5);
    drive->TurnAngle(90, Drive::RIGHT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallBackward();

    // Drive up Ramp
    //drive->DriveDist(100, 34);
    arm->SetDegree(IO::ARM_FLIP_SWITCH);
    drive->DriveUntilFL();
    drive->DriveDist(-100, 1);
    drive->TurnAngle(120, Drive::LEFT, Drive::RIGHT);
    drive->DriveDist(-100, 6);
    arm->SetDegree(IO::ARM_STORE);

    // Navigate to charge zone
    Sleep(0.5);
    drive->TurnAngle(5, Drive::LEFT, Drive::LEFT);
    Sleep(0.5);
    drive->SquareToWallForward();
    drive->SetDriveTime(100, 0, 0.25);

}

void encoderTest()
{

    drive->EncoderTurn(90, Drive::LEFT);
    Sleep(5.0);
    drive->EncoderTurn(-90, Drive::LEFT);
    Sleep(1.0);

    drive->EncoderTurn(90, Drive::RIGHT);
    Sleep(5.0);
    drive->EncoderTurn(-90, Drive::RIGHT);
    Sleep(1.0);
    /*drive->EncoderTurn(180, Drive::LEFT);
    Sleep(1.0);
    drive->EncoderTurn(-180, Drive::RIGHT);
    Sleep(1.0);*/
}
