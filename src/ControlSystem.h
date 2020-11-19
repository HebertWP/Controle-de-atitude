#if defined(ESP32)
#ifndef __ControlSystem_H__
#define __ControlSystem_H__

#include <Arduino.h>
#include <Actuator.h>
#include <FreeRTOS/semphr.h>

class ControlSystem
{
private:
    static SemaphoreHandle_t _semaphore;
    static Actuator *_motor1;
    static bool  _stop;
    static double _kp;
    static double _ki;
    static TaskHandle_t _xHandle;
    static void loop(void *arg);
     
public:
    static void init(Actuator * motor1);
    static void turnON();
    static void turnOFF();
};

#endif
#endif