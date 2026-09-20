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
uint8_t AngleFlag = 0;
uint8_t STFlag = 0;

int16_t AX, AY, AZ, GX, GY, GZ;
uint16_t TimerCount;
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init();
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  OLED_Init(); 
  NRF24L01_Init();
  // HAL_UART_Receive_IT(&huart1, &RxData, 1);
  HAL_UART_Receive_IT(&huart2, &RxData, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(RunFlag){
      LED_ON();
    }
    else{
      LED_OFF();
    }
    KeyNum = Key_GetNum();
    if(KeyNum == 1){
      if(RunFlag == 0){
        PID_Init(&AnglePID);
        PID_Init(&SpeedPID);
        PID_Init(&TurnPID);
        RunFlag = 1;
      }
      else if(RunFlag == 1){
        RunFlag = 0;
      }
    }
    /*OLED显示*/
    OLED_Clear();
		OLED_Printf(0, 0, OLED_6X8, "  Angle");
    OLED_Printf(0, 8, OLED_6X8, "P:%05.2f", AnglePID.Kp);
    OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", AnglePID.Ki);
    OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", AnglePID.Kd);
    OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", AnglePID.Target);
    OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", Angle);
    OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", AnglePID.Out);

    OLED_Printf(50, 0, OLED_6X8, "Speed");
    OLED_Printf(50, 8, OLED_6X8, "%05.2f", SpeedPID.Kp);
    OLED_Printf(50, 16, OLED_6X8, "%05.2f", SpeedPID.Ki);
    OLED_Printf(50, 24, OLED_6X8, "%05.2f", SpeedPID.Kd);
    OLED_Printf(50, 32, OLED_6X8, "%+05.1f", SpeedPID.Target);
    OLED_Printf(50, 40, OLED_6X8, "%+05.1f", AveSpeed);
    OLED_Printf(50, 48, OLED_6X8, "%+05.0f", SpeedPID.Out);

    OLED_Printf(88, 0, OLED_6X8, "Turn");
    OLED_Printf(88, 8, OLED_6X8, "%05.2f", TurnPID.Kp);
    OLED_Printf(88, 16, OLED_6X8, "%05.2f", TurnPID.Ki);
    OLED_Printf(88, 24, OLED_6X8, "%05.2f", TurnPID.Kd);
    OLED_Printf(88, 32, OLED_6X8, "%+05.1f", TurnPID.Target);
    OLED_Printf(88, 40, OLED_6X8, "%+05.1f", DifSpeed);
    OLED_Printf(88, 48, OLED_6X8, "%+05.0f", TurnPID.Out);
		/*OLED更新*/
		OLED_Update();

    if(AngleFlag == 1)
    {
      MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
      /*俯仰角姿态解算*/
      AngleAcc = -atan2(AX, AZ) / 3.14159 * 180;
      AngleGyro = Angle + (GY - 70) / 32768.0 * 2000 * 0.01;   //70为角速度计的大致零漂值
      float Alpha = 0.01;
      Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;       //使用互补滤波进一步减小零漂影响
  
      if(Angle > 50 || Angle < -50){
        RunFlag = 0;
      }
      if(RunFlag){
        AnglePID.Actual = Angle;
        // AnglePID.Target = 0;
        PID_Update(&AnglePID);
        AvePWM = -AnglePID.Out;

        LeftPWM = AvePWM + DifPWM / 2;
        RightPWM = AvePWM - DifPWM / 2;

        if(LeftPWM > 100){LeftPWM = 100;} else if(LeftPWM < -100){LeftPWM = -100;}
        if(RightPWM > 100){RightPWM = 100;} else if(RightPWM < -100){RightPWM = -100;}

        Motor_SetPWM(1, LeftPWM);
        Motor_SetPWM(2, RightPWM);
      }
      else{
        Motor_SetPWM(1, 0);
        Motor_SetPWM(2, 0);
      }
    }

    if(STFlag == 1)
    {
      LeftSpeed = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
      RightSpeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;

      AveSpeed = (LeftSpeed + RightSpeed) / 2;
      DifSpeed = LeftSpeed - RightSpeed;

      if(RunFlag){
        SpeedPID.Actual = AveSpeed;
        PID_Update(&SpeedPID);
        AnglePID.Target = SpeedPID.Out;

        TurnPID.Actual = DifSpeed;
        PID_Update(&TurnPID);
        DifPWM = TurnPID.Out;
      }
    }

    if(NRF24L01_Receive() == 1){
      uint8_t ID = NRF24L01_RxPacket[0];
      if (ID == 0x00 || 0x01) {
        if(ID == 0x01){
          NRF24L01_TxPacket[0] = 0x02;
          NRF24L01_TxPacket[1] = (int8_t)LeftPWM;
          NRF24L01_TxPacket[2] = (int8_t)RightPWM;
          *(float *)&NRF24L01_TxPacket[4] = Angle;
          *(float *)&NRF24L01_TxPacket[8] = LeftSpeed;
          *(float *)&NRF24L01_TxPacket[12] = RightSpeed;
          NRF24L01_Send();
        }

        // int8_t LH = NRF24L01_RxPacket[1];
        int8_t LV = NRF24L01_RxPacket[2];
        int8_t RH = NRF24L01_RxPacket[3];
        // int8_t RV = NRF24L01_RxPacket[4];
        uint8_t KEY = NRF24L01_RxPacket[5];

        SpeedPID.Target = LV / 25.0;
        TurnPID.Target = RH / 25.0;

        if (KEY == 1) {
          if (RunFlag == 0) {
            PID_Init(&AnglePID);
            PID_Init(&SpeedPID);
            PID_Init(&TurnPID);
            RunFlag = 1;
          } 
          else if (RunFlag == 1) {
            RunFlag = 0;
          }
        }
      }
    }
    if(RxFlag == 1){
      char *Tag = strtok(Rx_buffer, ",");
      if(strcmp(Tag, "key") == 0){
        // char *Name = strtok(NULL, ",");
        // char *Action = strtok(NULL, ",");
        
      }
      else if(strcmp(Tag, "slider") == 0){
        // char *Name = strtok(NULL, ",");
        // char *Value = strtok(NULL, ",");

        // if(strcmp(Name, "AngleKp") == 0){
        //   AnglePID.Kp = atof(Value);
        // }
        // else if(strcmp(Name, "AngleKi") == 0){
        //   AnglePID.Ki = atof(Value);
        // }
        // else if(strcmp(Name, "AngleKd") == 0){
        //   AnglePID.Kd = atof(Value);
        // }

        // else if(strcmp(Name, "SpeedKp") == 0){
        //   SpeedPID.Kp = atof(Value);
        // }
        // else if(strcmp(Name, "SpeedKi") == 0){
        //   SpeedPID.Ki = atof(Value);
        // }
        // else if(strcmp(Name, "SpeedKd") == 0){
        //   SpeedPID.Kd = atof(Value);
        // }

        // else if(strcmp(Name, "TurnKp") == 0){
        //   TurnPID.Kp = atof(Value);
        // }
        // else if(strcmp(Name, "TurnKi") == 0){
        //   TurnPID.Ki = atof(Value);
        // }
        // else if(strcmp(Name, "TurnKd") == 0){
        //   TurnPID.Kd = atof(Value);
        // }
      }
      else if(strcmp(Tag, "joystick") == 0){
        // int8_t LH = atoi(strtok(NULL, ","));
        int8_t LV = atoi(strtok(NULL, ","));
        int8_t RH = atoi(strtok(NULL, ","));
        // int8_t RV = atoi(strtok(NULL, ","));
        
        SpeedPID.Target = LV / 25.0;
        TurnPID.Target = RH / 25.0;
      }
      RxFlag = 0;
    }

    // Serial2_Printf("[plot,%f,%f]\r\n", TurnPID.Target, DifSpeed);
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
    // /*识别数据包格式*/
    // if(RxState == 0){
    //   if(RxData == '[' /*&& RxFlag == 0*/){
    //     RxState = 1;
    //     pRxPacket = 0;
    //   }
    // }
    // else if(RxState == 1){
    //   if(RxData == ']'){
    //     RxState = 0;
    //     Rx_buffer[pRxPacket] = '\0';
    //     // RxFlag = 1;
    //   }
    //   else{
    //     Rx_buffer[pRxPacket] = RxData;
    //     pRxPacket++;
    //   }
    // }
    
    // HAL_UART_Receive_IT(&huart1, &RxData, 1);
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  static uint16_t Count0, Count1;
  if(htim->Instance == TIM1){
    Key_Tick();

    Count0++;
    if(Count0 >= 10){
      Count0 = 0;
      AngleFlag = 1;
      // MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
      // /*俯仰角姿态解算*/
      // AngleAcc = -atan2(AX, AZ) / 3.14159 * 180;
      // AngleGyro = Angle + (GY - 70) / 32768.0 * 2000 * 0.01;   //70为角速度计的大致零漂值
      // float Alpha = 0.01;
      // Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;       //使用互补滤波进一步减小零漂影响
  
      // if(Angle > 50 || Angle < -50){
      //   RunFlag = 0;
      // }
      // if(RunFlag){
      //   AnglePID.Actual = Angle;
      //   // AnglePID.Target = 0;
      //   PID_Update(&AnglePID);
      //   AvePWM = -AnglePID.Out;

      //   LeftPWM = AvePWM + DifPWM / 2;
      //   RightPWM = AvePWM - DifPWM / 2;

      //   if(LeftPWM > 100){LeftPWM = 100;} else if(LeftPWM < -100){LeftPWM = -100;}
      //   if(RightPWM > 100){RightPWM = 100;} else if(RightPWM < -100){RightPWM = -100;}

      //   Motor_SetPWM(1, LeftPWM);
      //   Motor_SetPWM(2, RightPWM);
      // }
      // else{
      //   Motor_SetPWM(1, 0);
      //   Motor_SetPWM(2, 0);
      // }
    }
    Count1++;
    if(Count1 >= 50){
      Count1 = 0;
      STFlag = 1;
      // LeftSpeed = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
      // RightSpeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;

      // AveSpeed = (LeftSpeed + RightSpeed) / 2;
      // DifSpeed = LeftSpeed - RightSpeed;

      // if(RunFlag){
      //   SpeedPID.Actual = AveSpeed;
      //   PID_Update(&SpeedPID);
      //   AnglePID.Target = SpeedPID.Out;

      //   TurnPID.Actual = DifSpeed;
      //   PID_Update(&TurnPID);
      //   DifPWM = TurnPID.Out;
      // }
    }

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
