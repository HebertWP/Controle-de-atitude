#include "InertialMesure.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

SemaphoreHandle_t InertialMesure::_semaphore = NULL;
double *InertialMesure::_acceleration = (double *)calloc(3, sizeof(double));
double *InertialMesure::_rotation = (double *)calloc(3, sizeof(double));

void InertialMesure::init(uint8_t sda, uint8_t scl)
{
    InertialMesure::_semaphore = xSemaphoreCreateBinary();
    if (InertialMesure::_semaphore == NULL)
    {
        Serial.println("Erro 0");
    }
    xSemaphoreGive(InertialMesure::_semaphore);
    xTaskCreatePinnedToCore(InertialMesure::loop,
                            "Opr.Loop",
                            2048,
                            NULL,
                            3,
                            NULL,
                            tskNO_AFFINITY);
    delay(100);
};

void InertialMesure::loop(void *arg)
{
    Adafruit_MPU6050 mpu;
    sensors_event_t a, g, temp;
    if (xSemaphoreTake(InertialMesure::_semaphore, (TickType_t)20) == pdTRUE)
    {
        if (!mpu.begin())
            ESP.restart();

        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        xSemaphoreGive(InertialMesure::_semaphore);
    }
    while (1)
    {
        if (xSemaphoreTake(InertialMesure::_semaphore, (TickType_t)20) == pdTRUE)
        {
            mpu.getEvent(&a, &g, &temp);
            _acceleration[0] = a.acceleration.x;
            _acceleration[1] = a.acceleration.y;
            _acceleration[2] = a.acceleration.z;
            _rotation[0] = g.gyro.x;
            _rotation[1] = g.gyro.y;
            _rotation[2] = g.gyro.z;
            xSemaphoreGive(InertialMesure::_semaphore);
        }
        delay(500);
    }
}

double *InertialMesure::getAcceleration()
{
    static double out[3];
    if (xSemaphoreTake(InertialMesure::_semaphore, (TickType_t)20) == pdTRUE)
    {
        for (uint8_t i = 0; i < 3; i++)
            out[i] = _acceleration[i];
        xSemaphoreGive(InertialMesure::_semaphore);
    }
    return out;
};
double *InertialMesure::getRotation(){
    static double out[3];
    if (xSemaphoreTake(InertialMesure::_semaphore, (TickType_t)20) == pdTRUE)
    {
        for (uint8_t i = 0; i < 3; i++)
            out[i] = _rotation[i];
        xSemaphoreGive(InertialMesure::_semaphore);
    }
    return out;
};
