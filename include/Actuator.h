#ifndef __Actuator_H__
#define __Actuator_H__
#include <Arduino.h>
/**
 * @brief Class control responsible to control H bridge 
 * 
 */
class Actuator
{
private:
    float _power;
    uint8_t _en;
    uint8_t _dir1;
    uint8_t _dir2;
    
public:
    
    /**
     * @brief Construct a new Actuator object 
     * 
     * @param en Enable Pin of H bridge, it's use PWM signal 
     * @param dir1 Direction Pin 1 
     * @param dir2 Direction Pin 2 
     */
    Actuator(uint8_t en, uint8_t dir1, uint8_t dir2);

    /**
     * @brief Need to be caled before use the rest of metods 
     * 
     */
    void init();

    /**
     * @brief Set the Power object
     * 
     * @param power a number between -1 and 1, that represent -100% to 100%  
     */
    void setPower(float power);

    /**
     * @brief Get the Power object
     * 
     * @return float return the current power state, number between -1 and 1
     */
    float getPower();
};

#endif