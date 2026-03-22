/* USER CODE BEGIN Header */
/**
  * @file           : main.c
  * @brief          : 智能监控系统 - 最终复活版
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "ina219.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
// --- [重点] 在这里定义全局变量，分配内存 ---
float g_Voltage = 0.0f;  
float g_Current = 0.0f;  
float g_Power = 0.0f;   
uint8_t g_Status = 0;   
uint8_t g_WorkMode = 1;
// --- 串口接收相关 ---
uint8_t rx_byte;
char rx_buffer[256];
uint16_t rx_index = 0;

// --- CAN总线相关 ---
CAN_TxHeaderTypeDef TxHeader; 
uint8_t TxData[8];             
uint32_t TxMailbox;           
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/**
  * @brief  主函数入口
  */
int main(void)
{
  /* MCU 初始化 */
  HAL_Init();

  /* 配置时钟 */
  SystemClock_Config();

  /* 初始化所有外设 */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init(); // 调试打印
  MX_USART2_UART_Init(); // ESP8266通信

  /* USER CODE BEGIN 2 */
  // --- 1. 业务初始化 ---
  INA219_Init(); 
  OLED_Init();       
  OLED_Clear();      
  OLED_ShowString(0, 0, "MONITOR READY", OLED_8X16);
  OLED_Update();

  // --- 2. CAN 启动配置 ---
  HAL_CAN_Start(&hcan);
  TxHeader.StdId = 0x123;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 8;
  TxHeader.TransmitGlobalTime = DISABLE;
  
  // --- 3. 开启串口2接收中断 ---
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1); 
  /* USER CODE END 2 */

  /* 初始化 FreeRTOS 并启动调度器 */
  MX_FREERTOS_Init();
  osKernelStart();

  while (1)
  {
  }
}

/**
  * @brief 系统时钟配置 (保持你原来的配置即可)
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// --- 核心：串口中断回调函数 ---
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        rx_buffer[rx_index++] = rx_byte;
        
        if (rx_byte == '\n' || rx_index >= 250) {
            rx_buffer[rx_index] = '\0'; 
            
            // 数据交给 mqtt.c 解析
            extern void MQTT_Receive_Analysis(char *rx_buf);
            MQTT_Receive_Analysis(rx_buffer); 
            
            memset(rx_buffer, 0, sizeof(rx_buffer)); 
            rx_index = 0; 
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}
/* USER CODE END 4 */

/**
  * @brief 滴答定时器回调 (用于 HAL_Delay)
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
