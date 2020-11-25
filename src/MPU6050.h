#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <Arduino.h>
#include <exception>
#include "statistic.h"

#define MPU_ADDR 0x68

/*
 *******************************************************************************
 *                              Register Symbols                               *
 *******************************************************************************
*/

// BIT_ masks
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)

// [R] Accelerometer registers (X,Y,Z)
#define REG_AX_L 0x3C
#define REG_AX_H 0x3B
#define REG_AY_L 0x3E
#define REG_AY_H 0x3D
#define REG_AZ_L 0x40
#define REG_AZ_H 0x3F

// [R] Gyroscope registers (X,Y,Z)
#define REG_GX_L 0x44
#define REG_GX_H 0x43
#define REG_GY_L 0x46
#define REG_GY_H 0x45
#define REG_GZ_L 0x48
#define REG_GZ_H 0x47

// [R+W] Accelerometer configuration register + mode masks
#define REG_A_CFG 0x1C
#define A_CFG_2G 0x0
#define A_CFG_4G BIT_3
#define A_CFG_8G BIT_4
#define A_CFG_16G (BIT_3 | BIT_4)

// [R+W] Gyroscope configuration register + mode masks
#define REG_G_CFG 0x1B
#define G_CFG_250 0x0
#define G_CFG_500 BIT_3
#define G_CFG_1000 BIT_4
#define G_CFG_2000 (BIT_3 | BIT_4)

// [R+W] Power management (1) register + mode masks
#define REG_PWR_MGMT_1 0x6B
#define PWR_MGMT_1_RESET BIT_7
#define PWR_MGMT_1_SLEEP BIT_6

// [R+W] FIFO configuration register + mode masks
#define REG_FIFO_CFG 0x23
#define FIFO_CFG_TEMP BIT_7
#define FIFO_CFG_GX BIT_6
#define FIFO_CFG_GY BIT_5
#define FIFO_CFG_GZ BIT_4
#define FIFO_CFG_AXYZ BIT_3

// [R] FIFO count registers
#define REG_FIFO_COUNT_L 0x73
#define REG_FIFO_COUNT_H 0x72

// [R/W] FIFO output register
#define REG_FIFO 0x74

// [R+W] Sample-rate divider register
#define REG_SAMPLE_RATE_DIV 0x19

// [R+W] DLFP configuration register + masks
#define REG_DLPF_CFG 0x1A
#define DLPF_CFG_FILTER_0 0x0 // A{260Hz,0.0ms} G{256Hz 0.98ms} Fs=8kHz
#define DLPF_CFG_FILTER_1 0x1 // A{184Hz,2.0ms} G{188Hz 1.9ms}  Fs=1kHz
#define DLPF_CFG_FILTER_2 0x2 // A{94Hz, 3.0ms} G{98Hz  2.8ms}  Fs=1kHz
#define DLPF_CFG_FILTER_3 0x3 // A{44Hz, 4.9ms} G{42Hz, 4.8ms}  Fs=1kHz
#define DLPF_CFG_FILTER_4 0x4 // A{21Hz, 8.5ms} G{20Hz, 8.3ms}  Fs=1kHz
#define DLPF_CFG_FILTER_5 0x5 // A{10Hz,13.8ms} G{10Hz,13.4ms}  Fs=1kHz
#define DLPF_CFG_FILTER_6 0x6 // A{ 5Hz,19.0ms} G{ 5Hz,18.6ms}  Fs=1kHz

// [R+W] Interrupt enable register + masks
#define REG_INTR_EN 0x38
#define INTR_EN_DATA_RDY BIT_0
#define INTR_EN_FIFO_OFL BIT_4
#define INTR_DISABLE 0x00

// [R+W] Interrupt configuration register + masks
#define REG_INTR_CFG 0x37
#define INTR_CFG_ACTIVE_LOW BIT_7
#define INTR_CFG_OPEN_DRAIN BIT_6
#define INTR_CFG_LATCHING BIT_5
#define INTR_CFG_ANY_CLR BIT_4
#define INTR_CFG_FSYNC_LVL BIT_3
#define INTR_CFG_FSYNC_EN BIT_2
#define INTR_CFG_FSYNC_BYPASS BIT_1

// [R] Interrupt status register
#define REG_INTR_STATUS 0x3A

// [R+W] User control register + masks
#define REG_USER_CTRL 0x6A
#define USER_CTRL_FIFO_EN BIT_6
#define USER_CTRL_FIFO_RST BIT_2

//[R] Check register
#define REG_WHO_AM_I 0x75
#define WHO_AM_I 0x68

#define ERRO_1 "there is nothing in address"
#define ERRO_2 "MPU don't gve the correct answer"
#define ERRO_3 "Please learn how to use the library"
#define ERRO_4 "It's not reading "
class MPU6050 : public Statisc
{
public:
    class Exception : public std::exception
    {
        std::string _msg;

    public:
        Exception(const std::string &msg) : _msg(msg) {}

        virtual const char *what() const noexcept override
        {
            return _msg.c_str();
        }
    };
    
private:
    uint8_t _sda_pin;
    uint8_t _scl_pin;
    uint8_t _int_pin;
    int8_t _edr_mpu;
    int8_t _gyr_scale;
    int8_t _acc_scale;
    int8_t _dlpf;
    int8_t _FIFO;
    uint8_t _freq;
    uint16_t _num_samples;
    float *_samples;
    bool _int_enable;
    float _A_LSB;
    float _G_LSB;
    int _i;
    bool _reading; 

public:
    MPU6050(uint8_t int_pin,
            uint8_t sda_pin,
            uint8_t scl_pin,
            uint16_t freq = 250,
            uint16_t num_samples = 1000,
            int edr_mpu = MPU_ADDR,
            int8_t gyr_scale = G_CFG_2000,
            int8_t acc_scale = A_CFG_2G,
            int8_t dlpf = DLPF_CFG_FILTER_1,
            int8_t fifo = FIFO_CFG_GZ);
    
    void init();
    
    void starRead();
    
    void reading();
    
    bool isReadDone();
    
    float *read();
    
    Statisc::Data statisticRead();

    /**
     * @brief Set the Gyro Scale object
     * FS_SEL   | Full Scale Range  | LSB Sensitivity   | Use define 
     *  0       |   ± 250 °/s       |   131 LSB/°/s     |   G_CFG_250
     *  1       |   ± 500 °/s       |   65.5 LSB/°/s    |   G_CFG_500
     *  2       |   ± 1000 °/s      |   32.8 LSB/°/s    |   G_CFG_1000
     *  3       |   ± 2000 °/s      |   16.4 LSB/°/s    |   G_CFG_2000
     * 
     * @param scale 
     */
    void setGyroScale(uint8_t scale);
    
    /**
     * @brief Set the Accel Scale object
     * FS_SEL   | Full Scale Range  | LSB Sensitivity   |  Use define 
     *  0       |   ± 2g            |   16384LSB/g      |   A _CFG_2G
     *  1       |   ± 4g            |   8192 LSB/g      |   A_CFG_4G
     *  2       |   ± 8g            |   4096 LSB/g      |   A_CFG_8G
     *  3       |   ± 16g           |   2048 LSB/g      |   A_CFG_16G
     * 
     * @param scale 
     */
    void setAccelScale(uint8_t scale);
    
    /**
     * @brief Set the DLPF object
     * DLPF_CFG |   Accelerometer(Fs= 1kHz)         |   Gyroscope                                       |   Use define
     *          |   Bandwidth (Hz)  |   Delay (ms)  |   Bandwidth (Hz)  |   Delay (ms)  |   Fs (kHz)    |   
     *  0       |   260             |   0           |   256             |   0.98        |   8           |   DLPF_CFG_FILTER_0
     *  1       |   184             |   2.0         |   188             |   1.9         |   1           |   DLPF_CFG_FILTER_1
     *  2       |   94              |   3.0         |   98              |   2.8         |   1           |   DLPF_CFG_FILTER_2
     *  3       |   44              |   4.9         |   42              |   4.8         |   1           |   DLPF_CFG_FILTER_3
     *  4       |   21              |   8.5         |   20              |   8.3         |   1           |   DLPF_CFG_FILTER_4
     *  5       |   10              |   13.8        |   10              |   13.4        |   1           |   DLPF_CFG_FILTER_5
     *  6       |   5               |   19.0        |   5               |   18.6        |   1           |   DLPF_CFG_FILTER_6
     *  7       |RESERVED                           |RESERVED                           |   8           |   
     * 
     * @param dlpf
     * 
     */
    void setDLPF(uint8_t dlpf);

    /**
     * @brief Enable/Disable interupitons 
     *  Register (Hex)  |   Register(Decimal)   |Bit7|Bit6|Bit5|    Bit4        |   Bit3        |Bit2|Bit1|Bit0         |
     *  38              |   56                  |    | -  |    |FIFO_OFLOW_EN   |I2C_MST_INT_EN | -  | -  |DATA_RDY_EN  |
     * FIFO_OFLOW_EN:    When  set  to  1,  this  bit enables  a  FIFO  buffer  overflow  to  generate  an interrupt.
     * I2C_MST_INT_EN:   When  set  to  1,  this  bit enables  any  of  the  I2C  Masterinterrupt  sources  to generate an interrupt.
     * DATA_RDY_EN:      When set to 1, this bit enables the Data Ready interrupt, which occurs each time a write operation to all of the sensor registers has been completed.
     * 
     * @param on 
     */
    void enableInt(bool on=true);
    
    void setNumSamples(uint16_t num);

    /**
     * @brief Enable FIFO
     *  Register (Hex)  |   Register(Decimal)   |Bit7|  Bit6    |   Bit5    |   Bit4    |Bit3|  Bit2        |   Bit1        |   Bit0        |
     *  6A              |   106                 | -  |FIFO_EN   |I2C_MST_EN |I2C_IF_DIS | -  |FIFO_RESET    |I2C_MST_RESET  |SIG_COND_RESET |
     * FIFO_EN:         When set to 1, this bit enables FIFO operations.When this  bit  is cleared  to  0,  the  FIFO  buffer is  disabled.
     *                  The  FIFO  buffer cannot be written to or read from while disabled.
     *                  The  FIFO  buffer’sstatedoes  not  change  unless the  MPU-60X0is  power cycled.
     * I2C_MST_EN:      When set to 1, this bit enablesI2C Master Mode. When this  bit  is cleared  to  0, the auxiliary  I2C  bus  lines  (AUX_DA  and AUX_CL) are logically driven by the primary I2C bus (SDA and SCL).
     * I2C_IF_DIS:      MPU-6000:When  set  to  1,  this  bit disablesthe  primary  I2C  interface  and enablesthe SPI interfaceinstead.MPU-6050: Always write this bit as zero.
     * FIFO_RESET:      This bit resetsthe FIFO  bufferwhen set to 1 while FIFO_EN equals  0. This bit automatically clears to 0 after the reset has been triggered.
     * I2C_MST_RESET:   This  bit  resets  the  I2C Master  when  set  to  1while I2C_MST_EN equals 0.This bit automatically clears to 0 afterthe reset has been triggered.
     * SIG_COND_RESET:  When  set  to  1,  this  bit  resetsthe  signal  paths  for  all  sensors  (gyroscopes, accelerometers,  and  temperature  sensor).  This operation will  also  clear  the sensor  registers.This  bit  automatically  clears  to  0  after  the  reset  has  been triggered. When  resetting  only  the  signal  path  (and  not  the  sensor  registers),  please use Register 104, SIGNAL_PATH_RESET.
     */
    void enableFIFO();
    
    /**
     * @brief Set FIFO
     *  Register (Hex) |    Register(Decimal)   |   Bit7        |   Bit6        |   Bit5        |   Bit4        |   Bit3        |   Bit2        |   Bit1        |    Bit0       |
     *  23             |    35                  |TEMP_ FIFO_EN  |XG_ FIFO_EN    |YG_ FIFO_EN    |ZG_ FIFO_EN    |ACCEL_FIFO_EN  |SLV2_FIFO_EN   |SLV1_FIFO_EN   |SLV0_FIFO_EN   |
     * @param fifo 
     */
    void setFIFO(uint8_t fifo);

    /**
     * @brief Set the Freq object
     *  1~1000 Hz
     * @param freq 
     */
    void setFreq(uint16_t freq);

private:
    
    void checkConnection();
    void writeRegMPU(int reg, int val);
    uint8_t readRegMPU(uint8_t reg);
    uint16_t readTwoRegMPU(uint8_t reg);
    static bool ready;
    static void IRAM_ATTR ISR();

};

#endif