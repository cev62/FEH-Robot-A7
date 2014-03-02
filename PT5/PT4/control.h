#ifndef CONTROL_H
#define CONTROL_H

#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHUtility.h>
#include "drive.h"

class Control
{
public:
    Control(FEHLCD *lcd_in, ButtonBoard *button_board_in, AnalogInputPin *cds_cell_in);
    void WaitForMiddleButton();
    void WaitForStartLight();
//private:
    ButtonBoard *button_board;
    FEHLCD *lcd;
    AnalogInputPin *cds_cell;
    Drive *drive;
    //Timer *timer, *print_timer;
    //float wait_time;
    //int script;
};



#endif // CONTROL_H
