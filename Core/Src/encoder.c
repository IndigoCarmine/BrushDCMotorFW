/*
 * use_it.c
 *
 *  Created on: Aug 8, 2023
 *      Author: taman
 */

#include "tim.h"
#include "encoder.h"
#include "config.h"

int overflowcnt = 0;
__IO int direction = 1;

void Encoder_SetDirection(int dir){
	direction = dir;
}

void Encoder_OverflowCallback(){

    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_IT_UPDATE);
    if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)) //0 → 65535
    {
        overflowcnt --;
    }
    else //65535 → 0
    {
        overflowcnt ++;

    }
}
float Encoder_GetPosition(){
	return direction *ENCODER_SCALER* ((float)overflowcnt * UINT16_MAX + htim2.Instance->CNT);
}

void Encoder_Reset(){
	overflowcnt = 0;
	htim2.Instance->CNT =0;
}
