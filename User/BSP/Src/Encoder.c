/*
 * @Date: 2026-07-02 20:00:33
 * @LastEditTime: 2026-07-21 18:12:12
 * @FilePath: \01_基础驱动代码\User\BSP\Src\Encoder.c
 * @Description: 编码器驱动代码
 */
#include "tim.h"
/**
  * 函    数：获取编码器的增量值
  * 参    数：无
  * 返 回 值：自上此调用此函数后，编码器的增量值
  */
int16_t Encoder_Get(uint8_t n)
{
	/*使用Temp变量作为中继，目的是返回CNT后将其清零*/
	int16_t Temp;
	if(n == 1){
		Temp = __HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SET_COUNTER(&htim3, 0);
		return Temp;
	}
	else if(n == 2){
		Temp = -__HAL_TIM_GET_COUNTER(&htim4);
		__HAL_TIM_SET_COUNTER(&htim4, 0);
		return Temp;
	}
	return 0;
}
