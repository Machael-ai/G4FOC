#include "motor.h"
#include "current.h"
float c_a;
float c_b;
float c_c;

/*
相序学习
*/
void DriverAlign(SVPWM_Struct* foc){
     
       
        foc->ch_a=TIM_CHANNEL_1;
        foc->ch_b=TIM_CHANNEL_2;
        foc->ch_c=TIM_CHANNEL_3;
        float i_sample[3]={0};
        if(i_DriverAlign<100){
                //set_pwm(foc,htim1,htim1.Init.Period/2,0,0);//要在上一次ADC回调中设置占空比

                i_sample[0]=(ADC_Value[0]-foc->c1_zero)/50/0.01;     //(ADC_Value[1]-1.65)/50/0.01;
                i_sample[1]=(ADC_Value[1]-foc->c2_zero)/50/0.01;     //(ADC_Value[0]-1.65)/50/0.01;
                i_sample[2]=(ADC_Value[2]-foc->c3_zero)/50/0.01;     //(ADC_Value[2]-1.65)/50/0.01;
                I_Sample_Reconstruction(foc);
//                 float c_a=i_sample[0];
//                 float c_b=i_sample[1];
//                 float c_c=i_sample[2];
                
                float a1=i_sample[0];
                float b1=i_sample[1];
                float c1=i_sample[2];
                c_a=0.6*c_a+0.4*a1;
                c_b=0.6*c_b+0.4*b1;
                c_c=0.6*c_c+0.4*c1;
                i_DriverAlign++;
        }
        else if(i_DriverAlign==100){
                set_pwm(foc,htim1,0,0,0);
                float ab_ratio=fabs(c_a/c_b);
                float ac_ratio=fabs(c_a/c_c);
                if(ab_ratio>1.5f)  
                        foc->ch_a=foc->ch_a;
                else if(ab_ratio<0.7f){
                        uint32_t temp=foc->ch_a;
                        foc->ch_a=foc->ch_b;
                        foc->ch_b=temp;
                }
                else if(ac_ratio<0.7f){
                        uint32_t temp=foc->ch_a;
                        foc->ch_a=foc->ch_c;
                        foc->ch_c=temp;
                }
                set_pwm(foc,htim1,0,htim1.Init.Period/2,0);
                i_DriverAlign++;
        }
        //////////////////////////////////////
        else if(i_DriverAlign>100 && i_DriverAlign<200){
                //set_pwm(foc,htim1,0,htim1.Init.Period/2,0);//同理
                i_sample[0]=(ADC_Value[0]-foc->c1_zero)/50/0.01;     //(ADC_Value[1]-1.65)/50/0.01;
                i_sample[1]=(ADC_Value[1]-foc->c2_zero)/50/0.01;     //(ADC_Value[0]-1.65)/50/0.01;
                i_sample[2]=(ADC_Value[2]-foc->c3_zero)/50/0.01;     //(ADC_Value[2]-1.65)/50/0.01;
                I_Sample_Reconstruction(foc);
//                c_a=i_sample[0];
//                c_b=i_sample[1];
//                c_c=i_sample[2];

                float a1=i_sample[0];
                float b1=i_sample[1];
                float c1=i_sample[2];
                c_a=0.6*c_a+0.4*a1;
                c_b=0.6*c_b+0.4*b1;
                c_c=0.6*c_c+0.4*c1;
                i_DriverAlign++;                
        }
        else if(i_DriverAlign == 200){
                set_pwm(foc,htim1,0,0,0);
                float ba_ratio=fabs(c_b/c_a);
                float bc_ratio=fabs(c_b/c_c);
                if(ba_ratio>1.5f)  
                        foc->ch_b=foc->ch_b;
                else if(ba_ratio<0.7f){
                        uint32_t temp=foc->ch_b;
                        foc->ch_b=foc->ch_a;
                        foc->ch_a=temp;
                }
                else if(bc_ratio<0.7f){
                        uint32_t temp=foc->ch_b;
                        foc->ch_b=foc->ch_c;
                        foc->ch_c=temp;
                }
                DriverAlign_flag=true;
                 isample_flag=2;
        }
        //////////////////////////////////////
//        set_pwm(0,htim1.Init.Period/2,0);
//        HAL_Delay(1);
//        i_sample[0]=(ADC_Value[0]-1.65)/50/0.01;     //(ADC_Value[1]-1.65)/50/0.01;
//        i_sample[1]=(ADC_Value[1]-1.65)/50/0.01;     //(ADC_Value[0]-1.65)/50/0.01;
//        i_sample[2]=(ADC_Value[2]-1.65)/50/0.01;     //(ADC_Value[2]-1.65)/50/0.01;
//        a=i_sample[0];
//        b=i_sample[1];
//        c=i_sample[2];
//        for(int i=0;i<100;i++){
//                float a1=i_sample[0];
//                float b1=i_sample[1];
//                float c1=i_sample[2];
//                a=0.6*a+0.4*a1;
//                b=0.6*b+0.4*b1;
//                c=0.6*c+0.4*c1;
//        }
//        set_pwm(0,0,0);
//        float ca_ratio=fabs(a/b);
//        float cb_ratio=fabs(a/c);
//        if(ca_ratio>1.5f)  
//                foc->ch_c=foc->ch_c;
//        else if(ca_ratio<0.7f)
//                foc->ch_c=foc->ch_a;
//        else if(cb_ratio<0.7f)
//                foc->ch_c=foc->ch_b; 
}
/*
开环控制电机
通过openloop_analysis解析开环数据格式，传入theta_step和vq
*/
void open_loop(SVPWM_Struct *FOC){
        //FOC->theta=encoder.angle_f;
        //FOC->theta+=theta_step;
        FOC->theta+=0.002;
        if(FOC->theta>=2.0f*pi){
                FOC->theta-=2.0f*pi; 
                //FOC->theta=0;
        }
        FOC->vd=0;
        FOC->vq=10;   //vq通过openloop_analysis解析
        result1=SVPWM_Calculate(FOC,24,htim1.Init.Period);//ADC_Value[4]*10.39
        clarke(FOC);
        park(FOC);        
}


/*
function:       open loop parameter analysis
                head | high8 | low8 | theta_step | vq  |tail
                0x01 |  xx   |  xx  |     xx     | xx  |0xAF
*/
void openloop_analysis(uint8_t *usart1_rx,SVPWM_Struct *FOC){
           if(usart1_rx[0]==0x01 && usart1_rx[usart1_rx_num-1]==0xAF){
                factor=usart1_rx[1]<<8 | usart1_rx[2];
                theta_step=(float)usart1_rx[3]/(float)factor;
                FOC->vq=usart1_rx[4];
           }
}



/*

*/
void Current_loop(SVPWM_Struct* foc,float id_ref,float iq_ref){
        clarke(foc);
        park(foc);
        PIController_Compute(&cur_lop_iq_pictr,iq_ref,foc->iq);
        foc->vq=cur_lop_iq_pictr.output;
        PIController_Compute(&cur_lop_id_pictr,id_ref,foc->id);
        foc->vd=cur_lop_id_pictr.output;
        //foc->vd=0;
        //PIController_Compute(&cur_lop_id_pictr,id_ref,foc->id);
        //foc->theta+=theta_step;
        foc->theta+=0.0069;
        if(foc->theta>=2.0f*pi){
                foc->theta-=2.0f*pi; 
                //foc->theta=0;
        }
        result1=SVPWM_Calculate(foc,24,htim1.Init.Period);
}




