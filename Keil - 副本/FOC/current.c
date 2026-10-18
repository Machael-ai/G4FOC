#include "current.h"
#include "adc.h"
/*
根据相序学习的结果，将ABC相和SVPWM的i_sample对应
*/
void current_analysis(float *adc,SVPWM_Struct *foc){
        float i_sample[3]={0};
//        i_sample[0]=(*adc-1.65)/50/0.01-foc->ia_zero;     
//        i_sample[1]=(*(adc+1)-1.65)/50/0.01-foc->ib_zero;     
//        i_sample[2]=(*(adc+2)-1.65)/50/0.01-foc->ic_zero; 
        i_sample[0]=(*adc-foc->c1_zero)/50/0.01;     
        i_sample[1]=(*(adc+1)-foc->c2_zero)/50/0.01;     
        i_sample[2]=(*(adc+2)-foc->c3_zero)/50/0.01;         
        if(foc->ch_a==TIM_CHANNEL_1)
                foc->i_sample[0]=i_sample[0];
        else if(foc->ch_a==TIM_CHANNEL_2)
                foc->i_sample[0]=i_sample[1];
        else if(foc->ch_a==TIM_CHANNEL_3)
                foc->i_sample[0]=i_sample[2];
        
        ////////////////////////////////////////
        if(foc->ch_b==TIM_CHANNEL_1)
                foc->i_sample[1]=i_sample[0];
        else if(foc->ch_b==TIM_CHANNEL_2)
                foc->i_sample[1]=i_sample[1];
        else if(foc->ch_b==TIM_CHANNEL_3)
                foc->i_sample[1]=i_sample[2];
        
        ////////////////////////////////////////
        if(foc->ch_c==TIM_CHANNEL_1)
                foc->i_sample[2]=i_sample[0];
        else if(foc->ch_c==TIM_CHANNEL_2)
                foc->i_sample[2]=i_sample[1];
        else if(foc->ch_c==TIM_CHANNEL_3)
                foc->i_sample[2]=i_sample[2];
        
        I_Sample_Reconstruction(foc);
         
}
/*
电流采样前，连续采集200次取平均值作为电流零漂
*/
void current_zero(SVPWM_Struct *foc){
        
//                foc->ia_zero+=(ADC_Value[0]-1.65)/50/0.01;
//                foc->ib_zero+=(ADC_Value[1]-1.65)/50/0.01;
//                foc->ic_zero+=(ADC_Value[2]-1.65)/50/0.01;
        foc->c1_zero+=ADC_Value[0];
        foc->c2_zero+=ADC_Value[1];
        foc->c3_zero+=ADC_Value[2];


        foc->c1_zero=foc->c1_zero/2;
        foc->c2_zero=foc->c2_zero/2;
        foc->c3_zero=foc->c3_zero/2;
}
/*
重新设置ADC1的规则组触发信号
*/

void ADC1_ChangeRegularTrigger(ADC_HandleTypeDef *hadc, uint32_t trigger_source, uint32_t trigger_edge) {
    HAL_ADC_Stop_DMA(hadc);
    // 清除触发配置
    CLEAR_BIT(hadc->Instance->CFGR, ADC_CFGR_EXTSEL | ADC_CFGR_EXTEN);
    // 设置新触发源和边沿
    SET_BIT(hadc->Instance->CFGR, trigger_source | trigger_edge);
    HAL_ADC_Start_DMA(hadc, (uint32_t *)ADC_Value_temp, ADC_CHANNELS);
}

/*
获取最大值下标（0:a, 1:b, 2:c）
*/
static int max_index(uint16_t pa, uint16_t pb, uint16_t pc) {
    if (pa >= pb && pa >= pc) return 0;
    else if (pb >= pa && pb >= pc) return 1;
    else return 2;
}


/*

*/
void ADCTrigerMatch(SVPWM_Struct* foc, uint32_t* trigger_source, uint16_t pa, uint16_t pb, uint16_t pc) {
    uint32_t channels[3] = { foc->ch_a, foc->ch_b, foc->ch_c };
    if (foc->i_sam_rec == 3) {
        foc->fall_dowm_delay_flag=0;
        foc->i_sam_rec = max_index(pa, pb, pc);
    }
    else {
        foc->fall_dowm_delay_flag=1;
    }
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    if(foc->fall_dowm_delay_flag==0){
        if(foc->i_sam_rec==0) foc->pd=pa;
        else if(foc->i_sam_rec==0) foc->pd=pb;
        else foc->pd=pc;
    }
    else{
        if(foc->i_sam_rec==0) foc->pd=pa+10;
        else if(foc->i_sam_rec==0) foc->pd=pb+10;
        else foc->pd=pc+10;
        foc->i_sam_rec = max_index(pa, pb, pc);
    }
}

/*
采样点选择
中心对齐模式下，PWM的周期时间为（ARR+1）*2*(CCR+1)/PWM频率=1700*2*5/170M=100us
Tmin计算得400ns，对应到比较值是400/100000*1700=6.8
*/
void Sample_Point_Select(SVPWM_Struct* foc, TIM_HandleTypeDef htim, uint16_t pa, uint16_t pb, uint16_t pc) {
    const float plimit = htim.Init.Period * 0.95f;
    uint32_t adctrigger_source;
    uint16_t values[3] = {pa, pb, pc};
    foc->i_sam_rec = 3;  //// 如果没有超限则默认采样最大值

    // 检查是否有超限
    for (int i = 0; i < 3; ++i) {
            if (values[i] > plimit) {
                // 判断values[i]是否为中间值
                if ((values[i] > values[(i+1)%3] && values[i] < values[(i+2)%3]) || (values[i] < values[(i+1)%3] && values[i] > values[(i+2)%3])) {
                    // values[i]是中间值
                    uint16_t pmax = values[(i+1)%3] > values[(i+2)%3] ? values[(i+1)%3] : values[(i+2)%3];
                    if ((pmax - values[i]) > 14) {
                        foc->i_sam_rec = i; // 0:选a, 1:选b, 2:选c
                        break;
                    }
                }
            }
        }
    
    ADCTrigerMatch(foc, &adctrigger_source, pa, pb, pc);
    //ADC1_ChangeRegularTrigger(&hadc1, adctrigger_source, ADC_EXTERNALTRIGCONVEDGE_FALLING);
}

void I_Sample_Reconstruction(SVPWM_Struct* foc){
                if(foc->i_sam_rec==0){
                        foc->i_sample[0]=-(foc->i_sample[1]+foc->i_sample[2]);
                }
                else if(foc->i_sam_rec==1){
                        foc->i_sample[1]=-(foc->i_sample[0]+foc->i_sample[2]);
                }
                else if(foc->i_sam_rec==2){
                        foc->i_sample[2]=-(foc->i_sample[0]+foc->i_sample[1]);
                }
                else{
                        return;
                }
}
