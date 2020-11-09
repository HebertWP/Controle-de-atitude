#include "Actuator.h"
#include <Arduino.h>

Actuator::Actuator(uint8_t en, uint8_t *dir)
{
    this->_dir = (uint8_t *)calloc(2, sizeof(uint8_t));
    this->_en = en;
    this->_dir[0] = dir[0];
    this->_dir[1] = dir[1];
}

void Actuator::init()
{
    //ledcSetup(0, 10000, 8);
    //ledcAttachPin(ledPin, ledChannel);

    pinMode(_en, OUTPUT);
    pinMode(_dir[0], OUTPUT);
    pinMode(_dir[1], OUTPUT);
    this->setPower(0);
};

void Actuator::setPower(float power)
{
    if (power < 0)
    {
        //analogWrite(_en, -(power)*255);
        digitalWrite(_dir[0], HIGH);
        digitalWrite(_dir[1], LOW);
    }
    else
    {
        //analogWrite(_en, power * 255);
        digitalWrite(_dir[1], HIGH);
        digitalWrite(_dir[0], LOW);
    }
    _power = power;
}
