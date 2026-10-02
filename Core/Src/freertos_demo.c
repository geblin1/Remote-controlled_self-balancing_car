/*
 * @Author: geblin1 3390931275@qq.com
 * @Date: 2026-10-02 19:27:04
 * @LastEditors: geblin1 3390931275@qq.com
 * @LastEditTime: 2026-10-02 22:58:12
 * @FilePath: \平衡车程序\Core\Src\freertos.c
 * @Description: 本文件用于创建/管理FreeRTOS任务（内核位于 Middlewares/FreeRTOS/Source，使用原生FreeRTOS API）
 */
#include "freertos_demo.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "MPU6050.h"
#include "PID.h"
#include <math.h>
#include "Motor.h"
#include "NRF24L01.h"
#include "Encoder.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"
#include <string.h>
#include <stdlib.h>

#define START_TASK_STACK    128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;

#define IMU_TASK_STACK    256
#define IMU_TASK_PRIORITY 5
TaskHandle_t IMU_task_handle;

#define ANGLEPID_TASK_STACK    128
#define ANGLEPID_TASK_PRIORITY 4
TaskHandle_t AnglePID_task_handle;

#define SPEEDPID_TASK_STACK    128
#define SPEEDPID_TASK_PRIORITY 3
TaskHandle_t SpeedPID_task_handle;

#define TURNPID_TASK_STACK    128
#define TURNPID_TASK_PRIORITY 3
TaskHandle_t TurnPID_task_handle;

#define NRF24L01_TASK_STACK    128
#define NRF24L01_TASK_PRIORITY 2
TaskHandle_t NRF24L01_task_handle;

#define BLUETOOTH_TASK_STACK    128
#define BLUETOOTH_TASK_PRIORITY 2
TaskHandle_t BlueTooth_task_handle;

#define OLED_TASK_STACK    384
#define OLED_TASK_PRIORITY 1
TaskHandle_t OLED_task_handle;

#define KEY_TASK_STACK    128
#define KEY_TASK_PRIORITY 2
TaskHandle_t Key_task_handle;

#define LED_TASK_STACK    128
#define LED_TASK_PRIORITY 1
TaskHandle_t LED_task_handle;

/* 任务函数必须是 void (*)(void *) 类型，且函数内部不能返回（必须死循环） */
void start_task(void *argument);
void IMU_task(void *argument);
void AnglePID_task(void *argument);
void SpeedPID_task(void *argument);
void TurnPID_task(void *argument);
void NRF24L01_task(void *argument);
void BlueTooth_task(void *argument);
void OLED_task(void *argument);
void Key_task(void *argument);
void LED_task(void *argument);

/* 以下变量定义在main.c中，供TIM1中断与各任务共用（此处只做外部声明，避免重复定义） */
extern int16_t AX, AY, AZ, GX, GY, GZ;
extern float AngleAcc;   /*加速度计测得的俯仰角*/
extern float AngleGyro;  /*角速度计测得的俯仰角*/
extern float Angle;      /*滤波后得到的俯仰角*/

extern uint8_t KeyNum, RunFlag;

extern int16_t LeftPWM, RightPWM;
extern int16_t AvePWM, DifPWM;

extern float LeftSpeed, RightSpeed;
extern float AveSpeed, DifSpeed;

extern PID_t AnglePID;
extern PID_t SpeedPID;
extern PID_t TurnPID;

extern uint8_t RxFlag;
/*串口数据接收缓冲变量*/
extern uint8_t RxData;
extern char Rx_buffer[100];

void freertos_start()
{
    xTaskCreate(start_task, 
        (char *)"start_task", 
        START_TASK_STACK, 
        NULL, 
        START_TASK_PRIORITY, 
        (TaskHandle_t *)&start_task_handle);
    vTaskStartScheduler();
}

void start_task(void *argument)
{
    (void)argument;

    taskENTER_CRITICAL();
    xTaskCreate(IMU_task, 
        (char *)"IMU_task", 
        IMU_TASK_STACK, 
        NULL, 
        IMU_TASK_PRIORITY, 
        (TaskHandle_t *)&IMU_task_handle);
    xTaskCreate(AnglePID_task, 
        (char *)"Angle_task", 
        ANGLEPID_TASK_STACK, 
        NULL, 
        ANGLEPID_TASK_PRIORITY, 
        (TaskHandle_t *)&AnglePID_task_handle);
    xTaskCreate(SpeedPID_task, 
        (char *)"SpeedPID_task", 
        SPEEDPID_TASK_STACK, 
        NULL, 
        SPEEDPID_TASK_PRIORITY, 
        (TaskHandle_t *)&SpeedPID_task_handle);
    xTaskCreate(TurnPID_task, 
        (char *)"TurnPID_task", 
        TURNPID_TASK_STACK, 
        NULL, 
        TURNPID_TASK_PRIORITY, 
        (TaskHandle_t *)&TurnPID_task_handle);
    xTaskCreate(NRF24L01_task, 
        (char *)"NRF24L01_task", 
        NRF24L01_TASK_STACK, 
        NULL, 
        NRF24L01_TASK_PRIORITY, 
        (TaskHandle_t *)&NRF24L01_task_handle);
    xTaskCreate(BlueTooth_task, 
        (char *)"BlueTooth_task", 
        BLUETOOTH_TASK_STACK, 
        NULL, 
        BLUETOOTH_TASK_PRIORITY, 
        (TaskHandle_t *)&BlueTooth_task_handle);
    xTaskCreate(OLED_task, 
        (char *)"OLED_task", 
        OLED_TASK_STACK, 
        NULL, 
        OLED_TASK_PRIORITY, 
        (TaskHandle_t *)&OLED_task_handle);
    xTaskCreate(Key_task, 
        (char *)"Key_task", 
        KEY_TASK_STACK, 
        NULL, 
        KEY_TASK_PRIORITY, 
        (TaskHandle_t *)&Key_task_handle);
    xTaskCreate(LED_task, 
        (char *)"LED_task", 
        LED_TASK_STACK, 
        NULL, 
        LED_TASK_PRIORITY, 
        (TaskHandle_t *)&LED_task_handle);
    taskEXIT_CRITICAL();

    vTaskDelete(NULL);
}

/**
 * @description: 姿态解算，获取平衡车俯仰角
 * @param {void} *argument
 * @return {*}
 */
void IMU_task(void *argument)
{
    (void)argument;

    while(1)
    {
        MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
        /*俯仰角姿态解算*/
        AngleAcc = -atan2(AX, AZ) / 3.14159 * 180;
        AngleGyro = Angle + (GY - 70) / 32768.0 * 2000 * 0.01;   //70为角速度计的大致零漂值
        float Alpha = 0.01;
        Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;       //使用互补滤波进一步减小零漂影响

        vTaskDelay(pdMS_TO_TICKS(10));   /* 10ms周期，与上面的0.01系数对应 */
    }
}

/**
 * @description: 角度环PID控制，控制平衡车平衡
 * @param {void} *argument
 * @return {*}
 */
void AnglePID_task(void *argument)
{
    (void)argument;

    while(1)
    {
        /* TODO: 角度环PID计算 */
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
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @description: 速度环控制，通过控制平衡车倾斜角控制速度
 * @param {void} *argument
 * @return {*}
 */
void SpeedPID_task(void *argument)
{
    (void)argument;

    while(1)
    {
        /* TODO: 速度环PID计算 */
        LeftSpeed = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
        RightSpeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;

        AveSpeed = (LeftSpeed + RightSpeed) / 2;
        

        if(RunFlag){
            SpeedPID.Actual = AveSpeed;
            PID_Update(&SpeedPID);
            AnglePID.Target = SpeedPID.Out;           
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @description: 转向环控制，精确控制平衡车前进方向
 * @param {void} *argument
 * @return {*}
 */
void TurnPID_task(void *argument)
{
    (void)argument;

    while(1)
    {
        /* TODO: 转向环PID计算 */
        DifSpeed = LeftSpeed - RightSpeed;

        TurnPID.Actual = DifSpeed;
        PID_Update(&TurnPID);
        DifPWM = TurnPID.Out;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @description: NRF24L01通信任务，实现遥控器遥控
 * @param {void} *argument
 * @return {*}
 */
void NRF24L01_task(void *argument)
{
    (void)argument;
    while(1)
    {
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

        vTaskDelay(pdMS_TO_TICKS(10));   /* 必须延时让出CPU，否则同优先级任务会被饿死 */
    }
}

/**
 * @description: 蓝牙串口数据包解析任务
 * @param {void} *argument
 * @return {*}
 */
void BlueTooth_task(void *argument)
{
    (void)argument;
    while(1)
    {
        if(RxFlag == 1){
            char *Tag = strtok(Rx_buffer, ",");
            if(strcmp(Tag, "joystick") == 0){
                int8_t LV = atoi(strtok(NULL, ","));
                int8_t RH = atoi(strtok(NULL, ","));
    
                SpeedPID.Target = LV / 25.0;
                TurnPID.Target = RH / 25.0;
            }
            RxFlag = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(10));   /* 必须延时让出CPU */
    }
}

/**
 * @description: OLED显示任务
 * @param {void} *argument
 * @return {*}
 */
void OLED_task(void *argument)
{
    (void)argument;
    while(1)
    {
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

        vTaskDelay(pdMS_TO_TICKS(100));   /* 显示刷新无需太快，同时让出CPU */
    }
}

/**
 * @description: 按键任务，用来控制平衡车启停
 * @param {void} *argument
 * @return {*}
 */
void Key_task(void *argument)
{
    (void)argument;
    while(1)
    {
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

        vTaskDelay(pdMS_TO_TICKS(20));   /* 必须延时让出CPU */
    }
}

/**
 * @description: LED指示灯任务，亮起表示小车运行
 * @param {void} *argument
 * @return {*}
 */
void LED_task(void *argument)
{
    (void)argument;
    while(1)
    {
        if(RunFlag){
            LED_ON();
        }
        else{
            LED_OFF();
        }

        vTaskDelay(pdMS_TO_TICKS(100));   /* 指示任务无需高速刷新，让出CPU */
    }
}