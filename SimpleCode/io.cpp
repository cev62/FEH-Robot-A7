#include "io.h"

IO::IO(ButtonBoard *button_board_in, FEHLCD *lcd_in, FEHWONKA *rps_in, FEHEncoder *left_encoder_in, FEHEncoder *right_encoder_in, DigitalInputPin *left_switch_in, DigitalInputPin *right_switch_in, DigitalInputPin *arm_switch_in, AnalogInputPin *optosensor_in, AnalogInputPin *cds_cell_in)
{
    button_board_current_states = new bool[3];
    button_board_prev_states = new bool[3];

    for(int i = 0; i < 3; i++)
    {
        button_board_current_states[i] = false;
        button_board_prev_states[i] = false;
    }

    button_board = button_board_in;
    rps = rps_in;
    lcd = lcd_in;
    left_encoder = left_encoder_in;
    right_encoder = right_encoder_in;
    left_switch = left_switch_in;
    right_switch = right_switch_in;
    arm_switch = arm_switch_in;
    optosensor = optosensor_in;
    cds_cell = cds_cell_in;
}

void IO::Update()
{
    // Store all of the current button board states in the previous states array
    for(int i = 0; i < 3; i++)
    {
        button_board_prev_states[i] = button_board_current_states[i];
    }

    // Update the button board current states
    button_board_current_states[LEFT] = button_board->LeftPressed();
    button_board_current_states[MIDDLE] = button_board->MiddlePressed();
    button_board_current_states[RIGHT] = button_board->RightPressed();
}

bool IO::ButtonBoardGetButton(Button button)
{
    return button_board_current_states[button];
}

bool IO::ButtonBoardGetPressedEvent(Button button)
{
    return button_board_current_states[button] && ! button_board_prev_states[button];
}

void IO::ResetEncoders()
{
    left_encoder->ResetCounts();
    right_encoder->ResetCounts();
}

// Returns the average distance travelled by the two encoders IN INCHES
float IO::GetDistTravelled()
{
    return (left_encoder->Counts() + right_encoder->Counts()) / 2.0 / COUNTS_PER_INCH;
}

void IO::WaitForStartLight()
{
    float curr_value = cds_cell->Value();
    float initial_value = curr_value;
    while(true)
    {
        curr_value = cds_cell->Value();
        lcd->Write("CdS cell: ");
        lcd->WriteLine(curr_value);
        if(curr_value - initial_value < IO::START_LIGHT_THRESHOLD)
        {
            lcd->Clear();
            return;
        }
        if(button_board->RightPressed())
        {
            lcd->Clear();
            return;
        }
        Sleep(0.100);
        lcd->Clear();
    }
}