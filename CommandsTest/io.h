#ifndef IO_H
#define IO_H

#include <FEHIO.h>

typdef enum
{
    LEFT = 0,
    MIDDLE,
    RIGHT
} Button;

class IO
{
public:
    IO(ButtonBoard *button_board_in);
    void Update();
    bool ButtonBoardGetPressedEvent(Button button);
    bool ButtonBoardGetButton(Button button);

private:
    ButtonBoard *button_board;
    bool *button_board_prev_states, *button_board_current_states;
};

#endif // IO_H
