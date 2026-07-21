/*
 * @Date: 2026-07-02 18:14:27
 * @LastEditTime: 2026-07-21 19:46:03
 * @FilePath: \01_基础驱动代码\User\BSP\Src\PWM.c
 * @Description: PWM驱动代码
 */
#include "gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare1(uint16_t Compare)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Compare);
}
void PWM_SetCompare2(uint16_t Compare)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Compare);
}
