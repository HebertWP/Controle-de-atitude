#include <Arduino.h>
#include <StartSensor.h>

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
    _p1 = 450;
    _p2 = 428;
    _n2 = 550;
    _n1 = 600;
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
    typedef struct
    {
        float varadm;
        float ang;
    } data;

    data varadm[4];
    for (int i = 0; i < _num_samples; i++)
    {
        varadm[0].varadm = (float)_samples_plus_1[i] / (float)_p1;
        varadm[0].ang = 0;
        varadm[1].varadm = (float)_samples_plus_2[i] / (float)_p2;
        varadm[1].ang = 90;
        varadm[2].varadm = (float)_samples_mine_2[i] / (float)_n2;
        varadm[2].ang = -90;
        varadm[3].varadm = (float)_samples_mine_1[i] / (float)_n1;
        varadm[3].ang = 180;
        for (int i = 0; i < 3; i++)
            for (int y = i + 1; y < 4; y++)
                if (varadm[i].varadm > varadm[y].varadm)
                {
                    data aux = varadm[i];
                    varadm[i] = varadm[y];
                    varadm[y] = aux;
                }
        varadm[0].varadm = (varadm[0].varadm - 1 < 0) ? 0 : varadm[0].varadm - 1;
        _samples[i] = varadm[0].varadm * varadm[0].varadm * varadm[0].varadm * varadm[0].varadm * 10 + varadm[0].varadm * varadm[0].varadm * varadm[0].varadm * 10 + varadm[0].varadm * varadm[0].varadm * 10 + varadm[0].varadm * 10;

        if ((varadm[0].ang == 0 && varadm[1].ang == -90) ||
            (varadm[0].ang == 90 && varadm[1].ang == 0) ||
            (varadm[0].ang == -90 && varadm[1].ang == 180))
            _samples[i] = -_samples[i];
        _samples[i] += varadm[0].ang;

        float aux = varadm[1].varadm * varadm[1].varadm * varadm[1].varadm * varadm[1].varadm * .5 + varadm[1].varadm * varadm[1].varadm * varadm[1].varadm * .5 + varadm[1].varadm * varadm[1].varadm * .5 + varadm[1].varadm * .5;
        aux=1/aux;
        if ((varadm[0].ang == 0 && varadm[1].ang == -90) ||
            (varadm[0].ang == 90 && varadm[1].ang == 0) ||
            (varadm[0].ang == -90 && varadm[1].ang == 180))
            aux = -aux;
        _samples[i] += aux;
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
        float aux;
        cl->printf("_p1: %i; _p2: %i; _n1: %i; _n2: %i;\r\n", _p1, _p2, _n1, _n2);
        aux = (float)p1 / (float)_p1;
        cl->printf("P1: %i; Var1: %f;\r\n", p1, aux);
        aux = (float)p2 / (float)_p2;
        cl->printf("P2: %i; Var2: %f;\r\n", p2, aux);
        aux = (float)n2 / (float)_n2;
        cl->printf("N2: %i; Var2: %f;\r\n", n2, aux);
        cl->printf("N1: %i;\r\n", n1);
        k = false;
        while (!cl->available())
        {
            c = Serial.read();
            if (c == 'C' || c == 'c')
                k = false;
        }
    }
}