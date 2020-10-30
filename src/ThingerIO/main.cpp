#include <ThingerESP32.h>
#include "PassWordFile.h"

ThingerESP32 thing(USERNAME_THINGER, DEVICE_ID_THINGER, DEVICE_CREDENTIAL_THINGER);

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting...");
    
    pinMode(LED_BUILTIN, OUTPUT);

    thing.add_wifi(WIFI_NAME, WIFI_PASS);

    // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
    thing["led"] << digitalPin(LED_BUILTIN);

    // resource output example (i.e. reading a sensor value)
    thing["millis"] >> outputValue(millis());

    // more details at http://docs.thinger.io/arduino/
    Serial.println("Started");    
}

void loop()
{
    thing.handle();
}