#ifdef ESP32
#include <Arduino.h>
#include "ControlSystem.h"

SemaphoreHandle_t ControlSystem::_semaphore = NULL;
Actuator *ControlSystem::_motor1 = NULL;
StateMeasurement *ControlSystem::_measure = NULL;
WiFiClient *ControlSystem::_cl = NULL;

bool ControlSystem::_stop = true;
double ControlSystem::_ki = 0;
double ControlSystem::_kp = 0.5;
TaskHandle_t ControlSystem::_xHandle = NULL;

void ControlSystem::init(Actuator *motor1, StateMeasurement *measure, WiFiClient *cl)
{
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
    double sum = 0;
    static bool sat = false;
    while (!ControlSystem::_stop)
    {
        if (xSemaphoreTake(ControlSystem::_semaphore, (TickType_t)20) == pdTRUE)
        {
            ControlSystem::_measure->starMeasurementCycle();
            while (!ControlSystem::_measure->isMeasurementDone())
                ControlSystem::_measure->inMeasurement();
            now = ControlSystem::_measure->measurement();
            ControlSystem::_cl->printf("Angular Position: %f; Velocidade angular: %f\r\n", now.angular_position, now.angular_speed);
            
            if (!sat)
                sum += now.angular_position;
            float power=now.angular_position * ControlSystem::_kp + sum * ControlSystem::_ki;
            if (-0.5 < power && power < 0.5)
                ControlSystem::_motor1->setPower(0);
            else
                ControlSystem::_motor1->setPower(power);
            if (power < -1 || 1 < power)
                sat = true;
            else
                sat = false;
            xSemaphoreGive(ControlSystem::_semaphore);
        }
        delay(1000);
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
#endif