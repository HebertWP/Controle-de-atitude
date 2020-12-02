#include "Actuator.h"
#include <Arduino.h>

Actuator::Actuator(uint8_t en, uint8_t dir1, uint8_t dir2)
{
    this->_dir1 = dir1;
    this->_dir2 = dir2;
    this->_en = en;
}

void Actuator::init()
{
    ledcSetup(0, 5000, 8);
    ledcAttachPin(_en, 0);
    pinMode(_dir1, OUTPUT);
    pinMode(_dir2, OUTPUT);
    this->setPower(0);
};
//ledcAttachPin(ledPin, ledChannel);

void Actuator::setPower(float power)
{
    if (power < 0)
    {
#if defined(ESP)
        ledcWrite(0, -(power)*255);
#else
        analogWrite(_en, -(power)*255);
#endif
        digitalWrite(_dir1, HIGH);
        digitalWrite(_dir2, LOW);
    }
    else
    {
#if defined(ESP)
        ledcWrite(0, (power)*255);
#else
        analogWrite(_en, (power)*255);
#endif
        digitalWrite(_dir1, LOW);
        digitalWrite(_dir2, HIGH);
    }
    _power = power;
}

float Actuator::getPower()
{
    return _power;
}