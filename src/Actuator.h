#ifndef __Actuator_H__
#define __Actuator_H__
#include <Arduino.h>
class Actuator
{
private:
    float _power;
    uint8_t _en;
    uint8_t _dir1;
    uint8_t _dir2;
    
public:
    Actuator(uint8_t en, uint8_t dir1, uint8_t dir2);
    void init(); 
    void setPower(float power);
    float getPower();
};

#endif