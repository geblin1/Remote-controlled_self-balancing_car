/*
 * @Date: 2026-07-03 20:38:48
 * @LastEditTime: 2026-07-05 22:35:01
 * @FilePath: \15_倒立摆\User\Algorithm\Src\PID.c
 * @Description: PID控制算法封装代码
 */
#include "PID.h"

void PID_Init(PID_t *p){
    p->Target = 0;
    p->Actual = 0;
    p->Error0 = 0;
    p->Error1 = 0;
    p->ErrorInt = 0;
}

void PID_Update(PID_t *pid_handle){
    pid_handle->Error1 = pid_handle->Error0;
    pid_handle->Error0 = pid_handle->Target - pid_handle->Actual;
    /*积分分离*/
    if(fabs(pid_handle->Error0) > pid_handle->ErrorMax){
        pid_handle->ErrorInt = 0;
    }
    else {
        if(fabs(pid_handle->Ki) > EPSILON){
            pid_handle->ErrorInt += pid_handle->Error0;
        }
        else {
            pid_handle->ErrorInt = 0;
        }   
    }
    pid_handle->Out = pid_handle->Kp * pid_handle->Error0 
                    + pid_handle->Ki * pid_handle->ErrorInt
                    + pid_handle->Kd * (pid_handle->Error0 - pid_handle->Error1);
    if(pid_handle->Out > pid_handle->OutMax){
        pid_handle->Out = pid_handle->OutMax;
    }
    else if(pid_handle->Out < pid_handle->OutMin){
        pid_handle->Out = pid_handle->OutMin;
    }
}