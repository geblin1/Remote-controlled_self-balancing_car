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
#include "stm32f1xx_hal_tim.h"
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
uint8_t KeyNum = 0;
uint8_t Num = 0;

int16_t AX, AY, AZ, GX, GY, GZ;
uint16_t Count = 0;

uint8_t a = 123;
float x, y3, y2;
uint8_t RxData;
uint8_t RxFlag = 0;
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
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init();
  HAL_TIM_Base_Start_IT(&htim1);
  OLED_Init(); 
  // HAL_UART_Receive_IT(&huart1, &RxData, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    OLED_Printf(0, 0, OLED_8X16, "%+06d", AX);
    OLED_Printf(0, 16, OLED_8X16, "%+06d", AY);
    OLED_Printf(0, 32, OLED_8X16, "%+06d", AZ);
    OLED_Printf(64, 0, OLED_8X16, "%+06d", GX);
    OLED_Printf(64, 16, OLED_8X16, "%+06d", GY);
    OLED_Printf(64, 32, OLED_8X16, "%+06d", GZ);
    OLED_Printf(0, 48, OLED_8X16, "C:%4d", Count);
    OLED_Update();
    // if(RxFlag == 1){
    //   char *Tag = strtok(Rx_buffer, ",");
    //   if(strcmp(Tag, "key") == 0){
    //     char *Name = strtok(NULL, ",");
    //     char *Action = strtok(NULL, ",");
    //     if(strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0){
    //       printf("key,1,up\r\n");
    //     }
    //     else if(strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0){
    //       printf("key,2,down\r\n");
    //     }
    //   }
    //   else if(strcmp(Tag, "slider") == 0){
    //     char *Name = strtok(NULL, ",");
    //     char *Value = strtok(NULL, ",");
    //     if(strcmp(Name, "1") == 0){
    //       uint8_t IntValue = atoi(Value);
    //       printf("slider,1,%d\r\n", IntValue);
    //     }
    //     else if(strcmp(Name, "2") == 0){
    //       float FloatValue = atof(Value);
    //       printf("slider,2,%f\r\n", FloatValue);
    //     }
    //   }
    //   else if(strcmp(Tag, "joystick") == 0){
    //     int8_t LH = atoi(strtok(NULL, ","));
    //     int8_t LV = atoi(strtok(NULL, ","));
    //     int8_t RH = atoi(strtok(NULL, ","));
    //     int8_t RV = atoi(strtok(NULL, ","));
    //     printf("joystick,%d,%d,%d,%d\r\n", LH, LV, RH, RV);
    //   }
    //   RxFlag = 0;
    // }
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
    HAL_UART_Receive_IT(&huart1, &RxData, 1);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim->Instance == TIM1){
    MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ); 
    Key_Tick();
    Count = __HAL_TIM_GET_COUNTER(&htim1);
  }
}
/* USER CODE END 4 */

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
