#ifndef __Statistic_H__
#define __Statistic_H__
#include <Arduino.h>

class Statistic
{
public:
    virtual float mean();
    virtual float variance();
    
};

#endif