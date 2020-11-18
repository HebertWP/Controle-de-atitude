#include <WiFi.h>
#include <Arduino.h>
#include <Actuator.h>
#include <PassWordFile.h>
#include <ArduinoOTA.h>

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
    tcp(); //Funçao que gerencia os pacotes e clientes TCP.
    ArduinoOTA.handle();
}

void tcp()
{
    if (cl.connected()) //Detecta se há clientes conectados no servidor.
    {
        if (cl.available() > 0) //Verifica se o cliente conectado tem dados para serem lidos.
        {
            char read[6];
            float num = 0;
            int i = 0;
            while (cl.available() > 0) //Armazena cada Byte (letra/char) na String para formar a mensagem recebida.
            {
                char z = cl.read();
                read[i++] = z;
            }
            num = (read[1] - 48) + 0.1 * (read[3] - 48) + 0.01 * (read[4] - 48);
            num = (read[0] == '-') ? -num : num;

            //cl.print("\n...Sua mensagem: " + req + "\n");
            if (-1 <= num && num <= 1)
            {
                motor1.setPower(num);
            }
        }
    }
    else //Se nao houver cliente conectado,
    {
        cl = sv.available(); //Disponabiliza o servidor para o cliente se conectar.
        delay(1);
    }
}
/*#include <Arduino.h>
#include <Actuator.h>
#include <PassWordFile.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define AEN 17
#define A1 18
#define A2 5
WiFiServer sv(555); //Cria o objeto servidor na porta 555
WiFiClient cl;      //Cria o objeto cliente.

Actuator motor1(AEN, A1, A2);
void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");
    sv.begin();
    Serial.println("HAY");
    motor1.init();
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
}
void tcp();
void loop()
{
    for (float i = 0.8; i < 1; i = i + 0.01)
    {
        ArduinoOTA.handle();
        motor1.setPower(i);
        delay(200);
    }
    delay(2000);
    for (float i = -0.8; i > -1; i = i - 0.01)
    {
        ArduinoOTA.handle();
        motor1.setPower(i);
        delay(200);
    }
    delay(2000);
    tcp(); //Funçao que gerencia os pacotes e clientes TCP.
}

void tcp()
{
    if (cl.connected()) //Detecta se há clientes conectados no servidor.
    {
        if (cl.available() > 0) //Verifica se o cliente conectado tem dados para serem lidos.
        {
            String req = "";
            while (cl.available() > 0) //Armazena cada Byte (letra/char) na String para formar a mensagem recebida.
            {
                char z = cl.read();
                req += z;
            }
            //Mostra a mensagem recebida do cliente no Serial Monitor.
            Serial.print("\nUm cliente enviou uma mensagem");
            Serial.print("\n...IP do cliente: ");
            Serial.print(cl.remoteIP());
            Serial.print("\n...IP do servidor: ");
            Serial.print(WiFi.softAPIP());
            Serial.print("\n...Mensagem do cliente: " + req + "\n");
            //Envia uma resposta para o cliente
            cl.print("\nO servidor recebeu sua mensagem");
            cl.print("\n...Seu IP: ");
            cl.print(cl.remoteIP());
            cl.print("\n...IP do Servidor: ");
            cl.print(WiFi.softAPIP());
            cl.print("\n...Sua mensagem: " + req + "\n");
        }
    }
    else //Se nao houver cliente conectado,
    {
        cl = sv.available(); //Disponabiliza o servidor para o cliente se conectar.
        delay(1);
    }
}*/