#include "FOC.h"
#include "math.h"
#include "current.h"

void set_pwm(SVPWM_Struct *foc,TIM_HandleTypeDef htim,uint16_t pa,uint16_t pb,uint16_t pc){
        Sample_Point_Select(foc,htim,pa,pb,pc);
        __HAL_TIM_SET_COMPARE(&htim, foc->ch_a, pa);
        __HAL_TIM_SET_COMPARE(&htim, foc->ch_b, pb);
        __HAL_TIM_SET_COMPARE(&htim, foc->ch_c, pc);
        __HAL_TIM_SET_COMPARE(&htim, foc->ch_c, foc->pd);
}


void anti_park(SVPWM_Struct* foc){
	//u(1)*cos(u(3))-u(2)*sin(u(3))
	//u(1)*sin(u(3))+u(2)*cos(u(3))
	foc->valpha=(foc->vd)*cos(foc->theta)-(foc->vq)*sin(foc->theta);
	foc->vbeta=(foc->vd)*sin(foc->theta)+(foc->vq)*cos(foc->theta);
}	
	


void park(SVPWM_Struct* foc){
//	id=(*ialpha)*cos(*theta)+*ibeta*sin(*theta);
//	iq=-(*ialpha)*sin(*theta)+*ibeta*cos(*theta);
	foc->id=foc->ialpha*cos(foc->theta)+foc->ibeta*sin(foc->theta);
	foc->iq=-foc->ialpha*sin(foc->theta)+foc->ibeta*cos(foc->theta);
}
	

void anti_clarke(SVPWM_Struct* foc){
//	va=*valpha;
//	vb=(-*valpha+*vbeta*sqrt(3))/2;
//	vc=(-*valpha-*vbeta*sqrt(3))/2;
	foc->v_clarke[0]=foc->valpha;
	foc->v_clarke[1]=(-foc->valpha+foc->vbeta*sqrt(3))/2;
	foc->v_clarke[2]=(-foc->valpha-foc->vbeta*sqrt(3))/2;
}
	

void clarke(SVPWM_Struct* foc){
//	ialpha=*ia;
//	ibeta=(*ia+2*(*ib))/sqrt(3);
	foc->ialpha=foc->i_sample[0];
	foc->ibeta=(foc->i_sample[0]+2*(foc->i_sample[1]))/sqrt(3);
}
	
		


void SVPWM_Init(SVPWM_Result* result){
	result->tcmp1=0;
	result->tcmp2=0;
	result->tcmp3=0;
}

/*SVPWM
扇区     1 2 3 4 5 6
sector   3 1 5 4 6 2
Tpwm: PWM周期时间或者直接传入最大重装载值htim1.Init.Period
tcmp1/2/3: 重装载值
*/
SVPWM_Result SVPWM_Calculate(SVPWM_Struct* foc, float Udc, float Tpwm)
{
    anti_park(foc);
    SVPWM_Result result = {0};
	
    float Vref1 = foc->vbeta;
    float Vref2 = (sqrtf(3.0f) * foc->valpha - foc->vbeta) / 2.0f;
    float Vref3 = (-sqrtf(3.0f) * foc->valpha - foc->vbeta) / 2.0f;

    //============== Sector Calculation ==============
    uint8_t sector = 0;
    if (Vref1 > 0) sector += 1;
    if (Vref2 > 0) sector += 2;
    if (Vref3 > 0) sector += 4;

    result.sector = sector;

    //============== XYZ Calculation =================
    float X = sqrtf(3.0f) * foc->vbeta * Tpwm / Udc;
    float Y = Tpwm / Udc * (1.5f * foc->valpha + sqrtf(3.0f) / 2.0f * foc->vbeta);
    float Z = Tpwm / Udc * (-1.5f * foc->valpha + sqrtf(3.0f) / 2.0f * foc->vbeta);

    //============== Duty Time Calculation ===========
    float T1 = 0.0f, T2 = 0.0f;

    switch (sector)
    {
        case 1: T1 = Z;    T2 = Y;    break;
        case 2: T1 = Y;    T2 = -X;   break;
        case 3: T1 = -Z;   T2 = X;    break;
        case 4: T1 = -X;   T2 = Z;    break;
        case 5: T1 = X;    T2 = -Y;   break;
        case 6: T1 = -Y;   T2 = -Z;   break;
        default: break;
    }

    if ((T1 + T2) > Tpwm) {
        float scale = Tpwm / (T1 + T2);
        T1 *= scale;
        T2 *= scale;
    }

    float ta = (Tpwm - (T1 + T2)) / 4.0f;
    float tb = ta + T1 / 2.0f;
    float tc = tb + T2 / 2.0f;

    //=============== Compare Value Assignment ==============
    switch (sector)
    {
        case 1:
            result.tcmp1 = tb;
            result.tcmp2 = ta;
            result.tcmp3 = tc;
            break;
        case 2:
            result.tcmp1 = ta;
            result.tcmp2 = tc;
            result.tcmp3 = tb;
            break;
        case 3:
            result.tcmp1 = ta;
            result.tcmp2 = tb;
            result.tcmp3 = tc;
            break;
        case 4:
            result.tcmp1 = tc;
            result.tcmp2 = tb;
            result.tcmp3 = ta;
            break;
        case 5:
            result.tcmp1 = tc;
            result.tcmp2 = ta;
            result.tcmp3 = tb;
            break;
        case 6:
            result.tcmp1 = tb;
            result.tcmp2 = tc;
            result.tcmp3 = ta;
            break;
        default:
            result.tcmp1 = result.tcmp2 = result.tcmp3 = 0;
            break;
    }
    if(result.tcmp1<0) result.tcmp1=0;
		if(result.tcmp2<0) result.tcmp2=0;
		if(result.tcmp3<0) result.tcmp3=0;
		if(result.tcmp1>htim1.Init.Period) result.tcmp1=htim1.Init.Period;
		if(result.tcmp2>htim1.Init.Period) result.tcmp2=htim1.Init.Period;
		if(result.tcmp3>htim1.Init.Period) result.tcmp3=htim1.Init.Period;
                set_pwm(foc,htim1,result.tcmp1,result.tcmp2,result.tcmp3);
//		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, result.tcmp3);
//		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, result.tcmp2);
//		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, result.tcmp1);
		return result;
}


