/*
 * motor_control.c
 *
 *  Created on: 2023/08/01
 *      Author: taman
 */


#include "motor_control.h"
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "PID.h"
#include "tim.h"
#include "encoder.h"
#include "stdint.h"


//It is uint16_t. but I use uint32_t for DMA.
__IO uint32_t ampare = 0;

void InitADC(){
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ampare,1);
}

void Activate(){
	HAL_GPIO_WritePin(DRVOFF_GPIO_Port, DRVOFF_Pin, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim3);
}

enum FaultMasks{
	OUT1_IS_OPEN     = 0b10000000,
	OUT2_IS_OPEN     = 0b01000000,
	OUT_OF_AMP_LIM   = 0b00100000,
	DIVICE_IS_ACTIVE = 0b00010000,
	OUT1_HIGH_SHORT  = 0b00001000,
	OUT1_LOW_SHORT   = 0b00000100,
	OUT2_HIGH_SHORT  = 0b00000010,
	OUT2_LOW_SHORT   = 0b00000001,
};
uint8_t ReadFaultSummary(){
	return (uint8_t)(SPI_Read(0x0) & 0xF);
}
uint8_t ReadFault(){
	//read status1
	return (uint8_t)(SPI_Read(0x1) & 0xF);
}
void ClearFault(){
	SPI_Write(9, 0b10000000);
}



//ampare -> pwm compare value
PID currentPID = {
		.dt = 1,
		.max = INT16_MAX >> 1
};

//position -> ampare
PID positionPID = {
		.dt = 1,
		.max = UINT16_MAX
};

/*
Position -> Current -> PWM
*/
void MotorControlUpdate(){
	int32_t position = GetPosition();
	currentPID.target = PID_Update(&positionPID, position);
	int32_t pwm = PID_Update(&currentPID, ampare);

	if(pwm > 0){
		HAL_GPIO_WritePin(PH_GPIO_Port, PH_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(PH_GPIO_Port, PH_Pin, GPIO_PIN_RESET);
		pwm = -pwm;
	}
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
}

	

