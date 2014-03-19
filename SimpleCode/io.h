#ifndef IO_H
#define IO_H

#include <FEHIO.h>
#include <FEHWONKA.h>
#include <FEHLCD.h>
#include <FEHUtility.h>

#include "timer.h"

class IO
{
public:

    static const float COUNTS_PER_INCH = (24.0 * 2) / (2.75 * 3.14); // 24.0*2 is because 48 ticks are registered per revolution
    static const float START_LIGHT_THRESHOLD = -0.375;
    static const float SCOOP_LIGHT_THRESHOLD = 1.0;

    static const float PRINT_TIMEOUT = 0.100;
    static const float LOOP_TIMEOUT = 0.010;

    static const int ARM_STORE = 45;
    static const int ARM_APPROACH_SKID = 172;
    static const int ARM_PICKUP_SKID = 45;
    static const int ARM_SENSE_PIN = 120;
    static const int ARM_PULL_PIN = 90;

    static const int BOX_STORE = 30;
    static const int BOX_DUMP = 150;

    typedef enum
    {
        LEFT = 0,
        MIDDLE,
        RIGHT
    } Button;
    typedef enum
    {
        LEFT_COUNTER = 0,
        RIGHT_COUNTER
    } Counter;
    void Update();
    bool ButtonBoardGetPressedEvent(Button button);
    bool ButtonBoardGetButton(Button button);
    float GetDistTravelled();
    void ResetEncoders();
    void WaitForStartLight();
    void InitializeScoopLight();
    void ReadScoopLight();

    float cds_cell_shop_floor, cds_cell_scoop_light;
    Counter counter;

    FEHWONKA *rps;
    FEHLCD *lcd;
    ButtonBoard *button_board;
    FEHEncoder *left_encoder, *right_encoder;
    DigitalInputPin *fl_switch, *fr_switch, *bl_switch, *br_switch, *arm_switch;
    AnalogInputPin *optosensor, *cds_cell;
    bool *button_board_prev_states, *button_board_current_states;
    Timer *print_timer;

    IO(Timer *print_timer_in, ButtonBoard *button_board_in, FEHLCD *lcd_in, FEHWONKA *rps_in, FEHEncoder *left_encoder_in, FEHEncoder *right_encoder_in, DigitalInputPin *fl_switch_in, DigitalInputPin *fr_switch_in, DigitalInputPin *bl_switch_in, DigitalInputPin *br_switch_in, DigitalInputPin *arm_switch_in, AnalogInputPin *optosensor_in, AnalogInputPin *cds_cell_in);
};

#endif // IO_H
