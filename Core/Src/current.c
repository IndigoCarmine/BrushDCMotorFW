/*
 * current.c
 *
 *  Created on: 2023/08/11
 *      Author: taman
 */


#include "current.h"
#include "adc.h"
#include "stdint.h"

const int A_COEFF = 4570;          // defined by DRV 8244 [A/A]
const int CURRENT_REGISTER = 1000; // CURRENT SENSE REGISTER SIZE [Ohm]


// It is uint16_t. but I use uint32_t for DMA.
uint32_t current = 0;

void InitADC()
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1,&current, 1);
    hdma_adc1.Instance->CCR &= ~(DMA_IT_TC | DMA_IT_HT);
}

float GetCurrent()
{
    /*
    ampare:uint16_t
    ampare/UINT16_MAX * 3.3V : V
    ampare/UINT16_MAX * 3.3V / CURRENT_REGISTER : A (IPROPI output current)
    ampare/UINT16_MAX * 3.3V / CURRENT_REGISTER * A_COEFF : A (motor current)
    */
    return (float)current / UINT16_MAX * 3.3 / CURRENT_REGISTER * A_COEFF;
}

uint16_t GetLawCurrent()
{
    return current;
}
