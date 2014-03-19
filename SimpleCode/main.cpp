#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHWONKA.h>
#include <FEHServo.h>
#include <FEHMotor.h>

#include "timer.h"
#include "io.h"

// Function prototypes
void RunScript(char* script);
void pt7();
void test();
void comp();

// Variable Declarations
ButtonBoard *button_board;
AnalogInputPin *cds_cell, *optosensor;
DigitalInputPin *right_switch, *left_switch, *arm_switch;
FEHMotor *left, *right;
FEHServo *arm, *box;
FEHEncoder *left_encoder, *right_encoder;
IO *io;
//Drive *drive;
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
    right_switch = new DigitalInputPin(FEHIO::P0_1);
    left_switch = new DigitalInputPin(FEHIO::P0_2);
    arm_switch = new DigitalInputPin(FEHIO::P0_3);
    optosensor = new AnalogInputPin(FEHIO::P0_4);
    left_encoder = new FEHEncoder(FEHIO::P1_0);
    right_encoder = new FEHEncoder(FEHIO::P1_1);
    left = new FEHMotor(FEHMotor::Motor0);
    right = new FEHMotor(FEHMotor::Motor1);
    arm = new FEHServo(FEHServo::Servo0);
    box = new FEHServo(FEHServo::Servo1);
    arm->SetMin(500);
    arm->SetMax(2431);
    box->SetMin(500);
    box->SetMax(2431);
    lcd = &LCD;
    rps = &RPS;
    print_timer = new Timer();

    io = new IO(print_timer, button_board, lcd, rps, left_encoder, right_encoder, left_switch, right_switch, arm_switch, optosensor, cds_cell);

    scripts[0] = "pt7";
    scripts[1] = "test";
    scripts[2] = "comp";
    scripts[3] = "Toggle RPS";
    num_scripts = 4;
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
            lcd->Clear();
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
    if(script == "test"){ test(); }
    if(script == "comp"){ comp(); }
    if(script == "Toggle RPS"){ is_rps_enabled = !is_rps_enabled; }
}

void pt7()
{

}

void test()
{

}

void comp()
{

}
