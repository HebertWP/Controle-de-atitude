#ifndef __StarSensor_H__
#define __StarSensor_H__

#include <Arduino.h>
#include <exception>
#include <Sensor.h>
#include "Statistic.h"

class StarSensor : public Statistic, Sensor
{

private:
    uint8_t _plus_eix_1;
    uint8_t _mine_eix_1;
    uint8_t _plus_eix_2;
    uint8_t _mine_eix_2;
    uint16_t _num_samples;
    uint16_t _freq;
    float *_samples;
    int _i;
    bool _reading;

public:
    StarSensor(uint8_t positive_pin_eix_1,
               uint8_t negative_pin_eix_1,
               uint8_t positive_pin_eix_2,
               uint8_t negative_pin_eix_2,
               uint16_t num_samples = 10,
               uint16_t freq = 500);


    void starRead();

    void reading();

    bool isReadDone();

    float *read();

    float mean();

    float variance();

    void setNumSamples(uint16_t num);

    void setFreq(uint16_t freq);

    void calibrate(uint8_t tx, uint8_t rx);
};

#endif