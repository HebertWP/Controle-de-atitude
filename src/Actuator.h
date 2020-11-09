#ifndef __Actuator_H__
#define __Actuator_H__
#include <Arduino.h>

class Actuator
{
private:
    float _power;
    uint8_t _en;
    uint8_t *_dir;
    
public:
    Actuator(uint8_t en, uint8_t *dir);
    void init(); 
    void setPower(float power);
};

#endif