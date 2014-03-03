#ifndef ARM_H
#define ARM_H

#include <FEHServo.h>

class Arm
{
public:
    FEHServo *motor;
    void Set(int degree)
    {
        motor->SetDegree(degree);
    }
    Arm(FEHServo *motor_in)
    {
        motor = motor_in;
    }
};

#endif // ARM_H

