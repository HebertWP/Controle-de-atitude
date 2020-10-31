#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <FreeRTOS/semphr.h>

SemaphoreHandle_t xSemaphore = NULL;

void Write(void *arg);
void Read(void *arg);

void setup()
{
    Serial.begin(115200);

    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL)
    {
        Serial.println("Erro 0");
    }
    xSemaphoreGive(xSemaphore);
    
    xTaskCreatePinnedToCore(Write,
                            "TaskOnApp",
                            2048,
                            NULL,
                            4,
                            NULL,
                            tskNO_AFFINITY);

    xTaskCreatePinnedToCore(Read,
                            "TaskOnPro",
                            2048,
                            NULL,
                            8,
                            NULL,
                            tskNO_AFFINITY);
}

void loop()
{

}

void Write(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(xSemaphore, (TickType_t)20) == pdTRUE)
        {
            Serial.println("Escrevendo");
            xSemaphoreGive(xSemaphore);
        }
        delay(1000);
    }
}

void Read(void *arg)
{
    while (1)
    {

        if (xSemaphoreTake(xSemaphore, (TickType_t)20) == pdTRUE)
        {
            Serial.println("Lendo");
            xSemaphoreGive(xSemaphore);
        }
        delay(100);
    }
}
