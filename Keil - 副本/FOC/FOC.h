#ifndef __FOC_H__
#define __FOC_H__

#include "main.h"
#include "math.h"
#include "stdint.h"
#include "tim.h"
#include "PID.h"
#include "def.h"



void anti_park(SVPWM_Struct* foc);
void park(SVPWM_Struct* foc);
void anti_clarke(SVPWM_Struct* foc);
void clarke(SVPWM_Struct* foc);
SVPWM_Result SVPWM_Calculate(SVPWM_Struct* foc, float Udc, float Tpwm);
void SVPWM_Init(SVPWM_Result* result);
void set_pwm(SVPWM_Struct *foc,TIM_HandleTypeDef htim,uint16_t pa,uint16_t pb,uint16_t pc);




#endif 
