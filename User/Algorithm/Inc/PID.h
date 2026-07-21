/*
 * @Date: 2026-07-03 20:39:08
 * @LastEditTime: 2026-07-05 22:33:27
 * @FilePath: \15_倒立摆\User\Algorithm\Inc\PID.h
 * @Description: PID控制算法封装代码头文件
 */
#ifndef __PID_H
#define __PID_H

#include <math.h>
#include <stdint.h>

#define EPSILON 1e-6f

typedef struct {
    float Target;
    float Actual;
    float Out;
    float Kp;
    float Ki;
    float Kd;
    float Error0;
    float Error1;
    float ErrorInt;
    float OutMax;
    float OutMin;
    float ErrorMax;
} PID_t;

void PID_Update(PID_t *pid_handle);

#endif