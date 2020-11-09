#if defined(ESP)
#ifndef __ComunicationSystem_H__
#define __ComunicationSystem_H__

#include <Arduino.h>
#include <Actuator.h>
#include <FreeRTOS/semphr.h>

class ComunicationSystem
{
private:
    static SemaphoreHandle_t _semaphore;
    static void loop(void *arg);
public:
    static void init();
};

#endif
#endif