#if defined(ESP)
#ifndef __Comunication_H__
#define __Comunication_H__

#include <Arduino.h>
#include <ThingerESP32.h>
#include "PassWordFile.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class ComunicationSystem : public ThingerESP32
{
private:
public:
    ComunicationSystem() : ThingerESP32(USERNAME_THINGER, DEVICE_ID_THINGER, DEVICE_CREDENTIAL_THINGER)
    {    
    };
};

#endif
#endif

/*
#if defined(ESP)
#ifndef __Comunication_H__
#define __Comunication_H__
#include <Arduino.h>
#include <ThingerESP32.h>
#include "PassWordFile.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class ComunicationSystem : public ThingerESP32
{
private:
    SemaphoreHandle_t _semaphore;
    static void loop(void *arg)
    {
        delay(10000);
        while (true)
        {
            //if (xSemaphoreTake(((ComunicationSystem *)arg)->_semaphore, (TickType_t)20) == pdTRUE)
            //{
            ((ThingerESP32 *)arg)->handle();
            //    xSemaphoreGive(((ComunicationSystem *)arg)->_semaphore);
            //}
            delay(10);
        }
    }
public:
    ComunicationSystem() : ThingerESP32(USERNAME_THINGER, DEVICE_ID_THINGER, DEVICE_CREDENTIAL_THINGER)
    {
        _semaphore = xSemaphoreCreateBinary();
        if (_semaphore == NULL)
        {
            Serial.println("Erro 0");
            ESP.restart();
        }
        xSemaphoreGive(_semaphore);
    
    };
    void init()
    {
        xTaskCreatePinnedToCore(ComunicationSystem::loop,
                                "Opr.Loop",
                                2048,
                                (void *)this,
                                3,
                                NULL,
                                tskNO_AFFINITY);
    
        delay(20);
    };
};

#endif
#endif
*/