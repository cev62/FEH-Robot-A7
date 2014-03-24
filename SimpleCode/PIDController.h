#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include "timer.h"

class PIDController
{
public:
    PIDController(float kP_in, float kI_in, float kD_in);
    void SetInput(float input_in);
    void SetSetpoint(float setpoint_in);
    float GetOutput(float input_in);
private:
    float input, output, error, setpoint, total_error, prev_error;
    float kP, kI, kD;
    Timer *timer;
};

#endif // PIDCONTROLLER_H
