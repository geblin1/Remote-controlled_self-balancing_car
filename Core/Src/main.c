/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "LED.h"
#include <stdint.h>
#include <stdio.h>
#include "Key.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "MPU6050.h"
#include "Motor.h"
#include "PID.h"
#include "Encoder.h"
#include "Serial.h"
#include "NRF24L01.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_demo.h"
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

/* USER CODE BEGIN PV */

int16_t AX, AY, AZ, GX, GY, GZ;
/*加速度计测得的俯仰角*/
float AngleAcc;
/*角速度计测得的俯仰角*/
float AngleGyro;
/*滤波后得到的俯仰角*/
float Angle;

uint8_t KeyNum, RunFlag;

int16_t LeftPWM, RightPWM;
int16_t AvePWM, DifPWM;

float LeftSpeed, RightSpeed;
float AveSpeed, DifSpeed;

PID_t AnglePID = {
  .Kp = 4,
  .Ki = 0.2,
  .Kd = 6,
  .OutMax = 100,
  .OutMin = -100,
  .ErrorMax = 100,
};
PID_t SpeedPID = {
  .Kp = 2.5,
  .Ki = 0,
  .Kd = 0,
  .OutMax = 20,
  .OutMin = -20,
  .ErrorMax = 100,
};
PID_t TurnPID = {
  .Kp = 2,
  .Ki = 4,
  .Kd = 0,
  .OutMax = 50,
  .OutMin = -50,
  .ErrorMax = 100,
};

uint8_t RxFlag = 0;
/*串口数据接收缓冲变量*/
uint8_t RxData;
char Rx_buffer[100];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  OLED_Init(); 
  NRF24L01_Init();
  // HAL_UART_Receive_IT(&huart1, &RxData, 1);
  HAL_UART_Receive_IT(&huart2, &RxData, 1);

  freertos_start();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
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

/**
  * @brief  任务栈溢出钩子函数，当FreeRTOS检测到任务栈溢出时被调用
  *         （由FreeRTOSConfig.h中的configCHECK_FOR_STACK_OVERFLOW = 2启用）
  * 参    数：xTask 发生溢出的任务句柄，pcTaskName 任务名称
  * 返 回 值：无
  */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;
  (void)pcTaskName;
  taskDISABLE_INTERRUPTS();
  while (1)
  {
    LED_ON();
    HAL_Delay(500);
    LED_OFF();
    HAL_Delay(500);
  }
}

/**
  * @brief  Redirect printf to UART1 (via _write syscall for ARM GCC)
  * @param  file: 文件描述符
  * @param  ptr:  数据缓冲区指针
  * @param  len:  数据长度
  * @retval 实际发送的字节数
  */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  static uint8_t RxState = 0;
  static uint8_t pRxPacket = 0;
  if(huart->Instance == USART1){

  }
  else if(huart->Instance == USART2){
    /*识别数据包格式*/
    if(RxState == 0){
      if(RxData == '[' && RxFlag == 0){
        RxState = 1;
        pRxPacket = 0;
      }
    }
    else if(RxState == 1){
      if(RxData == ']'){
        RxState = 0;
        Rx_buffer[pRxPacket] = '\0';
        RxFlag = 1;
      }
      else{
        Rx_buffer[pRxPacket] = RxData;
        pRxPacket++;
      }
    }
    HAL_UART_Receive_IT(&huart2, &RxData, 1);
  }
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  /* TIM1为1ms时基(中断优先级1)，在此做按键扫描：Key_Tick()内部每20次判定一次按键（20ms去抖），
     且不调用任何FreeRTOS API，因此在中断中调用是安全的 */
  if (htim->Instance == TIM1)
  {
    Key_Tick();
  }
  
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
