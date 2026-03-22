#ifndef __MQTT_H
#define __MQTT_H

#include "main.h"

// 函数声明
void ESP8266_Init(void);
void MQTT_Publish_Data(void);
void MQTT_Receive_Analysis(char *rx_buf);
void ESP_Send_Raw(char *cmd);
void PC_Log(char *str);

#endif
