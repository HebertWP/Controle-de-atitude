#include <Arduino.h>
#include <WiFi.h>
#include <PassWordFile.h>
#include <ArduinoOTA.h>
#include "StartSensor.h"
#include <Actuator.h>

#define POSITIVE1 32
#define NEGATIVE1 35
#define POSITIVE2 33
#define NEGATIVE2 34

#define AEN 17
#define A1 18
#define A2 5
Actuator motor1(AEN, A1, A2);

WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.

uint16_t samples = 30;
uint16_t freq = 100;
StarSensor sens(POSITIVE1, NEGATIVE1, POSITIVE2, NEGATIVE2, samples, freq);

void tcp();
void initWifiOTA();

void setup()
{
    Serial.begin(115200);
    initWifiOTA();
    ArduinoOTA.handle();
    motor1.init();
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
            char z = cl.read();
            char read[4];
            int i = 0;
            float *m;
            switch (z)
            {
            case 'E':
            case 'e':
                cl.printf("Samples :%i\r\n", samples);
                cl.printf("Freq: %i\r\n", freq);
                sens.starRead();
                while (!sens.isReadDone())
                    sens.reading();
                m = sens.read();
                cl.printf("X = [ ");
                for (int i = 0; i < samples - 1; i++)
                    cl.printf(" %f, ", m[i]);
                cl.printf(" %f ]\r\n", m[samples - 1]);
                cl.printf("Mean : %f\r\n", sens.mean());
                cl.printf("Variance : %f\r\n", sens.variance());

                break;
            case 'S':
            case 's':
                while (cl.available() > 0)
                {
                    z = cl.read();
                    read[i++] = z;
                }
                samples = 1000 * (read[0] - 48) + 100 * (read[1] - 48) + 10 * (read[2] - 48) + (read[3] - 48);
                sens.setNumSamples(samples);
                cl.printf("Samples :%i\r\n", samples);
                break;
            case 'F':
            case 'f':
                while (cl.available() > 0)
                {
                    z = cl.read();
                    read[i++] = z;
                }
                freq = 1000 * (read[0] - 48) + 100 * (read[1] - 48) + 10 * (read[2] - 48) + (read[3] - 48);
                sens.setFreq(freq);
                cl.printf("Freq: %i\r\n", freq);
                break;
            case 'C':
            case 'c':
                sens.calibrate(&cl);
                break;
            case 'L':
            case 'l':
                sens.setNumSamples(10);
                sens.setFreq(10);

                for (int i = 0; i < 30; i++)
                {
                    sens.starRead();
                    while (!sens.isReadDone())
                        sens.reading();
                    m = sens.read();
                    cl.printf("Mean : %f\r\n", sens.mean());
                }
                sens.setFreq(freq);
                sens.setNumSamples(samples);
                break;
            case 'R':
            case 'r':
                ESP.restart();
                break;
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
    sv.begin();
}