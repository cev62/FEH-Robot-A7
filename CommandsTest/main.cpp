#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <math.h>

#include "drive.h"
#include "command.h"
#include "printcommand.h"
#include "commands/drivecommand.h"
#include "commands/turnamountcommand.h"
#include "commands/turntoanglecommand.h"
#include "util/script.h"
#include "util/script.cpp" // Needs to be here to eliminate Template definition madness
#include "io.h"
#include <FEHWONKA.h>

const float LOOP_TIMEOUT = 0.010;
const float PRINT_TIMEOUT = 0.100;
const int NUM_SCRIPTS= 4;

// Function Prototypes
void InitScripts();

// Declare Variables
FEHLCD *lcd;
Command *current;
Script<Command> *script; // The current script
Script<Command> **scripts; // Array of pointers to scripts
int script_position;
ButtonBoard *button_board;
IO *io;
Timer *print_timer;
Drive *drive;
FEHMotor *drive_left, *drive_right;
bool is_rps_enabled, has_rps_been_initialized;
FEHWONKA RPS, *rps;

int main(void)
{
    // Initialize Variables
    lcd = &LCD;
    lcd->Clear( FEHLCD::Black );
    lcd->SetFontColor( FEHLCD::White );
    button_board = new ButtonBoard(FEHIO::Bank3);
    print_timer = new Timer();
    scripts = new Script<Command>*[NUM_SCRIPTS];
    drive_left = new FEHMotor(FEHMotor::Motor0);
    drive_right = new FEHMotor(FEHMotor::Motor1);
    drive = new Drive(drive_left, drive_right);
    script_position = 0;
    is_rps_enabled = true;
    has_rps_been_initialized = false;
    rps = &RPS;
    io = new IO(button_board, rps);

    // Main Loop, allows for multiple scripts to be run back to back. Does not stop. Ever.
    // Make sure scripts are re-initialized every iteration
    while(true)
    {

        // Initialize Scripts
        InitScripts();
        script = scripts[script_position];
        Command::SetScript(script);

        // Initialize subsystems in the Command class
        Command::Init(lcd, drive, io);

        // Script chooser
        while(true)
        {
            io->Update();
            if(io->ButtonBoardGetPressedEvent(IO::MIDDLE))
            {
                // Middle button pressed: begin the script that is currently selected
                Command::script = script;
                break;
            }
            if(io->ButtonBoardGetPressedEvent(IO::LEFT))
            {
                // Left button pressed: move the script selector down
                script_position -= 1;
                if(script_position < 0)
                {
                    script_position += NUM_SCRIPTS;
                }
                script = scripts[script_position];
            }
            if(io->ButtonBoardGetPressedEvent(IO::RIGHT))
            {
                // Right button pressed: move the script selector up
                script_position += 1;
                if(script_position > NUM_SCRIPTS - 1)
                {
                    script_position -= NUM_SCRIPTS;
                }
                script = scripts[script_position];
            }
            if(print_timer->GetTime() > PRINT_TIMEOUT)
            {
                // Print status of script choosing
                lcd->Clear();
                lcd->Write("Selected: ");
                lcd->WriteLine(script->name);
                lcd->WriteLine("Side buttons --> choose");
                lcd->WriteLine("Middle button --> run");
                lcd->Write("RPS is: ");
                lcd->WriteLine(is_rps_enabled ? "Enabled" : "Disabled");
                print_timer->Reset();
            }
        } // Script choose loop

        // If the script is the Toggle RPS script, do so
        if(script->name == "Toggle RPS")
        {
            is_rps_enabled = !is_rps_enabled;
            has_rps_been_initialized = false;
        }

        // Conditionally init RPS
        if(is_rps_enabled && (script->name != "Toggle RPS") && !has_rps_been_initialized)
        {
            rps->InitializeMenu(); //call the region config menu
            rps->Enable(); //enable the RPS
        }

        // Scheduler Loop
        while(true)
        {
            if(script->commands_size <= 0)
            {
                lcd->WriteLine("End of Script");
                break;
            }
            if(script->is_new_current_command)
            {
                current = script->GetCurrentCommand();
                current->Init();
            }
            // TODO: Add IO.UserInterrupt to kill whole script
            // TODO: Add IO.UserInterrupt to kill single command
            io->Update();
            current->Run();
            if(print_timer->GetTime() > PRINT_TIMEOUT)
            {
                lcd->Clear();
                lcd->Write("Cmd: ");
                lcd->WriteLine(current->name);
                current->PrintStatus();
                print_timer->Reset();
            }
            if(current->EndCondition())
            {
                current->Finish();
                script->NextCommand();
            }
            else if(current->FailedCondition())
            {
                script->NextCommand();// Take it of the deque first so new failure commands can be up next
                current->Failure();

                // Debugging failure commands in script
                lcd->Clear();
                for(int i = 0; i < script->commands_size; i++)
                {
                    lcd->WriteLine(script->commands[i]->name);
                }
                lcd->WriteLine("Done");
                Sleep(5.0);
            }

            Sleep(LOOP_TIMEOUT);
        } // Scheduler loop

    } // Main loop

}

void InitScripts()
{
    // Initialize all of the scripts
    for(int i = 0; i < NUM_SCRIPTS; i++)
    {
        scripts[i] = new Script<Command>();
    }

    // These are just handle for clarity when adding commands below
    Script<Command> *test = scripts[0];
    Script<Command> *comp = scripts[1];
    Script<Command> *pt6 = scripts[2];
    Script<Command> *toggle_rps = scripts[3];
    // TODO: add test script to enable the user to manually set command parameters and run them

    // Set Script Names
    test->SetName("Test");
    comp->SetName("Competition");
    pt6->SetName("PT 6");
    toggle_rps->SetName("Toggle RPS");


    // *** TEST *** BEGIN //
    test->AddSequential(new PrintCommand("Test 1"));
    test->AddSequential(new PrintCommand("Test 2"));
    test->AddSequential(new PrintCommand("Test 3"));
    test->MergeQueue();
    // *** TEST *** END //



    // *** COMPETTION *** BEGIN //
    comp->AddSequential(new PrintCommand("Comp 1"));
    comp->AddSequential(new PrintCommand("Comp 2"));
    comp->AddSequential(new PrintCommand("Comp 3"));
    comp->MergeQueue();
    // *** COMPETITION *** END //



    // *** PT 6 *** BEGIN //
    pt6->AddSequential(new DriveCommand(100, 0, 2.5));
    pt6->AddSequential(new TurnAmountCommand(-90, Drive::LEFT));
    pt6->AddSequential(new PrintCommand("Pt6 3"));
    pt6->MergeQueue();
    // *** PT 6 *** END //



    // *** Toggle RPS *** BEGIN //
    // This script is a special case handled in the script selector
    // *** Toggle RPS *** END //



    // Merge the queue on all of the scripts
    for(int j = 0; j < NUM_SCRIPTS; j++)
    {
        scripts[j]->MergeQueue();
    }
}

