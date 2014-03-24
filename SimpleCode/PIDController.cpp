#include "PIDController.h"

PIDController::PIDController(float kP_in, float kI_in, float kD_in)
{
    input = 0.0;
    output = 0.0;
    error = 0.0;
    prev_error = 0.0;
    total_error = 0.0;
    setpoint = 0.0;

    kP = kP_in;
    kI = kI_in;
    kD = kD_in;

    timer = new Timer();
}

void PIDController::SetInput(float input_in)
{
    input = input_in;
}

void PIDController::SetSetpoint(float setpoint_in)
{
    setpoint = setpoint_in;
}

float PIDController::GetOutput(float input_in)
{
    float dt = timer->GetTime();
    timer->Reset();

    error = setpoint - input_in;
    total_error += error * dt;

    output = kP * error + kI * total_error + kD * (error - prev_error) / dt;

    prev_error = error;
}
