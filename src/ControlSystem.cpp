#include <Arduino.h>
#include "ControlSystem.h"

Actuator *ControlSystem::_motor1 = NULL;
StateMeasurement *ControlSystem::_measure = NULL;
PID *ControlSystem::_pid1 = NULL;
PID *ControlSystem::_pid2 = NULL;
SemaphoreHandle_t ControlSystem::_semaphore = NULL;
WiFiClient *ControlSystem::_cl = NULL;

bool ControlSystem::_stop = true;
TaskHandle_t ControlSystem::_xHandle = NULL;

void ControlSystem::init(Actuator *motor1, StateMeasurement *measure, PID *pid1, PID *pid2, WiFiClient *cl)
{
    ControlSystem::_pid1 = pid1;
    ControlSystem::_pid2 = pid2;
    ControlSystem::_cl = cl;
    ControlSystem::_stop = true;
    ControlSystem::_motor1 = motor1;
    ControlSystem::_motor1->init();
    ControlSystem::_measure = measure;
    ControlSystem::_measure->init();
    ControlSystem::_semaphore = xSemaphoreCreateBinary();
    if (ControlSystem::_semaphore == NULL)
    {
        Serial.println("Erro 0");
    }
    xSemaphoreGive(ControlSystem::_semaphore);
}

void ControlSystem::loop(void *arg)
{
    State now;
    float power = 0;
    while (!ControlSystem::_stop)
    {
        if (xSemaphoreTake(ControlSystem::_semaphore, (TickType_t)20) == pdTRUE)
        {
            ControlSystem::_measure->startMeasurementCycle();
            while (!ControlSystem::_measure->isMeasurementDone())
                ControlSystem::_measure->inMeasurement();
            now = ControlSystem::_measure->measurement();
            //power = ControlSystem::_pid2->UpdateData(now.angular_speed, ControlSystem::_cl);
            power = ControlSystem::_pid1->UpdateData(now.angular_position, ControlSystem::_cl);
            ControlSystem::_motor1->setPower(power);
            if (ControlSystem::_cl != NULL && ControlSystem::_cl->connected())
            {
                ControlSystem::_cl->printf("Angular Position: %f; Velocidade angular: %f;\r\n", now.angular_position, now.angular_speed);
                ControlSystem::_cl->printf("Power: %f %;\r\n", ControlSystem::_motor1->getPower());
                ControlSystem::_cl->printf("Ref: %f%;\r\n", ControlSystem::_pid1->getSetValue());
                ControlSystem::_cl->printf("------------------------\r\n");
            }
            xSemaphoreGive(ControlSystem::_semaphore);
        }
        delay(333);
    }
}

void ControlSystem::turnOFF()
{
    if (ControlSystem::_stop == true)
        return;
    bool end = false;
    while (!end)
    {
        if (uxSemaphoreGetCount(ControlSystem::_semaphore) == 0)
        {
            ControlSystem::_stop = true;
            end = true;
        }
    }
    delay(600);
    ControlSystem::_motor1->setPower(0);
    ControlSystem::_pid1->reset();
    ControlSystem::_pid2->reset();
    vTaskDelete(ControlSystem::_xHandle);
}

void ControlSystem::turnON()
{
    if (ControlSystem::_stop == false)
        return;
    ControlSystem::_stop = false;
    xTaskCreate(ControlSystem::loop,
                "Control Loop",
                2048,
                NULL,
                3,
                &ControlSystem::_xHandle);
}