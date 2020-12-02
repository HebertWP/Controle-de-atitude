#ifndef __StepperMotor_H__
#define __StepperMotor_H__

#include <Arduino.h>
class StepperMotor
{
private:
    uint8_t _step;
    uint8_t _dir;
    uint8_t _en;
    int _num_step;
    bool _on;
    void step();
public:
    StepperMotor(uint8_t step, uint8_t dir, uint8_t en, int steps_per_rotation);
    void init();
    void disable();
    void enable();
    void move(float ang);
};

#endif