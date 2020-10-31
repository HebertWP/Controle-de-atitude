// Basic demo for accelerometer readings from Adafruit MPU6050
#include <Arduino.h>
#include "InertialMesure.h"

void setup(void)
{
    Serial.begin(115200);
    InertialMesure::init();
}

void loop()
{
    double *a=InertialMesure::getAcceleration();
    double *r=InertialMesure::getRotation();
    
    Serial.print("Acceleration X: ");
    Serial.print(a[0]);
    Serial.print(", Y: ");
    Serial.print(a[1]);
    Serial.print(", Z: ");
    Serial.print(a[2]);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(r[0]);
    Serial.print(", Y: ");
    Serial.print(r[1]);
    Serial.print(", Z: ");
    Serial.print(r[2]);
    Serial.println(" rad/s");
    Serial.println("");
    delay(800);
}