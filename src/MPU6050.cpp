#include <Arduino.h>
#include <Wire.h>
#include "MPU6050.h"

MPU6050::MPU6050(uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, uint16_t num_samples, int edr_mpu, uint8_t gyr_scale, uint8_t acc_scale, uint8_t dlpf, bool int_enable)
{
    _scl_pin = scl_pin;
    _sda_pin = sda_pin;
    _int_pin = int_pin;
    _edr_mpu = edr_mpu;
    _gyr_scale = gyr_scale;
    _acc_scale = acc_scale;
    _dlpf = dlpf;
    _int_enable = int_enable;
    _num_samples = num_samples;
    _samples = NULL;
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
    this->enableInt(_int_enable);
    this->setNumSamples(_num_samples);
    pinMode(_int_pin, INPUT);
}

float *MPU6050::readRawMPU()
{
    this->checkConnection();

    int16_t AcX, AcY, AcZ, GyX, GyY, GyZ, Tmp;
    
    
    int y = 0;
    for (int i = 0; i < _num_samples; i++)
    {
        while (digitalRead(_int_pin) == LOW)
            ;
        Wire.beginTransmission(_edr_mpu);
        Wire.write(REG_AX_H);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU_ADDR, 14);

        AcX = Wire.read() << 8;
        AcX |= Wire.read();
        _samples[y++] = ((float)AcX) / _A_LSB;

        AcY = Wire.read() << 8;
        AcY |= Wire.read();
        _samples[y++] = ((float)AcY) / _A_LSB;

        AcZ = Wire.read() << 8;
        AcZ |= Wire.read();
        _samples[y++] = ((float)AcZ) / _A_LSB;

        Tmp = Wire.read() << 8;
        Tmp |= Wire.read();
        _samples[y++] = Tmp / 340 + 36.53;

        GyX = Wire.read() << 8;
        GyX |= Wire.read();
        _samples[y++] = ((float)GyX) / _G_LSB;

        GyY = Wire.read() << 8;
        GyY |= Wire.read();
        _samples[y++] = ((float)GyY) / _G_LSB;

        GyZ = Wire.read() << 8;
        GyZ |= Wire.read();
        _samples[y++] = ((float)GyZ) / _G_LSB;

        while (digitalRead(_int_pin) == HIGH)
            ;

    }
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

void MPU6050::setNumSamples(uint16_t num)
{
    _num_samples = num;
    if (_samples != NULL)
        free((void *)_samples);
    _samples = (float *)malloc(num * sizeof(float) * 7);
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