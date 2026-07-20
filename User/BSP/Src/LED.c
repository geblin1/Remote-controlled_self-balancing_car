/*
 * @Date: 2026-06-07 17:55:48
 * @LastEditTime: 2026-06-07 20:05:12
 * @FilePath: \OLED_FreeRTOS\Core\Src\LED.c
 * @Description: 这是一个LED驱动文件，后缀ON表示灯亮，后缀OFF表示灯灭，后缀Turn表示改变灯的状态
 */
#include "LED.h"
#include "gpio.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"
void LED_ON(){
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
}
void LED_OFF(){
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
}
void LED_Turn(){
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
}
