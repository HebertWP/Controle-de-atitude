#if defined(ESP)
#ifndef __StateMeasurement_H__
#define __StateMeasurement_H__

#include <Arduino.h>
#include "MPU6050.h"
#include "StartSensor.h"
#include <Kalman.h>
#include <WiFi.h>

#define Nstate 2 // position, speed
#define Nobs 2   // position, speed

// measurement std
#define v_p 45
#define v_s 5
// model std (1/inertia)
#define m_p 45
#define m_s 5


typedef struct
{
    float angular_position;
    float angular_speed;
} State;

class StateMeasurement
{
private:
    MPU6050 *_mpu;
    StarSensor *_star;
    
    KALMAN<Nstate, Nobs> *_Kalman;
    WiFiClient * _cl;
    unsigned long _time;
public:
    StateMeasurement(uint8_t sda, uint8_t scl, uint8_t int_i2c, uint8_t eix1p, uint8_t eix1n, uint8_t eix2p, uint8_t eix2n, WiFiClient *cl);

    void init();

    void starMeasurementCycle();

    void inMeasurement();

    bool isMeasurementDone();

    State measurement();
};

#endif
#endif