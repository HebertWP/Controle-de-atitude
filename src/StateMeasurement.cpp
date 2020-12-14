#if defined(ESP)

#include <Arduino.h>
#include "StateMeasurement.h"

StateMeasurement::StateMeasurement(uint8_t sda, uint8_t scl, uint8_t int_i2c, uint8_t eix1p, uint8_t eix1n, uint8_t eix2p, uint8_t eix2n, WiFiClient *cl)
{
    _mpu = new MPU6050(sda, scl, int_i2c, 100, 8);
    _star = new StarSensor(eix1p, eix1n, eix2p, eix2n, 8, 100);
    _Kalman = new KALMAN<Nstate, Nobs>();
    _cl = cl;
}

void StateMeasurement::init()
{
    _mpu->init();
    _time = millis();
    // time evolution matrix
    // measurement matrix
    _Kalman->H = {1.0, 0.0,
                  0.0, 1.0};
    // measurement covariance matrix
    _Kalman->R = {v_p, 0.0,
                  0.0, v_s};
    // model covariance matrix
    _Kalman->Q = {m_p * m_p, 0.0,
                  0.0, m_s * m_s};

    this->starMeasurementCycle();
    while (!this->isMeasurementDone())
        this->inMeasurement();
    _Kalman->x = {_star->mean(), _mpu->mean()};
}

void StateMeasurement::starMeasurementCycle()
{
    try
    {
        _mpu->starRead();
    }
    catch (MPU6050::Exception e)
    {
        Serial.println(e.what());
        _mpu->starRead();
    }

    _star->starRead();
}

void StateMeasurement::inMeasurement()
{
    if (!_mpu->isReadDone())
        _mpu->reading();
    if (!_star->isReadDone())
        _star->reading();
}

bool StateMeasurement::isMeasurementDone()
{
    return (_mpu->isReadDone() && _star->isReadDone());
}

using namespace BLA;

State StateMeasurement::measurement()
{
    State out;
    BLA::Matrix<Nobs> obs; // observation vector
    _star->read();
    _mpu->read();
    _time = millis() - _time;
    _Kalman->F = {1, _time,
                  0, 1};
    obs(0) = _star->mean();
    obs(1) = _mpu->mean();
    //_cl->printf("%f\r\n", _star->mean());

    _Kalman->update(obs);
    out.angular_position = _Kalman->x(0);
    out.angular_speed = _Kalman->x(1);
    return out;
}

#endif