#include <Arduino.h>
#include <StartSensor.h>

StarSensor::StarSensor(uint8_t positive_pin_eix_1, uint8_t negative_pin_eix_1, uint8_t positive_pin_eix_2, uint8_t negative_pin_eix_2, uint16_t num_samples, uint16_t freq)
{
    _plus_eix_1 = positive_pin_eix_1;
    _mine_eix_1 = negative_pin_eix_1;
    _plus_eix_2 = positive_pin_eix_1;
    _mine_eix_2 = negative_pin_eix_2;
    _freq = freq;
    _num_samples = num_samples;
    _samples = NULL;
    _i = 0;
    this->setNumSamples(_num_samples);
    this->setFreq(_freq);
}

void StarSensor::setNumSamples(uint16_t num)
{
    _num_samples = num;
    if (_samples != NULL)
        free((void *)_samples);
    _samples = (float *)calloc(_num_samples, sizeof(float));
}

void StarSensor::setFreq(uint16_t freq)
{
    _freq = freq;
}

void StarSensor::starRead()
{
    _i = 0;
    _reading = true;
}

void StarSensor::reading()
{
    static unsigned long time = 0;
    if (!_reading)
        return;
    //    throw MPU6050::Exception(ERRO_4);

    if (millis() - time >= 1000/_freq)
    {
        _samples[_i++] = analogRead(_plus_eix_1);
        time = millis();
    }
    if (_i == _num_samples)
    {
        _reading = false;
    }
}

bool StarSensor::isReadDone()
{
    return !(_reading);
}

float *StarSensor::read()
{
    return _samples;
}

float StarSensor::mean()
{
    float mean = 0;
    for (int i = 0; i < _num_samples; i++)
        mean += _samples[i] / _num_samples;
    return mean;
}

float StarSensor::variance()
{
    float mean = this->mean();
    float variance = 0;
    for (int i = 0; i < _num_samples; i++)
        variance += ((_samples[i] - mean) * (_samples[i] - mean)) / (_num_samples - 1);
    return variance;
}

void StarSensor::calibrate(uint8_t tx, uint8_t rx)
{
    HardwareSerial MySerial(1);
    int plus1[200];
    int neg1[200];
    int plus2[200];
    int neg2[200];
    unsigned long time = 0;
    MySerial.begin(115200, SERIAL_8N1, rx, tx);

    for (int i = 0; i < 200; i++)
    {
        time = millis();
        plus1[i] = analogRead(_plus_eix_1);
        neg1[i] = analogRead(_mine_eix_1);
        plus2[i] = analogRead(_plus_eix_2);
        neg2[i] = analogRead(_mine_eix_2);
        MySerial.write('+');
        while (millis() - time < 100)
            ;
    }

    Serial.printf("Plus_eix_1_table = {");
    for (int i = 0; i < 200 - 1; i++)
    {
        Serial.printf("%i, ", plus1[i]);
    }
    Serial.printf("%i };\r\n", plus1[199]);

    Serial.printf("Plus_eix_2_table = {");
    for (int i = 0; i < 200 - 1; i++)
    {
        Serial.printf("%i, ", plus2[i]);
    }
    Serial.printf("%i };\r\n", plus2[199]);

    Serial.printf("Neg_eix_1_table = {");
    for (int i = 0; i < 200 - 1; i++)
    {
        Serial.printf("%i, ", neg1[i]);
    }
    Serial.printf("%i };\r\n", neg1[199]);

    Serial.printf("Neg_eix_2_table = {");
    for (int i = 0; i < 200 - 1; i++)
    {
        Serial.printf("%i, ", neg2[i]);
    }
    Serial.printf("%i };\r\n", neg2[199]);
}