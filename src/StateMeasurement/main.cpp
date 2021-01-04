#include <Arduino.h>
#include <WiFi.h>
#include "PassWordFile.h"
#include <ArduinoOTA.h>
#include "StateMeasurement.h"
#include <Actuator.h>

WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.

#define AEN 17
#define A1 21
#define A2 5
Actuator motor1(AEN, A1, A2);

#define POSITIVE1 32
#define NEGATIVE1 35
#define POSITIVE2 33
#define NEGATIVE2 34

#define SDAPIN 04
#define SCLPIN 16
#define INTPIN 19

StateMeasurement a(SDAPIN, SCLPIN, INTPIN, POSITIVE1, NEGATIVE1, POSITIVE2, NEGATIVE2, &cl);
void tcp();
void initWifiOTA();

void setup()
{
    Serial.begin(115200);
    initWifiOTA();
    ArduinoOTA.handle();
    motor1.init();
    a.init();
}

void loop()
{
    ArduinoOTA.handle();
    tcp();
}

void tcp()
{
    State m;
    if (cl.connected())
    {
        a.startMeasurementCycle();
        while (!a.isMeasurementDone())
            a.inMeasurement();
        m = a.measurement();
        cl.printf("X = | %f |\r\n", m.angular_position);
        cl.printf("    | %f |\r\n", m.angular_speed);
        delay(1000);
    }
    else
    {
        cl = sv.available();
        delay(1);
    }
}

void initWifiOTA()
{
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
    sv.begin();
}