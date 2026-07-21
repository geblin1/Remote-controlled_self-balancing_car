/*
 * @Date: 2026-07-02 18:14:38
 * @LastEditTime: 2026-07-02 19:08:41
 * @FilePath: \01_基础驱动代码\User\App\Inc\Motor.h
 * @Description: 直流电机驱动头文件
 */

#ifndef __MOTOR_H
#define __MOTOR_H
#include <stdint.h>
void Motor_SetPWM(uint8_t n, int8_t PWM);

#endif
