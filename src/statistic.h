#ifndef __Statistic_H__
#define __Statistic_H__
#include <Arduino.h>

class Statisc
{
public:
    typedef struct Data
    {
        float mean;
        float variance;
    } Data;

    virtual Statisc::Data statisticRead();
};

#endif