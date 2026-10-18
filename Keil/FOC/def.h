#ifndef _DEF_H
#define _DEF_H

#include "main.h"

typedef struct {
    float tcmp1;
    float tcmp2;
    float tcmp3;
    uint8_t sector;
} SVPWM_Result;

typedef struct {
        float c1_zero;
        float c2_zero;
        float c3_zero;
        uint32_t ch_a;
        uint32_t ch_b;
        uint32_t ch_c;
    float vd;
    float vq;
    float theta;
	float valpha;
    float vbeta;
	float id;
	float iq;
	float ialpha;
	float ibeta;
	float i_sample[3];   //采样电流值 0A 1B 2C
	float v_clarke[3];
        uint8_t i_sam_rec;   //电流重构标志位  0重构A  1重构B  2重构C
        uint8_t fall_dowm_delay_flag;
        uint16_t pd;//用作上桥臂下降沿延时触发ADC
}SVPWM_Struct;

typedef struct {
    float Kp;        
    float Ki;        
    float integral;  
    float output;     
    float maxOutput; 
    float minOutput; 
}PIController;

#endif