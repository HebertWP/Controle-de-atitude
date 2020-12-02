#include <Arduino.h>
#include <StepperMotor.h>

StepperMotor::StepperMotor(uint8_t step, uint8_t dir, uint8_t en, int steps_per_rotation)
{
    _on = false;
    _en = en;
    _dir = dir;
    _step = step;
    _num_step = steps_per_rotation;
}

void StepperMotor::init()
{
    pinMode(_en, OUTPUT);
    pinMode(_step, OUTPUT);
    pinMode(_dir, OUTPUT);
    this->disable();
}

void StepperMotor::disable()
{
    _on = false;
    digitalWrite(_en, HIGH);
    delay(1);
}

void StepperMotor::enable()
{
    _on = true;
    digitalWrite(_en, LOW);
    delay(1);
}

void StepperMotor::move(float ang)
{
    if (ang > 0)
        digitalWrite(_dir, HIGH);
    else
    {
        digitalWrite(_dir, LOW);
        ang = -ang;
    }
    for (int step = 0; step < _num_step * ang / 360; step++)
        this->step();
}

void StepperMotor::step()
{
    digitalWrite(_step, HIGH);
    delayMicroseconds(140);
    digitalWrite(_step, LOW);
    delayMicroseconds(600);
}