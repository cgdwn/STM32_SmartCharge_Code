#ifndef _INA219_H
#define _INA219_H   

#include "main.h"

void INA219_Init(void); 

float INA219_GetVoltage(void);

float INA219_GetCurrent(void);  

#endif      
 