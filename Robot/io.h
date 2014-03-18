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
    DigitalInputPin *left_switch, *right_switch, *arm_switch;
    AnalogInputPin *optosensor, *cds_cell;
    bool *button_board_prev_states, *button_board_current_states;
    float COUNTS_PER_INCH;
    IO(ButtonBoard *button_board_in, FEHWONKA *rps_in, FEHEncoder *left_encoder_in, FEHEncoder *right_encoder_in, DigitalInputPin *left_switch_in, DigitalInputPin *right_switch_in, DigitalInputPin *arm_switch_in, AnalogInputPin *optosensor_in, AnalogInputPin *cds_cell_in)
    {
        COUNTS_PER_INCH = (24.0 * 2) / (2.75 * 3.14); // 24.0*2 is because 48 ticks are registered per revolution
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
        left_switch = left_switch_in;
        right_switch = right_switch_in;
        arm_switch = arm_switch_in;
        optosensor = optosensor_in;
        cds_cell = cds_cell_in;
    }
};

#endif // IO_H
