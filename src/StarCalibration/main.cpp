#include <Arduino.h>
#include "StepperMotor.h"

#define dir 8
#define step 9
#define en 7
#define num_step 3200
StepperMotor motor(step, dir, en, num_step);

void setup()
{
    Serial.begin(115200);
    motor.init();
    motor.enable();
}

void loop()
{
    unsigned long time = 0;
    if (Serial.available() > 0)
    {
        char c = Serial.read();
        switch (c)
        {
        case 'L':
            Serial.print("In loop\r\n");
            for (int i = 0; i < num_step / 8; i++)
            {
                time = millis();
                motor.move(0.9);
                while (millis() - time < 1000)
                    ;
            }
            break;
        case 'E':
            Serial.print("Free rotation\r\n");
            motor.enable();
            break;
        case 'D':
            Serial.print("Stoped\r\n");
            motor.disable();
            break;
        case '+':
            Serial.print("Move +0.9º\r\n");
            motor.move(0.9);
            break;
        case '-':
            Serial.print("Move -0.9º\r\n");
            motor.move(-0.9);
            break;
        default:
            break;
        }
    }
}