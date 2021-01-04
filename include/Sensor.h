#ifndef __Sensor_H__
#define __Sensor_H__
#include <Arduino.h>

/**
 * @brief Interface to sample reding to sensors 
 * 
 */
class Sensor
{
public:

    /**
     * @brief star read samples
     * 
     */
    virtual void starRead();

    /**
     * @brief call it during measure to proper measure the sensor 
     * 
     */
    virtual void reading();

    /**
     * @brief ask if the sample is done
     * 
     * @return true if the measure is over
     * @return false you need call reading() more times 
     */
    virtual bool isReadDone();

    /**
     * @brief get samples array 
     * 
     * @return float* poiter to first measure in time 
     */
    virtual float *read();

    /**
     * @brief Set the Num Samples in measure
     * 
     * @param num number of samples
     */
    virtual void setNumSamples(uint16_t num);

    /**
     * @brief Set the Freq of aquisition(remebe nyquist)
     * 
     * @param freq 
     */
    virtual void setFreq(uint16_t freq);
};

#endif