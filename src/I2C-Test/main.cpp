#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PassWordFile.h>
#include <ArduinoOTA.h>

#define SET_BITS(OUTPUT, INPUT, NUM_BITS, TO_SHIFT) (OUTPUT = ((OUTPUT & (~((0XFFU >> (8 - NUM_BITS)) << TO_SHIFT))) | (INPUT << TO_SHIFT)))

const int I2C_ADDR = 0x68;

const int sda_pin = 04;
const int scl_pin = 16;
#define MAX_READ 1024

int out[MAX_READ];

WiFiServer sv(555);
WiFiClient cl;

enum state
{
    IDLE,
    READY,
    REG_16,
    REG,
    GET,
    TIME_16,
    TIME,
    BURST,
    SAME,
    SET,
    DATA_16,
    DATA,
    TO_SHIFT,
    NUM_BITS,
};

void processByteCommunication(char ucByte);

void writeReg(int reg, int val);
int *readReg(uint8_t reg, int q);
void findI2C();

void tcp();

void setup()
{
    Serial.begin(115200);
    Wire.begin(sda_pin, scl_pin);
    findI2C();

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
    if (Serial.available() > 0)
        processByteCommunication(Serial.read());
    ArduinoOTA.handle();
}

void writeReg(int reg, int val)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission(true);
}

int *readReg(uint8_t reg, int q)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_ADDR, q);

    for (int i = 0; i < q; i++)
        out[i] = Wire.read();
    return out;
}

void findI2C()
{
    Wire.beginTransmission(I2C_ADDR);
    int data = Wire.endTransmission(true);

    if (data == 0)
    {
        Serial.print("Dispositivo encontrado no endereço: 0x");
        Serial.println(I2C_ADDR, HEX);
    }
    else
    {
        Serial.println("Dispositivo não encontrado!");
        delay(2000);
        ESP.restart();
    }
}

void processByteCommunication(char ucByte)
{
    static enum state esUartState = IDLE;
    static int reg = 0;
    static int time = 0;
    static int data = 0;
    static int tosh = 0;
    static int nuby = 0;

    switch (esUartState)
    {
    case IDLE:
        switch (ucByte)
        {
        case '#':
            esUartState = READY;
            break;
        }
        break;

    case READY:
        if (48 <= (uint8_t)ucByte && (uint8_t)ucByte <= 57)
        {
            reg = (ucByte - 48) * 16;
            esUartState = REG_16;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            reg = (ucByte - 55) * 16;
            esUartState = REG_16;
        }
        else
            esUartState = IDLE;
        break;

    case REG_16:
        if ('0' <= ucByte && ucByte <= '9')
        {
            reg = reg + (ucByte - 48);
            esUartState = REG;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            reg = reg + (ucByte - 55);
            esUartState = REG;
        }
        else
            esUartState = IDLE;
        break;

    case REG:
        switch (ucByte)
        {
        case 'g':
            esUartState = GET;
            break;
        case 's':
            esUartState = SET;
            break;
        case ';':
            Serial.print("Reg: 0x");
            Serial.print(reg, HEX);
            Serial.print(" / ");
            Serial.print(reg, BIN);
            Serial.print("; Val: 0x");
            int *out;
            out = readReg(reg, 1);
            Serial.print(out[0], HEX);
            Serial.print(" / ");
            Serial.print(out[0], BIN);
            Serial.println(";");

        default:
            esUartState = IDLE; /*some one send comand via uart in same time, wi back idle state, before acet nem orders */
        }
        break;

    case GET: /*set KP*/
        if ('0' <= ucByte && ucByte <= '9')
        {
            time = (ucByte - 48) * 16;
            esUartState = TIME_16;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            time = (ucByte - 55) * 16;
            esUartState = TIME_16;
        }
        else
            esUartState = IDLE;
        break;

    case TIME_16:
        if ('0' <= ucByte && ucByte <= '9')
        {
            time = time + (ucByte - 48);
            esUartState = TIME;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            time = time + (ucByte - 55);
            esUartState = TIME;
        }
        else
            esUartState = IDLE;
        break;

    case TIME:
        switch (ucByte)
        {
        case 'b':
            esUartState = BURST;
            break;
        case 's':
            esUartState = SAME;
            break;
        default:
            esUartState = IDLE;
        }
        break;

    case BURST:
        if (ucByte == ';')
        {
            Serial.printf("Reg: %i; time: %i; mode: Burst;\n", reg, time);
            int *out;
            out = readReg(reg, time);
            for (int i = 0; i < time; i++)
            {
                Serial.print("0x");
                Serial.println(out[i], HEX);
            }
        }
        esUartState = IDLE;
        break;

    case SAME:
        if (ucByte == ';')
        {
            Serial.printf("Reg: %i; time: %i; mode: Same;\n", reg, time);
            int *out;
            for (int i = 0; i < time; i++)
            {
                out = readReg(reg, 1);
                Serial.print("0x");
                Serial.println(out[0], HEX);
            }
        }
        esUartState = IDLE;
        break;

    case SET:
        if ('0' <= ucByte && ucByte <= '9')
        {
            data = (ucByte - 48) * 16;
            esUartState = DATA_16;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            data = (ucByte - 55) * 16;
            esUartState = DATA_16;
        }
        else
            esUartState = IDLE;
        break;

    case DATA_16:
        if ('0' <= ucByte && ucByte <= '9')
        {
            data = data + (ucByte - 48);
            esUartState = DATA;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            data = data + (ucByte - 55);
            esUartState = DATA;
        }
        else
            esUartState = IDLE;
        break;

    case DATA:
        if ('0' <= ucByte && ucByte <= '9')
        {
            tosh = (ucByte - 48);
            esUartState = TO_SHIFT;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            tosh = (ucByte - 55);
            esUartState = TO_SHIFT;
        }
        else if (ucByte == ';')
        {

            Serial.print("Reg: 0x");
            Serial.print(reg, HEX);
            Serial.print(" / ");
            Serial.print(reg, BIN);
            int out=0;
            SET_BITS(out, data, 8, 0);
            Serial.print("; Val: 0x");
            Serial.print(out, HEX);
            Serial.print(" / ");
            Serial.print(out, BIN);
            Serial.print(";\n");
            writeReg(reg, out);

            esUartState = IDLE;
        }
        else
            esUartState = IDLE;

        break;

    case TO_SHIFT:
        if ('0' <= ucByte && ucByte <= '9')
        {
            nuby = (ucByte - 48);
            esUartState = NUM_BITS;
        }
        else if ('A' <= ucByte && ucByte <= 'F')
        {
            nuby = (ucByte - 55);
            esUartState = NUM_BITS;
        }
        else
            esUartState = IDLE;
        break;

    case NUM_BITS:
        if (ucByte == ';')
        {
            Serial.printf("Reg: %i; data: %i; tosh: %i; numbi: %i;\n", reg, data, tosh, nuby);
            int *out = readReg(reg, 1);

            SET_BITS(out[0], data, nuby, tosh);
            Serial.print("O valor do regstrador será : 0x");
            Serial.print(out[0], HEX);
            Serial.print(" / ");
            Serial.print(out[0], BIN);
            Serial.print(";\n");
            writeReg(reg, out[0]);
        }
        esUartState = IDLE;
        break;
    }
}