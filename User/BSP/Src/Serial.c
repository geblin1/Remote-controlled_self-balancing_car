/*
 * @Date: 2026-07-21 18:34:34
 * @LastEditTime: 2026-07-21 18:37:13
 * @FilePath: \01_基础驱动代码\User\BSP\Src\Serial.c
 * @Description: 串口打印驱动代码，1为有线串口，2为蓝牙串口
 */
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
  * 函    数：串口1打印，使用方法与printf相同
  * 参    数：format 格式化字符串，后跟可变参数列表
  * 返 回 值：无
  */
void Serial1_Printf(const char *format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
}

/**
  * 函    数：串口2打印，使用方法与printf相同
  * 参    数：format 格式化字符串，后跟可变参数列表
  * 返 回 值：无
  */
void Serial2_Printf(const char *format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
}