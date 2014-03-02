#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <math.h>

#include "command.h"
#include "printcommand.h"
#include "util/script.h"
#include "util/script.cpp" // Needs to be here to eliminate Template definition madness

const float LOOP_TIMEOUT = 0.010;

// Construct Variables
FEHLCD *lcd;
Command *current;
Script<Command> script;

int main(void)
{
    // Construct Variables
    lcd = &LCD;

    // Initialize subsystems in the Command class
    Command::Init(lcd);
    script = new Script<Command>("Test");
    lcd->Clear( FEHLCD::Black );
    lcd->SetFontColor( FEHLCD::White );

    // Add commands to the script
    script->AddSequential(new PrintCommand("Test 1"));
    script->AddSequential(new PrintCommand("Test 2"));
    script->AddSequential(new PrintCommand("Test 3"));
    script->MergeQueue();

    // Init Scheduler
    current = script->GetCurrentCommand();
    current->Init();

    // Scheduler Loop
    while(true)
    {
        current = script->GetCurrentCommand();
        current->Run();
        current->PrintStatus();
        if(current->EndCondition())
        {
            current->Finish();
            script->NextCommand();
            if(script->commands_size == 0){ break; }
            current = script->GetCurrentCommand();
            current->Init();
        }
        if(current->FailedCondition())
        {
            script->NextCommand();// Take it of the deque so new failure commands can be up next
            current->Failure();
            if(script->commands_size == 0){ break; }
            current = script->GetCurrentCommand();
            current->Init();
        }

        Sleep(LOOP_TIMEOUT);
    }

}

