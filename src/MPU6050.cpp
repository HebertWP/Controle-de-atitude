#ifdef ESP

#include <Arduino.h>
#include <Wire.h>
#include "MPU6050.h"
#include "statistic.h"

bool MPU6050::ready = false;

MPU6050::MPU6050(uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, uint16_t freq, uint16_t num_samples, int edr_mpu, int8_t gyr_scale, int8_t acc_scale, int8_t dlpf, int8_t fifo)
{
    _scl_pin = scl_pin;
    _sda_pin = sda_pin;
    _int_pin = int_pin;
    _edr_mpu = edr_mpu;
    _gyr_scale = gyr_scale;
    _acc_scale = acc_scale;
    _dlpf = dlpf;
    _int_enable = true;
    _FIFO = fifo;
    _freq = (1000 - freq) / freq;
    _num_samples = num_samples;
    _samples = NULL;
    _i = 0;
    _reading = false;
}

void MPU6050::init()
{
    pinMode(_int_pin, INPUT);
    Wire.begin(_sda_pin, _scl_pin);
    this->checkConnection();
    writeRegMPU(REG_PWR_MGMT_1, 0);
    this->setGyroScale(_gyr_scale);
    this->setAccelScale(_acc_scale);
    this->setDLPF(_dlpf);
    this->setFreq(1000 / (1 + _freq));
    this->enableInt(_int_enable);
    this->enableFIFO();
    this->setFIFO(_FIFO);
    this->setNumSamples(_num_samples);
    pinMode(_int_pin, INPUT);
}

void MPU6050::starRead()
{
    this->checkConnection();
    _i = 0;
    _reading = true;

    this->enableFIFO();
    MPU6050::ready = false;
    attachInterrupt(_int_pin, MPU6050::ISR, RISING);
}

void MPU6050::reading()
{
    Serial.printf("%i\n", _i);
    if (!_reading)
        throw MPU6050::Exception(ERRO_4);
    if (!MPU6050::ready)
        return;

    MPU6050::ready = false;
    uint16_t tam;
    int16_t a;

    tam = this->readTwoRegMPU(REG_FIFO_COUNT_H);
    for (int i = 0; i < tam / 2 && _i < _num_samples; i++)
    {
        a = this->readTwoRegMPU(REG_FIFO);
        _samples[_i++] = ((float)a) / _G_LSB;
    }
    if (_i == _num_samples)
    {
        detachInterrupt(_int_pin);
        _reading = false;
    }
}

bool MPU6050::isReadDone()
{
    return !(_reading);
}

float *MPU6050::read()
{
    return _samples;
}

void MPU6050::setGyroScale(uint8_t scale)
{
    this->checkConnection();
    _gyr_scale = scale;
    writeRegMPU(REG_G_CFG, _gyr_scale);
    /* FS_SEL   | Full Scale Range  | LSB Sensitivity   | Use define 
     *  0       |   ± 250 °/s       |   131 LSB/°/s     |   G_CFG_250
     *  1       |   ± 500 °/s       |   65.5 LSB/°/s    |   G_CFG_500
     *  2       |   ± 1000 °/s      |   32.8 LSB/°/s    |   G_CFG_1000
     *  3       |   ± 2000 °/s      |   16.4 LSB/°/s    |   G_CFG_2000
    */
    switch (_gyr_scale)
    {
    case G_CFG_250:
        _G_LSB = 131;
        break;
    case G_CFG_500:
        _G_LSB = 65.5;
        break;
    case G_CFG_1000:
        _G_LSB = 32.8;
        break;
    case G_CFG_2000:
        _G_LSB = 16.4;
        break;
    default:
        throw MPU6050::Exception(ERRO_3);
        break;
    };
}

void MPU6050::setAccelScale(uint8_t scale)
{
    this->checkConnection();
    _acc_scale = scale;
    writeRegMPU(REG_A_CFG, _acc_scale);

    /* FS_SEL   | Full Scale Range  | LSB Sensitivity   |  Use define 
     *  0       |   ± 2g            |   16384LSB/g      |   A _CFG_2G
     *  1       |   ± 4g            |   8192 LSB/g      |   A_CFG_4G
     *  2       |   ± 8g            |   4096 LSB/g      |   A_CFG_8G
     *  3       |   ± 16g           |   2048 LSB/g      |   A_CFG_16G
     */
    switch (_acc_scale)
    {
    case A_CFG_2G:
        _A_LSB = 16384;
        break;
    case A_CFG_4G:
        _A_LSB = 8192;
        break;
    case A_CFG_8G:
        _A_LSB = 4096;
        break;
    case A_CFG_16G:
        _A_LSB = 2048;
        break;
    default:
        throw MPU6050::Exception(ERRO_3);
        break;
    };
}

void MPU6050::setDLPF(uint8_t dlpf)
{
    _dlpf = dlpf;
    this->checkConnection();
    this->writeRegMPU(REG_DLPF_CFG, _dlpf);
}

void MPU6050::enableInt(bool on)
{
    _int_enable = on;
    this->checkConnection();
    if (on)
        this->writeRegMPU(REG_INTR_EN, INTR_EN_DATA_RDY);
    else
        this->writeRegMPU(REG_INTR_EN, INTR_DISABLE);
}

void MPU6050::enableFIFO()
{
    this->checkConnection();
    this->writeRegMPU(REG_USER_CTRL, USER_CTRL_FIFO_EN | USER_CTRL_FIFO_RST);
}

void MPU6050::setFIFO(uint8_t fifo)
{
    _FIFO = fifo;
    this->checkConnection();
    this->writeRegMPU(REG_FIFO_CFG, _FIFO);
}

void MPU6050::setFreq(uint16_t freq)
{
    _freq = (1000 - freq) / freq;
    this->checkConnection();
    this->writeRegMPU(REG_SAMPLE_RATE_DIV, _freq);
}

void MPU6050::setNumSamples(uint16_t num)
{
    _num_samples = num;
    if (_samples != NULL)
        free((void *)_samples);
    _samples = (float *)calloc(_num_samples, sizeof(float));
}

void MPU6050::checkConnection()
{
    int data = 0;
    Wire.beginTransmission(_edr_mpu);
    data = Wire.endTransmission(true);

    if (data != 0)
        throw MPU6050::Exception(ERRO_1);
    data = this->readRegMPU(REG_WHO_AM_I);

    if (data != WHO_AM_I)
        throw MPU6050::Exception(ERRO_2);
}

void MPU6050::writeRegMPU(int reg, int val)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission(true);
}

uint8_t MPU6050::readRegMPU(uint8_t reg)
{
    uint8_t data;
    Wire.beginTransmission(_edr_mpu);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_edr_mpu, (int)1);
    data = Wire.read();
    return data;
}

uint16_t MPU6050::readTwoRegMPU(uint8_t reg)
{
    uint16_t data;
    Wire.beginTransmission(_edr_mpu);

    Wire.beginTransmission(_edr_mpu);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_edr_mpu, 2);

    data = Wire.read() << 8;
    data |= Wire.read();

    return data;
}

void IRAM_ATTR MPU6050::ISR()
{
    MPU6050::ready = true;
}

float MPU6050::mean()
{
    float mean=0;
    for(int i =0; i <_num_samples ; i++)
        mean += _samples[i]/_num_samples;
    return mean;
}

float MPU6050::variance()
{
    float mean=this->mean();
    float variance=0;
    for(int i =0; i <_num_samples ; i++)
        variance += ((_samples[i]-mean)*(_samples[i]-mean))/(_num_samples -1 );
    return variance;
}
#endif