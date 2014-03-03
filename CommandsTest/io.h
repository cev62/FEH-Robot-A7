#ifndef IO_H
#define IO_H

#include <FEHIO.h>
#include <FEHWONKA.h>

class IO
{
public:
    typedef enum
    {
        LEFT = 0,
        MIDDLE,
        RIGHT
    } Button;
    void Update();
    bool ButtonBoardGetPressedEvent(Button button);
    bool ButtonBoardGetButton(Button button);
    float GetDistTravelled();
    void ResetEncoders();

    FEHWONKA *rps;
    ButtonBoard *button_board;
    FEHEncoder *left_encoder, *right_encoder;
    bool *button_board_prev_states, *button_board_current_states;
    float COUNTS_PER_INCH;
    IO(ButtonBoard *button_board_in, FEHWONKA *rps_in, FEHEncoder *left_encoder_in, FEHEncoder *right_encoder_in)
    {
        COUNTS_PER_INCH = 24.0 / (2.75 * 3.14);
        button_board = button_board_in;

        button_board_current_states = new bool[3];
        button_board_prev_states = new bool[3];

        for(int i = 0; i < 3; i++)
        {
            button_board_current_states[i] = false;
            button_board_prev_states[i] = false;
        }

        rps = rps_in;
        left_encoder = left_encoder_in;
        right_encoder = right_encoder_in;
    }
};

#endif // IO_H
