#include <ThingerESP32.h>
#include "PassWordFile.h"
#include <math.h>
double ace[3];
double rot[3];
ThingerESP32 thing(USERNAME_THINGER, DEVICE_ID_THINGER, DEVICE_CREDENTIAL_THINGER);
pson out;
int parameter = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(LED_BUILTIN, OUTPUT);
    out = ace[0];
    thing.add_wifi(WIFI_NAME, WIFI_PASS);

    thing["enable_disable"] << inputValue(parameter);

    thing["Acelaracao"] >> [](pson &out) {
        out["X"] = ace[0];
        out["Y"] = ace[1];
        out["Z"] = ace[2];
    };
    thing["Rotacao"] >> [](pson &out) {
        out["X"] = rot[0];
        out["Y"] = rot[1];
        out["Z"] = rot[2];
    };

    // more details at http://docs.thinger.io/arduino/
    Serial.println("Started");
}

void loop()
{
    for (float i = 0; i < 2 * PI; i = i + 0.2)
    {
        ace[0] = sin(i);
        ace[1] = -ace[0];
        ace[2] = cos(i);
        thing.handle();
        Serial.println(parameter);
        delay(10);
    }
}