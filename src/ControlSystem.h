#if defined(ESP)
#ifndef __ControlSystem_H__
#define __ControlSystem_H__

#include <Arduino.h>
#include <Actuator.h>
#include <FreeRTOS/semphr.h>

class ControlSystem
{
private:
    static SemaphoreHandle_t _semaphore;
    static void loop(void *arg);
    static Actuator* _motor1; 
    static Actuator* _motor2;
    static float _ref1;
    static float _ref2;
public:
    static void init(Actuator* motor1, Actuator* motor2);
    static void setref(uint8_t eix, float ref);
};

#endif
#endif