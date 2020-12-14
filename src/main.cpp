#include <Arduino.h>

void setup()
{

}

void loop()
{
    
}
/*
#include "ComunicationSystem.h"
#include "PassWordFile.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "InertialMesure.h"
#include "ControlSystem.h"

double rot[3];
ComunicationSystem thing; //(USERNAME_THINGER, DEVICE_ID_THINGER, DEVICE_CREDENTIAL_THINGER);
pson out;
boolean stabilize = 0;
#define AEN 17
#define A1 18
#define A2 5
Actuator motor1(AEN, A1, A2);
WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.
void tcp();

void setup()
{
    InertialMesure::init();
    motor1.init();
    ControlSystem::init(&motor1);
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

    thing["Rotation"] >> [](pson &out) {
        out["X"] = rot[0];
        out["Y"] = rot[1];
        out["Z"] = rot[2];
    };

    Serial.println("Started");
}

void loop()
{
    static boolean old = stabilize;

    double *r = InertialMesure::getRotation();

    rot[0] = r[0];
    rot[1] = r[1];
    rot[2] = r[2];

    if (old != stabilize)
    {
        if (stabilize == 0)
            ControlSystem::turnOFF();
        else
            ControlSystem::turnON();
        old = stabilize;
    }
    thing.handle();
    ArduinoOTA.handle();
    Serial.println(stabilize);
    //tcp(); //Funçao que gerencia os pacotes e clientes TCP.
    delay(10);
}

void tcp()
{
    if (cl.connected()) //Detecta se há clientes conectados no servidor.
    {
        cl.println(stabilize);
    }
    else //Se nao houver cliente conectado,
    {
        cl = sv.available(); //Disponabiliza o servidor para o cliente se conectar.
        delay(1);
    }
}
*/