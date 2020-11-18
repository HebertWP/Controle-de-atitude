#if defined(ESP)
#ifndef __InertialMesure_H__
#define __InertialMesure_H__

#include <Arduino.h>
#include <FreeRTOS/semphr.h>

class InertialMesure
{
private:
    static SemaphoreHandle_t _semaphore;
    static double *_acceleration;
    static double *_rotation;
    static void loop(void *arg);
    
public:
    static void init(uint8_t sda = -1, uint8_t scl = -1);
    static double *getAcceleration();
    static double *getRotation();

};

#endif
#endif