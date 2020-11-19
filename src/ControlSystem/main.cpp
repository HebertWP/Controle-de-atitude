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
WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.
Actuator motor1(AEN, A1, A2);
void tcp();
void setup()
{
    Serial.begin(115200); //Habilita a comm serial.
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

    sv.begin(); //Inicia o servidor TCP na porta declarada no começo.
}

void loop()
{
    ArduinoOTA.handle();
    tcp(); //Funçao que gerencia os pacotes e clientes TCP.
}

void tcp()
{
    if (cl.connected()) //Detecta se há clientes conectados no servidor.
    {
        if (cl.available() > 0) //Verifica se o cliente conectado tem dados para serem lidos.
        {
            char read[2];
            int i = 0;
            while (cl.available() > 0) //Armazena cada Byte (letra/char) na String para formar a mensagem recebida.
            {
                char z = cl.read();
                read[i++] = z;
            }
            switch (read[0])
            {
            case 'E':
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
    else //Se nao houver cliente conectado,
    {
        cl = sv.available(); //Disponabiliza o servidor para o cliente se conectar.
        delay(1);
    }
}