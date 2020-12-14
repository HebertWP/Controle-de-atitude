#ifndef __Sensor_H__
#define __Sensor_H__
#include <Arduino.h>

class Sensor
{
public:
    virtual void starRead();

    virtual void reading();

    virtual bool isReadDone();

    virtual float *read();

    virtual void setNumSamples(uint16_t num);

    virtual void setFreq(uint16_t freq);
};

#endif