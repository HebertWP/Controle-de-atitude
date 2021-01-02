#include "Actuator.h"
#include <Arduino.h>
#ifdef ESP
Actuator::Actuator(uint8_t en, uint8_t dir1, uint8_t dir2)
{
    this->_dir1 = dir1;
    this->_dir2 = dir2;
    this->_en = en;
}

void Actuator::init()
{
    ledcSetup(0, 30, 8);
    ledcAttachPin(_en, 0);
    pinMode(_dir1, OUTPUT);
    pinMode(_dir2, OUTPUT);
    this->setPower(0);
};
//ledcAttachPin(ledPin, ledChannel);

void Actuator::setPower(float power)
{
    if (power < -1)
        power = -1;
    else if (power > 1)
        power = 1;
    if (power < 0)
    {
        ledcWrite(0, -(power)*255);
        digitalWrite(_dir1, HIGH);
        digitalWrite(_dir2, LOW);
    }
    else
    {
        ledcWrite(0, (power)*255);
        digitalWrite(_dir1, LOW);
        digitalWrite(_dir2, HIGH);
    }
    _power = power;
}

float Actuator::getPower()
{
    return _power;
}
#endif