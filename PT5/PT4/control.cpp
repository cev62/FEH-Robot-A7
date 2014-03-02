#include "control.h"

Control::Control(FEHLCD *lcd_in, ButtonBoard *button_board_in, AnalogInputPin *cds_cell_in)
{
    lcd = lcd_in;
    button_board = button_board_in;
    cds_cell = cds_cell_in;
    //timer = new Timer();
    //script = 0; // 0=COMP, 1=PT1, etc.
}

/*void Control::Wait(float time)
{
    wait_time = time;
    timer->Reset();
    while(true)
    {
        Sleep(0.020);
        if(timer->GetTime() > wait_time)
        {
            return;
        }
    }
}*/

void Control::WaitForMiddleButton()
{
    lcd->Write("Wait (middle button)...");
    while(true)
    {
        if(button_board->MiddlePressed())
        {
            lcd->WriteLine("OK");
            lcd->Clear();
            return;
        }
        Sleep(0.020);
    }
}


void Control::WaitForStartLight()
{
    float curr_value = cds_cell->Value();
    float initial_value = curr_value;
    while(true)
    {
        curr_value = cds_cell->Value();
        lcd->Write("CdS cell: ");
        lcd->WriteLine(curr_value);
        if(curr_value - initial_value < -0.375)
        {
            lcd->WriteLine("START");
            return;
        }
        if(button_board->RightPressed())
        {
            lcd->WriteLine("Manual Override");
            return;
        }
        Sleep(0.100);
        lcd->Clear();
    }
}
