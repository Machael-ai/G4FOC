#ifndef _MOTOR_H
#define _MOTOR_H

#include "FOC.h"
#include "PID.h"
#include "def.h"
#include "main.h"


void openloop_analysis(uint8_t *usart1_rx,SVPWM_Struct *FOC);
void open_loop(SVPWM_Struct *FOC);
void Current_loop(SVPWM_Struct* foc,float id_ref,float iq_ref);
void DriverAlign(SVPWM_Struct* foc);



#endif