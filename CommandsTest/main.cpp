#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <math.h>

#include "drive.h"
#include "command.h"
#include "printcommand.h"
#include "util/script.h"
#include "util/script.cpp" // Needs to be here to eliminate Template definition madness
#include "io.h"

const float LOOP_TIMEOUT = 0.010;
const float PRINT_TIMEOUT = 0.100;
const int NUM_SCRIPTS= 3;

// Function Prototypes
void InitScripts();

// Construct Variables
FEHLCD *lcd;
Command *current;
Script<Command> script; // The current script
Script **scripts; // Array of pointers to scripts
int script_position;
ButtonBoard *button_board;
IO *io;
Timer *print_timer;
Drive *drive;
FEHMotor *drive_left, *drive_right;

int main(void)
{
    // Initialize Variables
    lcd = &LCD;
    lcd->Clear( FEHLCD::Black );
    lcd->SetFontColor( FEHLCD::White );
    button_board = new ButtonBoard(FEHIO::Bank3);
    io = new IO(button_board);
    print_timer = new Timer();
    scripts = new Script*[NUM_SCRIPTS];
    drive_left = new FEHMotor(FEHMotor::Motor0);
    drive_right = new FEHMotor(FEHMotor::Motor1);
    drive = new Drive(drive_left, drive_right);

    // Main Loop, allows for multiple scripts to be run back to back. Does not stop. Ever.
    // Make sure scripts are re-initialized every iteration
    while(true)
    {

        // Initialize Scripts
        InitScripts();
        script_position = 0;
        script = scripts[script_position];
        Command::SetScript(script);

        // Initialize subsystems in the Command class
        Command::Init(lcd);

        // Script chooser
        while(true)
        {
            io->Update();
            if(io->ButtonBoardGetPressedEvent(IO::MIDDLE))
            {
                // Middle button pressed: begin the script that is currently selected
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
                lcd->Write("Selected: ");
                lcd->WriteLine(script->name);
                lcd->WriteLine("Side buttons --> choose");
                lcd->WriteLine("Middle button --> run");
                print_timer->Reset();
            }
        } // Script choose loop

        // Scheduler Loop
        while(true)
        {
            if(script->commands_size == 0)
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
    Script *test = scripts[0];
    Script *comp = scripts[0];
    Script *pt6 = scripts[0];



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
    pt6->AddSequential(new PrintCommand("Pt6 1"));
    pt6->AddSequential(new PrintCommand("Pt6 2"));
    pt6->AddSequential(new PrintCommand("Pt6 3"));
    pt6->MergeQueue();
    // *** PT 6 *** END //



    // Merge the queue on all of the scripts
    for(int j = 0; j < NUM_SCRIPTS; j++)
    {
        scripts[j]->MergeQueue();
    }
}

