#ifndef __ControlSystem_H__
#define __ControlSystem_H__

#include <Arduino.h>
#include "Actuator.h"
#include "StateMeasurement.h"
#include "PID.h"

/**
 * @brief By now this class just control one axis 
 * 
 */
class ControlSystem
{
private:
    static Actuator *_motor1;
    static StateMeasurement *_measure;
    static PID *_pid1;
    static PID *_pid2;
    static SemaphoreHandle_t _semaphore;
    static bool _stop;
    static TaskHandle_t _xHandle;
    static void loop(void *arg);
    static WiFiClient *_cl;

public:

    /**
     * @brief Need to be called before usage 
     * 
     * @param motor1 motor that control the eix 
     * @param measure state measure  
     * @param pid pid class, that had ben configured  
     * @param cl for debug 
     */
    static void init(Actuator *motor1, StateMeasurement *measure, PID *pid1, PID *pid2, WiFiClient *cl = NULL);

    /**
     * @brief turn ON attitude control
     * 
     */
    static void turnON();

    /**
     * @brief turn OFF attitude control 
     * 
     */
    static void turnOFF();
};

#endif