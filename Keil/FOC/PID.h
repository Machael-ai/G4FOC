#ifndef __PID_H__
#define __PID_H__

#include "main.h"
#include "FOC.h"
#include "def.h"



void PIController_Init(PIController *ctr,float kp,float ki,float max,float min);
float PIController_Compute(PIController *ctr, float reference, float feedback);


#endif
