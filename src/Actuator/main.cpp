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
            if (read[0] != 'L')
            {
                num = (read[1] - 48) + 0.1 * (read[3] - 48) + 0.01 * (read[4] - 48);
                num = (read[0] == '-') ? -num : num;

                //cl.print("\n...Sua mensagem: " + req + "\n");
                if (-1 <= num && num <= 1)
                {
                    motor1.setPower(num);
                    cl.printf("Potencia do motor em : %f\%\n", motor1.getPower() * 100);
                }
            }
            else
            {
                cl.printf("In Loop \n");
                for (float i = -1; i <= 1; i = i + 0.1)
                {
                    motor1.setPower(i);
                    cl.printf("Power in: %f\r\n", motor1.getPower() * 100);
                    delay(1000);
                }
                for (float i = 1; i >= -1; i = i - 0.1)
                {
                    motor1.setPower(i);
                    cl.printf("Power in: %f\r\n", motor1.getPower() * 100);
                    delay(1000);
                }
                for (float i = -1; i <= 0; i = i + 0.1)
                {
                    motor1.setPower(i);
                    cl.printf("Power in: %f\r\n", motor1.getPower() * 100);
                    delay(1000);
                }
                cl.printf("Closed loop \n");
            }
        }
    }
    else //Se nao houver cliente conectado,
    {
        cl = sv.available(); //Disponabiliza o servidor para o cliente se conectar.
        delay(1);
    }
}