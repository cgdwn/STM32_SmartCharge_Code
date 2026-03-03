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
