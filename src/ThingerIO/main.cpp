#include "ComunicationSystem.h"
#include "PassWordFile.h"
#include <math.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
double ace[3];
double rot[3];
ComunicationSystem thing; //(USERNAME_THINGER, DEVICE_ID_THINGER, DEVICE_CREDENTIAL_THINGER);
pson out;
boolean stabilize = 0;
void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NAME, WIFI_PASS);

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        })
        .onEnd([]() {
            Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                Serial.println("End Failed");
        });

    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");

    Serial.println(WiFi.localIP());

    Serial.println("Starting...");

    thing["stabilize"] << inputValue(stabilize);

    /*    thing["Rotacao"] >> [](pson &out) {
        out["X"] = rot[0];
        out["Y"] = rot[1];
        out["Z"] = rot[2];
    };
*/
    Serial.println("Started");
}

void loop()
{
    Serial.println(stabilize);
    thing.handle();
    ArduinoOTA.handle();
    delay(10);
}