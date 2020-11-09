#include <Arduino.h>
#include "Actuator.h"
int a[3]={2,2,3};

void setup()
{
    Actuator::init(3,{2,4},1,{4,2});
}

void loop()
{
}