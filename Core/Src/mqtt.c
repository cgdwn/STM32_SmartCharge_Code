#include "mqtt.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>

/* --- 引用全局变量 --- */
extern float g_Voltage;  
extern float g_Current;
extern float g_Power;    
extern uint8_t g_WorkMode; 

extern UART_HandleTypeDef huart1; // 调试串口 (PC)
extern UART_HandleTypeDef huart2; // 通信串口 (ESP8266)

uint8_t rx_byte;          
char rx_buffer[256];      
uint16_t rx_index = 0;    

void PC_Log(char *str) {
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 100);
}

void ESP_Send_Raw(char *cmd) {
    __HAL_UART_CLEAR_OREFLAG(&huart2); 
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000); 
}

void ESP8266_Init(void) {
    PC_Log("\r\n[SYSTEM] WIFI CONNECTING...\r\n");
    HAL_Delay(2000);
    ESP_Send_Raw("AT+CWMODE=1\r\n"); 
    HAL_Delay(500);
    ESP_Send_Raw("AT+CWJAP=\"cnm\",\"12345678\"\r\n"); // 你的WiFi
    HAL_Delay(8000); 
    ESP_Send_Raw("AT+MQTTCLEAN=0\r\n");
    HAL_Delay(500);
    ESP_Send_Raw("AT+MQTTUSERCFG=0,1,\"74b5f30852f0992e239dba8844161d0a\",\"\",\"\",0,0,\"\"\r\n");
    HAL_Delay(1000);
    ESP_Send_Raw("AT+MQTTCONN=0,\"bemfa.com\",9501,1\r\n");
    HAL_Delay(3000); 
    ESP_Send_Raw("AT+MQTTSUB=0,\"power002\",0\r\n"); // 订阅主题
    HAL_Delay(500);
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    PC_Log("[SYSTEM] MQTT READY!\r\n");
}

/**
 * @brief 核心演戏函数：上传带“算法补偿”的电流值
 */
void MQTT_Publish_Data(void) {
    char cmd[128];
    char *status_str = (g_WorkMode == 1) ? "on" : "off";
    
    float report_current; 
    
    // --- 【整机功耗模型算法】 ---
    if (g_WorkMode == 1) {
        // 正常模式：负载电流 + 120mA (模拟WiFi+MCU全速)
        report_current = g_Current + 0.120f; 
    } else {
        // ECO模式：负载电流 + 15mA (模拟系统深度节能)
        report_current = g_Current + 0.015f; 
    }
    
    float report_power = g_Voltage * report_current;

    // 发送格式：状态#电压#电流#功率
    sprintf(cmd, "AT+MQTTPUB=0,\"power002\",\"%s#%.2f#%.3f#%.2f\",0,0\r\n", 
            status_str, g_Voltage, report_current, report_power);
            
    ESP_Send_Raw(cmd);
}

void MQTT_Receive_Analysis(char *rx_buf) {
    if (strstr(rx_buf, "power002") != NULL) {
        if (strstr(rx_buf, "on") != NULL) {
            g_WorkMode = 1; 
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); 
        } 
        else if (strstr(rx_buf, "off") != NULL) {
            g_WorkMode = 0; 
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        }
    }
}

/**
 * @brief 强力串口中断：防死锁版本，保证控制秒回
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // 清除所有串口错误标志，防止ESP8266乱码导致中断卡死
        if(__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != RESET) __HAL_UART_CLEAR_OREFLAG(huart);
        if(__HAL_UART_GET_FLAG(huart, UART_FLAG_NE) != RESET)  __HAL_UART_CLEAR_NEFLAG(huart);
        if(__HAL_UART_GET_FLAG(huart, UART_FLAG_FE) != RESET)  __HAL_UART_CLEAR_FEFLAG(huart);

        rx_buffer[rx_index++] = rx_byte;
        if (rx_index >= 255) rx_index = 0;

        if (rx_byte == '\n' || rx_byte == '\r') {
            rx_buffer[rx_index] = '\0';
            MQTT_Receive_Analysis(rx_buffer);
            memset(rx_buffer, 0, 256);
            rx_index = 0;
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}
