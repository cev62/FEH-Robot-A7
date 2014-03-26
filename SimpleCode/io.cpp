#include "io.h"

IO::IO(Timer *print_timer_in, ButtonBoard *button_board_in, FEHLCD *lcd_in, FEHWONKA *rps_in, FEHEncoder *left_encoder_in, FEHEncoder *right_encoder_in, DigitalInputPin *fl_switch_in, DigitalInputPin *fr_switch_in, DigitalInputPin *bl_switch_in, DigitalInputPin *br_switch_in, DigitalInputPin *arm_switch_in, AnalogInputPin *optosensor_in, AnalogInputPin *cds_cell_in)
{
    button_board_current_states = new bool[3];
    button_board_prev_states = new bool[3];
    num_button_pushes_required = 1;

    IO::X_COORD_DRIVE_RAMP = 4.0;

    optosensor_curr_value = 0.0;

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
    fl_switch = fl_switch_in;
    fr_switch = fr_switch_in;
    bl_switch = bl_switch_in;
    br_switch = br_switch_in;
    arm_switch = arm_switch_in;
    optosensor = optosensor_in;
    cds_cell = cds_cell_in;
    print_timer = print_timer_in;

    print_timer->SetTimeout(IO::PRINT_TIMEOUT);
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

// Returns the average distance travelled by the left encoder IN INCHES
float IO::GetDistTravelledLeft()
{
    return left_encoder->Counts() / COUNTS_PER_INCH;
}

// Returns the average distance travelled by the right encoder IN INCHES
float IO::GetDistTravelledRight()
{
    return right_encoder->Counts() / COUNTS_PER_INCH;
}

void IO::WaitForStartLight()
{
    float curr_value = cds_cell->Value();
    float initial_value = curr_value;
    while(true)
    {
        curr_value = cds_cell->Value();
        if(print_timer->IsTimeout())
        {
            lcd->Clear();
            lcd->Write("CdS cell: ");
            lcd->WriteLine(curr_value);
        }
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
        Sleep(IO::LOOP_TIMEOUT);
    }
}

void IO::InitializeScoopLight()
{
    cds_cell_shop_floor = cds_cell->Value();
}

void IO::ReadScoopLight()
{
    cds_cell_scoop_light = cds_cell->Value();
    if(cds_cell_shop_floor - IO::SCOOP_LIGHT_THRESHOLD > cds_cell_scoop_light)
    {
        counter = IO::RIGHT_COUNTER;
        lcd->Clear(FEHLCD::Red);
        lcd->WriteLine("RIGHT COUNTER");
        lcd->WriteLine(cds_cell_shop_floor);
        lcd->WriteLine(cds_cell_scoop_light);
    }
    else
    {
        counter = IO::LEFT_COUNTER;
        lcd->Clear(FEHLCD::Blue);
        lcd->WriteLine("LEFT COUNTER");
        lcd->WriteLine(cds_cell_shop_floor);
        lcd->WriteLine(cds_cell_scoop_light);
    }

}

void IO::InitializeLineFollowingPin()
{
    optosensor_factory_floor = optosensor->Value();
}

bool IO::IsOnLinePin()
{
    optosensor_curr_value = optosensor->Value();
    return optosensor_curr_value < optosensor_factory_floor - IO::LINE_THRESHOLD_FACTORY;
}

bool IO::IsRPSGood()
{
    float rps_x_new = rps->X();
    float rps_y_new = rps->Y();
    float rps_heading_new = rps->Heading();
    if(rps_x_new == 0.0 && rps_y_new == 0.0 && rps_heading_new == 0.0)
    {
        lcd->Clear(FEHLCD::Green);
        return false;
    }
    else
    {
        rps_x = rps_x_new;
        rps_y = rps_y_new;
        rps_heading = rps_heading_new;
        return true;
    }
}
