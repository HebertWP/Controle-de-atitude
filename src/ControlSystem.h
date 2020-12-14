#if defined(ESP32)
#ifndef __ControlSystem_H__
#define __ControlSystem_H__

#include <Arduino.h>
#include "Actuator.h"
#include "StateMeasurement.h"

class ControlSystem
{
private:
    static SemaphoreHandle_t _semaphore;
    static Actuator *_motor1;
    static StateMeasurement *_measure;
    static bool _stop;
    static double _kp;
    static double _ki;
    static TaskHandle_t _xHandle;
    static void loop(void *arg);
    static WiFiClient *_cl;

public:
    static void init(Actuator *motor1, StateMeasurement *measure, WiFiClient *cl = NULL);
    static void turnON();
    static void turnOFF();
};

#endif
#endif