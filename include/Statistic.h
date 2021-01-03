#ifndef __Statistic_H__
#define __Statistic_H__
#include <Arduino.h>

/**
 * @brief Interface that request to implement statistic to some sensor 
 * 
 */
class Statistic
{
public:

    /**
     * @brief get mean last sample recored by this sensor 
     * 
     * @return float return the mean of measurement 
     */
    virtual float mean();

    /**
     * @brief variance of last measurement set 
     * 
     * @return float return de variance os measurement 
     */
    virtual float variance();
};

#endif