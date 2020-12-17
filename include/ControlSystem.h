#if defined(ESP32)
#ifndef __ControlSystem_H__
#define __ControlSystem_H__

#include <Arduino.h>
#include "Actuator.h"
#include "StateMeasurement.h"
#include "PID.h"

class ControlSystem
{
private:
    static Actuator *_motor1;
    static StateMeasurement *_measure;
    static PID *_pid;
    static SemaphoreHandle_t _semaphore;
    static bool _stop;
    static TaskHandle_t _xHandle;
    static void loop(void *arg);
    static WiFiClient *_cl;

public:
    static void init(Actuator *motor1, StateMeasurement *measure, PID *pid, WiFiClient *cl = NULL);
    static void turnON();
    static void turnOFF();
};

#endif
#endif