#include <Arduino.h>
#include <ControlSystem.h>
#include <InertialMesure.h>

SemaphoreHandle_t ControlSystem::_semaphore = NULL;
Actuator *ControlSystem::_motor1 = NULL;
bool ControlSystem::_stop = true;
double ControlSystem::_ki = -1.8;
double ControlSystem::_kp = -4;
TaskHandle_t ControlSystem::_xHandle = NULL;

void ControlSystem::init(Actuator *motor1)
{

    ControlSystem::_stop = true;
    ControlSystem::_motor1 = motor1;
    ControlSystem::_semaphore = xSemaphoreCreateBinary();
    if (ControlSystem::_semaphore == NULL)
    {
        Serial.println("Erro 0");
    }
    xSemaphoreGive(ControlSystem::_semaphore);
}

void ControlSystem::loop(void *arg)
{
    double now = 0;
    double sum = 0;

    while (!ControlSystem::_stop)
    {
        if (xSemaphoreTake(ControlSystem::_semaphore, (TickType_t)20) == pdTRUE)
        {
            now = (InertialMesure::getRotation())[2];
            sum += now;
            ControlSystem::_motor1->setPower(now * ControlSystem::_kp + sum * ControlSystem::_ki);

            xSemaphoreGive(ControlSystem::_semaphore);
        }
        delay(500);
    }
    while (1);
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
                "Opr.Loop",
                2048,
                NULL,
                3,
                &ControlSystem::_xHandle);
}