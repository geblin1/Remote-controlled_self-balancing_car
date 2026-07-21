/*
 * @Date: 2026-07-02 18:14:30
 * @LastEditTime: 2026-07-21 17:50:44
 * @FilePath: \01_基础驱动代码\User\App\Src\Motor.c
 * @Description: 直流电机驱动代码
 */
#include "PWM.h"
#include "gpio.h"
#include "main.h"
#include <stdint.h>
/**
  * 函    数：直流电机设置速度
  * 参    数：PWM 要设置的速度，范围：-100~100
  * 返 回 值：无
  */
void Motor_SetPWM(uint8_t n, int8_t PWM)
{
	if(n == 1){
		if (PWM >= 0)							//如果设置正转的速度值
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);	//PB12置高电平
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);	//PB13置低电平，设置方向为正转
			PWM_SetCompare1(PWM);				//PWM设置为速度值
		}
		else									//否则，即设置反转的速度值
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);	//PB12置低电平
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);	//PB13置高电平，设置方向为反转
			PWM_SetCompare1(-PWM);			//PWM设置为负的速度值，因为此时速度值为负数，而PWM只能给正数
		}
	}
	else if(n == 2){
		if (PWM >= 0)							//如果设置正转的速度值
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	//PB12置高电平
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);	//PB13置低电平，设置方向为正转
			PWM_SetCompare2(PWM);				//PWM设置为速度值
		}
		else									//否则，即设置反转的速度值
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);	//PB12置低电平
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);	//PB13置高电平，设置方向为反转
			PWM_SetCompare2(-PWM);			//PWM设置为负的速度值，因为此时速度值为负数，而PWM只能给正数
		}
	}
}
