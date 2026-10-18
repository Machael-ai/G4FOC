
#include "PID.h"



void PIController_Init(PIController *ctr,float kp,float ki,float max,float min) {
    ctr->Kp = kp;
    ctr->Ki = ki;
    ctr->integral = 0.0f;
    ctr->output = 0.0f;
    ctr->maxOutput = max;
    ctr->minOutput = min;
}



float PIController_Compute(PIController *ctr, float reference, float feedback) {
    // 计算误差
    float error = reference - feedback;
    // 更新积分项
    ctr->integral += error;
    // 计算输出
    ctr->output = ctr->Kp * error + ctr->Ki * ctr->integral;
    // 限制输出
    if (ctr->output > ctr->maxOutput) {
        ctr->output = ctr->maxOutput;
    } 
    if (ctr->output < ctr->minOutput) {
        ctr->output = ctr->minOutput;
    }
    return ctr->output;
}