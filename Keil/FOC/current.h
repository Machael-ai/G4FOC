#ifndef __CURRENT_H__
#define __CURRENT_H__

#include "main.h"


void current_analysis(float *adc,SVPWM_Struct *foc);
void current_zero(SVPWM_Struct *foc);
void ADC1_ChangeRegularTrigger(ADC_HandleTypeDef *hadc, uint32_t trigger_source, uint32_t trigger_edge);
void Sample_Point_Select(SVPWM_Struct* foc,TIM_HandleTypeDef htim,uint16_t pa,uint16_t pb,uint16_t pc);
void I_Sample_Reconstruction(SVPWM_Struct* foc);
void ADCTrigerMatch(SVPWM_Struct* foc,uint32_t* trigger_source,uint16_t pa,uint16_t pb,uint16_t pc);



#endif 
