#include "io.h"

IO::IO(ButtonBoard *button_board_in)
{
    button_board = button_board_in;

    /*for(int i = 0; i < 3; i++)
    {
        button_board_current_states[i] = false;
        button_board_prev_states[i] = false;
    }*/
}

void IO::Update()
{
    // Store all of the current button board states in the previous states array
    /*for(int i = 0; i < 3; i++)
    {
        button_board_prev_states[i] = button_board_current_states[i];
    }

    // Update the button board current states
    button_board_current_states[LEFT] = button_board->LeftPressed();
    button_board_current_states[MIDDLE] = button_board->MiddlePressed();
    button_board_current_states[RIGHT] = button_board->RightPressed();*/
}

bool IO::ButtonBoardGetButton(Button button)
{
    return false;//button_board_current_states[button];
}

bool IO::ButtonBoardGetPressedEvent(Button button)
{
    return false;//button_board_current_states[button] && ! button_board_prev_states[button];
}
