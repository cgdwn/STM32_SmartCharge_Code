/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Sensor_TaskHandle;
osThreadId CAN_TaskHandle;
osThreadId MQTT_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartSensor_Task(void const * argument);
void StartCAN_Task(void const * argument);
void StartMQTT_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Sensor_Task */
  osThreadDef(Sensor_Task, StartSensor_Task, osPriorityNormal, 0, 256);
  Sensor_TaskHandle = osThreadCreate(osThread(Sensor_Task), NULL);

  /* definition and creation of CAN_Task */
  osThreadDef(CAN_Task, StartCAN_Task, osPriorityAboveNormal, 0, 128);
  CAN_TaskHandle = osThreadCreate(osThread(CAN_Task), NULL);

  /* definition and creation of MQTT_Task */
  osThreadDef(MQTT_Task, StartMQTT_Task, osPriorityNormal, 0, 512);
  MQTT_TaskHandle = osThreadCreate(osThread(MQTT_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartSensor_Task */
/**
  * @brief  Function implementing the Sensor_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSensor_Task */
void StartSensor_Task(void const * argument)
{
  /* USER CODE BEGIN StartSensor_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartSensor_Task */
}

/* USER CODE BEGIN Header_StartCAN_Task */
/**
* @brief Function implementing the CAN_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCAN_Task */
void StartCAN_Task(void const * argument)
{
  /* USER CODE BEGIN StartCAN_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartCAN_Task */
}

/* USER CODE BEGIN Header_StartMQTT_Task */
/**
* @brief Function implementing the MQTT_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMQTT_Task */
void StartMQTT_Task(void const * argument)
{
  /* USER CODE BEGIN StartMQTT_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartMQTT_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

