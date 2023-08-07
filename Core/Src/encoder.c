/*
 * use_it.c
 *
 *  Created on: Aug 8, 2023
 *      Author: taman
 */

#include "tim.h"
#include "encoder.h"

int overflowcnt = 0;

void EncoderOverflowCallback(){
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
int32_t GetPosition(){
	return overflowcnt * UINT16_MAX + htim2.Instance->CNT;
}
