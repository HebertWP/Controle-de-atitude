#include <Arduino.h>
#include <StartSensor.h>

StarSensor::Calibrate StarSensor::_p1 = {180, 3763};
StarSensor::Calibrate StarSensor::_p2 = {190, 3800};
StarSensor::Calibrate StarSensor::_n1 = {300, 0};
StarSensor::Calibrate StarSensor::_n2 = {280, 0};

StarSensor::StarSensor(uint8_t positive_pin_eix_1, uint8_t negative_pin_eix_1, uint8_t positive_pin_eix_2, uint8_t negative_pin_eix_2, uint16_t num_samples, uint16_t freq)
{
    _plus_eix_1 = positive_pin_eix_1;
    _mine_eix_1 = negative_pin_eix_1;
    _plus_eix_2 = positive_pin_eix_2;
    _mine_eix_2 = negative_pin_eix_2;
    _freq = freq;
    _num_samples = num_samples;
    _samples_plus_1 = NULL;
    _samples_mine_1 = NULL;
    _samples_plus_2 = NULL;
    _samples_mine_2 = NULL;
    _samples = NULL;
    _i = 0;
    this->setNumSamples(_num_samples);
    this->setFreq(_freq);
}

void StarSensor::setNumSamples(uint16_t num)
{
    _num_samples = num;
    if (_samples_plus_1 != NULL)
        free((void *)_samples_plus_1);
    _samples_plus_1 = (int *)calloc(_num_samples, sizeof(int));

    if (_samples_plus_2 != NULL)
        free((void *)_samples_plus_2);
    _samples_plus_2 = (int *)calloc(_num_samples, sizeof(int));

    if (_samples_mine_1 != NULL)
        free((void *)_samples_mine_1);
    _samples_mine_1 = (int *)calloc(_num_samples, sizeof(int));

    if (_samples_mine_2 != NULL)
        free((void *)_samples_mine_2);
    _samples_mine_2 = (int *)calloc(_num_samples, sizeof(int));

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

    if (millis() - time >= 1000 / _freq)
    {
        _samples_plus_1[_i] = analogRead(_plus_eix_1);
        _samples_mine_1[_i] = analogRead(_mine_eix_1);
        _samples_plus_2[_i] = analogRead(_plus_eix_2);
        _samples_mine_2[_i] = analogRead(_mine_eix_2);
        _i++;
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
    float varadm = 0;

    for (int i = 0; i < _num_samples; i++)
    {
        varadm = _samples_plus_1[i] / StarSensor::_p1.in;
        _samples[i] = 0;

        if (varadm > 1.5)
        {
            if (_samples_plus_2[i] / StarSensor::_p2.in < _samples_mine_2[i] / StarSensor::_n2.in)
                _samples[i] = 45;
            else
                _samples[i] = -45;
        }

        if (varadm > _samples_plus_2[i] / StarSensor::_p2.in)
        {
            varadm = _samples_plus_2[i] / StarSensor::_p2.in;
            _samples[i] = 90;
            if (varadm > 1.5)
            {
                if (_samples_plus_1[i] / StarSensor::_p1.in < _samples_mine_1[i] / StarSensor::_n1.in)
                    _samples[i] = -45;
                else
                    _samples[i] = 135;
            }
        }

        if (varadm > _samples_mine_1[i] / StarSensor::_n1.in)
        {
            varadm = _samples_mine_1[i] / StarSensor::_n1.in;
            _samples[i] = 180;
            if (varadm > 1.5)
            {
                if (_samples_plus_2[i] / StarSensor::_p2.in < _samples_mine_2[i] / StarSensor::_n2.in)
                    _samples[i] = 135;
                else
                    _samples[i] = -135;
            }
        }

        if (varadm > _samples_mine_2[i] / StarSensor::_n2.in)
        {
            varadm = _samples_mine_2[i] / StarSensor::_n2.in;
            _samples[i] = -90;
            if (varadm > 1.5)
            {
                if (_samples_plus_1[i] / StarSensor::_p1.in < _samples_mine_1[i] / StarSensor::_n1.in)
                    _samples[i] = -45;
                else
                    _samples[i] = -135;
            }
        }
    }
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

void StarSensor::calibrate(WiFiClient *cl)
{
    HardwareSerial MySerial(1);
    char c;
    int p1, n1, p2, n2;
    boolean k = true;
    for (int i = 0; k; i++)
    {
        p1 = 0;
        p2 = 0;
        n1 = 0;
        n2 = 0;
        for (int y = 0; y < 10; y++)
        {
            p1 += analogRead(_plus_eix_1) / 10;
            n1 += analogRead(_mine_eix_1) / 10;
            p2 += analogRead(_plus_eix_2) / 10;
            n2 += analogRead(_mine_eix_2) / 10;
            delay(30);
        }
        cl->printf("%i,", p1);
        cl->printf("%i,", p2);
        cl->printf("%i,", n1);
        cl->printf("%i,\r\n", n2);
        k = false;
        while (!cl->available())
        {
            c = Serial.read();
            if (c == 'C' || c == 'c')
                k = false;
        }
    }
}