#include "mqtt.h"
#include "usart.h"
#include "gpio.h"       // 必须包含这个，才能控制 GPIO 灯
#include <string.h>
#include <stdio.h>

/* --- 1. 引用 main.c 里的监测变量 --- */
extern float g_Voltage; 
extern float g_Current;
extern float g_Power;

// 新增：引入工作模式变量，用来告诉 freertos.c 现在的状态
extern uint8_t g_WorkMode; 

extern UART_HandleTypeDef huart1; // 调试串口
extern UART_HandleTypeDef huart2; // ESP8266串口

/**
 * @brief 打印日志到电脑串口(UART1)
 */
void PC_Log(char *str) {
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 100);
}

/**
 * @brief 发送指令到ESP8266(UART2)
 */
void ESP_Send_Raw(char *cmd) {
    __HAL_UART_CLEAR_OREFLAG(&huart2); 
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000); 
}

/**
 * @brief ESP8266 初始化：接入巴法云
 */
void ESP8266_Init(void) {
    PC_Log("\r\n[SYSTEM] STARTING MQTT HANDSHAKE...\r\n");

    HAL_Delay(2000); // 等待模块稳定

    // 1. 复位连接
    ESP_Send_Raw("AT+MQTTCLEAN=0\r\n");
    HAL_Delay(500);

    // 2. 配置巴法云私钥 (根据你的控制台修改)
    ESP_Send_Raw("AT+MQTTUSERCFG=0,1,\"74b5f30852f0992e239dba8844161d0a\",\"\",\"\",0,0,\"\"\r\n");
    HAL_Delay(1000);

    // 3. 连接巴法云
    ESP_Send_Raw("AT+MQTTCONN=0,\"bemfa.com\",9501,0\r\n");
    HAL_Delay(3000); 

    // 4. 订阅主题
    ESP_Send_Raw("AT+MQTTSUB=0,\"haochi001\",0\r\n");
    HAL_Delay(500);
    
    PC_Log("[SYSTEM] MQTT READY! MONITORING ONLY...\r\n");
}

/**
 * @brief 核心：上传 V, I, P 数据到云端
 * 格式：#电压#电流#功率#
 */
void MQTT_Publish_Data(void) {
    char cmd[128];
    sprintf(cmd, "AT+MQTTPUB=0,\"haochi001\",\"#%.2f#%.3f#%.2f\",0,0\r\n", 
            g_Voltage, g_Current, g_Power);
    ESP_Send_Raw(cmd);
}

/**
 * @brief 核心修改：指令解析 (控制灯 + 切换模式)
 */
void MQTT_Receive_Analysis(char *rx_buf) {
    // 只要收到包含主题的数据
    if (strstr(rx_buf, "haochi001") != NULL) {
        
        // 1. 手机点“开” -> 正常模式，点亮板子上的灯
        if (strstr(rx_buf, "on") != NULL) {
            g_WorkMode = 1; // 标记为正常工作模式
            
            // 点亮核心板自带的 PC13 绿色指示灯 (通常是低电平亮)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); 
            
            PC_Log(">>> CLOUD: NORMAL MODE (LED ON) <<<\r\n");
        } 
        
        // 2. 手机点“关” -> 低功耗模式，熄灭板子上的灯
        else if (strstr(rx_buf, "off") != NULL) {
            g_WorkMode = 0; // 标记为低功耗模式
            
            // 熄灭核心板自带的 PC13 绿色指示灯 (高电平灭)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
            
            PC_Log(">>> CLOUD: ECO MODE (LED OFF) <<<\r\n");
        }
    }
}
