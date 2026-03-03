#include "ina219.h"
#include "i2c.h"    


float INA219_GetVoltage(void)
{
    uint8_t buf[2]; 
    int16_t raw_value;  
    float real_voltage; 

    HAL_I2C_Mem_Read(&hi2c1, 0x80, 0x02, I2C_MEMADD_SIZE_8BIT, buf, 2, 100);

    raw_value = (buf[0] << 8) | buf[1];

    raw_value >>= 3;

    real_voltage = raw_value * 0.004f;

    return real_voltage;
}

float  INA219_GetCurrent(void)
{
    uint8_t buf[2]; 
    int16_t raw_value;  
    float real_current; 

    HAL_I2C_Mem_Read(&hi2c1, 0x80, 0x04, I2C_MEMADD_SIZE_8BIT, buf, 2, 100);

    raw_value = (buf[0] << 8) | buf[1];

    real_current = raw_value * 0.01f;

    return real_current;
}

void  INA219_Configuration(void)
{
    uint8_t config[2] = {0x39, 0x9F}; 

    HAL_I2C_Mem_Write(&hi2c1, 0x80, 0x00, I2C_MEMADD_SIZE_8BIT, config, 2, 100);
}

void  INA219_Calibration(void)
{
    uint8_t calibration[2] = {0x10, 0x00}; 

    HAL_I2C_Mem_Write(&hi2c1, 0x80, 0x05, I2C_MEMADD_SIZE_8BIT, calibration, 2, 100);
}

void INA219_Init(void)
{
    INA219_Configuration();
    HAL_Delay(100);
    INA219_Calibration();
}
