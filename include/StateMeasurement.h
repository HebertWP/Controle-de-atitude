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
#define v_p 30
#define v_s 1
// model std (1/inertia)
#define m_p 30
#define m_s 1


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

    /**
     * @brief Construct a new State Measurement object
     * 
     * @param sda MPU's SDA pin
     * @param scl MPU's SCL pin
     * @param int_i2c MPU's interuption pin
     * @param eix1p LDR1
     * @param eix1n LDR2
     * @param eix2p LDR3
     * @param eix2n LDR4
     * @param cl Debug in PUTTY
     */
    StateMeasurement(uint8_t sda, uint8_t scl, uint8_t int_i2c, uint8_t eix1p, uint8_t eix1n, uint8_t eix2p, uint8_t eix2n, WiFiClient *cl);

    /**
     * @brief necessary call it before rest 
     * 
     */
    void init();

    /**
     * @brief start MeasurementCycle
     * 
     */
    void startMeasurementCycle();

    /**
     * @brief call it after starMeasurement until measurement is done
     * 
     */
    void inMeasurement();

    /**
     * @brief see if measurement is over
     * 
     * @return true case is over
     * @return false case not, so cal inMeasurement again
     */
    bool isMeasurementDone();

    /**
     * @brief take the control stare measurement
     * 
     * @return State, it's a struc with angular rotation and angular speed 
     */
    State measurement();
};

#endif