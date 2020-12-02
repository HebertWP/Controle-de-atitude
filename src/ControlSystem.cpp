#include <Arduino.h>
#include <ControlSystem.h>
#include <InertialMesure.h>
#ifdef ESP
SemaphoreHandle_t ControlSystem::_semaphore = NULL;
Actuator *ControlSystem::_motor1 = NULL;
bool ControlSystem::_stop = true;
double ControlSystem::_ki = 1.2;
double ControlSystem::_kp = 3.0;
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
    static bool sat =false;
    while (!ControlSystem::_stop)
    {
        if (xSemaphoreTake(ControlSystem::_semaphore, (TickType_t)20) == pdTRUE)
        {
            now = (InertialMesure::getRotation())[2];
            if(!sat)
                sum += now;
            float power=now * ControlSystem::_kp + sum * ControlSystem::_ki;
            if(-0.5 < power && power <0.5)
                ControlSystem::_motor1->setPower(0);
            else
                ControlSystem::_motor1->setPower(power);
            if( power < -1 || 1 < power)
                sat=true;
            else
                sat=false;
            xSemaphoreGive(ControlSystem::_semaphore);
        }
        delay(300);
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
#endif