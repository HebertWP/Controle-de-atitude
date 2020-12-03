#include <Arduino.h>
#include <WiFi.h>
#include <PassWordFile.h>
#include <ArduinoOTA.h>
#include <Actuator.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>

#include "MPU6050.h"
#define AEN 17
#define A1 21
#define A2 5
WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.
Actuator motor1(AEN, A1, A2);

const uint8_t sda_pin = 04;
const uint8_t scl_pin = 16;
const uint8_t int_pin = 19;
uint16_t samples = 2000;
uint16_t freq = 500;
int16_t GyZ = 0;
MPU6050 mpu(sda_pin, scl_pin, int_pin, freq, samples);

void tcp();
void initWifiOTA();

void setup()
{
    Serial.begin(115200);
        initWifiOTA();
    ArduinoOTA.begin();
    motor1.init();
    try
    {
        mpu.init();
    }
    catch (MPU6050::Exception e)
    {
        Serial.println(e.what());
        ArduinoOTA.handle();
        delay(1000);
        ESP.restart();
    }
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
                cl.printf("Samples :%i\r\n",samples);
                cl.printf("Freq: %i\r\n",freq);
                mpu.starRead();
                while (!mpu.isReadDone())
                    mpu.reading();
                m = mpu.read();
                cl.printf("X = [ ");
                for (int i = 0; i < samples - 1; i++)
                    cl.printf(" %f, ", m[i]);
                cl.printf(" %f ]\r\n", m[samples - 1]);
                cl.printf("Mean : %f\r\n",mpu.mean());
                cl.printf("Variance : %f\r\n",mpu.variance());
                
                break;
            case 'S':
                while (cl.available() > 0)
                {
                    z = cl.read();
                    read[i++] = z;
                }
                samples = 1000 * (read[0] - 48) + 100 * (read[1] - 48) + 10 * (read[2] - 48) + (read[3] - 48);
                mpu.setNumSamples(samples);
                cl.printf("Samples :%i\r\n",samples);
                break;
            case 'F':
                while (cl.available() > 0)
                {
                    z = cl.read();
                    read[i++] = z;
                }
                freq = 1000 * (read[0] - 48) + 100 * (read[1] - 48) + 10 * (read[2] - 48) + (read[3] - 48);
                mpu.setFreq(freq);
                cl.printf("Freq: %i\r\n",freq);
                break;
            case 'R':
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