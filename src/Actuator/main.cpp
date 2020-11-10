#include <Arduino.h>
#include "Actuator.h"

Actuator mot1= Actuator(1, 2, 3);
void setup()
{
    mot1.init();
}

void loop()
{
    for(float i = 0; i <256 ; i++){
        mot1.setPower(i);
        delay(100);
    }
}