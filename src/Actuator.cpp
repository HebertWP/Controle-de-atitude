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
    //ledcSetup(0, 10000, 8);
    //ledcAttachPin(ledPin, ledChannel);

    pinMode(_en, OUTPUT);
    pinMode(_dir1, OUTPUT);
    pinMode(_dir2, OUTPUT);
    this->setPower(0);
};

void Actuator::setPower(float power)
{
    if (power < 0)
    {
        //analogWrite(_en, -(power)*255);
        digitalWrite(_dir1, HIGH);
        digitalWrite(_dir2, LOW);
    }
    else
    {
        //analogWrite(_en, power * 255);
        digitalWrite(_dir1, HIGH);
        digitalWrite(_dir2, LOW);
    }
    _power = power;
}
