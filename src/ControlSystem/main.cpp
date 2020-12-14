#include <Arduino.h>
#include <WiFi.h>
#include <PassWordFile.h>
#include <ArduinoOTA.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>

#include "StateMeasurement.h"
#include "Actuator.h"
#include "ControlSystem.h"

WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.

#define AEN 17
#define A1 21
#define A2 5
Actuator motor1(AEN, A1, A2);

#define POSITIVE1 33
#define NEGATIVE1 35
#define POSITIVE2 34
#define NEGATIVE2 32

#define SDAPIN 04
#define SCLPIN 16
#define INTPIN 19

StateMeasurement state(SDAPIN, SCLPIN, INTPIN, POSITIVE1, NEGATIVE1, POSITIVE2, NEGATIVE2, &cl);

void tcp();
void initWifiOTA();

void setup()
{
    Serial.begin(115200);
    initWifiOTA();
    sv.begin();
    ArduinoOTA.handle();
    ControlSystem::init(&motor1,&state, &cl);
}
void loop()
{
    ArduinoOTA.handle();
    tcp();
}
void tcp()
{
    if (cl.connected())
    {
        if (cl.available() > 0) //Verifica se o cliente conectado tem dados para serem lidos.
        {
            char c = cl.read();
            switch (c)
            {
            case 'O':
            case 'o':
                cl.printf("Controle ligado\r\n");
                ControlSystem::turnON();
                break;
            case 'F':
            case 'f':
                cl.printf("Controle desligado\r\n");
                ControlSystem::turnOFF();
                break;
            case 'R':
            case 'r':
                ESP.restart();
            default:
                break;
            }
        }
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
}
/*
#include <WiFi.h>
#include <Arduino.h>
#include <Actuator.h>
#include <PassWordFile.h>
#include <ArduinoOTA.h>
#include "InertialMesure.h"
#include "ControlSystem.h"

#define AEN 17
#define A1 18
#define A2 5
WiFiServer sv(555);
WiFiClient cl;
Actuator motor1(AEN, A1, A2);
void tcp();
void initOTA();
void setup()
{
    Serial.begin(115200);
    motor1.init();
    InertialMesure::init();
    ControlSystem::init(&motor1);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NAME, WIFI_PASS);

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    initOTA();

    sv.begin(); //Inicia o servidor TCP na porta declarada no começo.
}

void loop()
{
    ArduinoOTA.handle();
    tcp();
}

void tcp()
{
    if (cl.connected())
    {
        if (cl.available() > 0)
        {
            char read[2];
            int i = 0;
            while (cl.available() > 0)
            {
                char z = cl.read();
                read[i++] = z;
            }
            switch (read[0])
            {
            case 'R':
                cl.println("Reset");
                ESP.restart();
                break;
            case 'O':
                cl.println("Turn ON");
                ControlSystem::turnON();
                break;
            case 'F':
                cl.println("Turn OFF");
                ControlSystem::turnOFF();
                break;
            }
        }
    }
    else
    {
        cl = sv.available();
        delay(1);
    }
}

void initOTA()
{
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
}
*/