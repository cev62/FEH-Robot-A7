#include "io.h"

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
