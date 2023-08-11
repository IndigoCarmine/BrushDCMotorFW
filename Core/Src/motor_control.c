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
#include "led.h"
#include "current.h"



void DriverActivate(){

	HAL_GPIO_WritePin(nSleep_GPIO_Port, nSleep_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(DRVOFF_GPIO_Port, DRVOFF_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(nSleep_GPIO_Port, nSleep_Pin, GPIO_PIN_SET);

	while(HAL_GPIO_ReadPin(nFAULT_GPIO_Port, nFAULT_Pin)!=GPIO_PIN_RESET){
	  led_on(state1);
	  led_process();
	}
	while(HAL_GPIO_ReadPin(nFAULT_GPIO_Port, nFAULT_Pin)!=GPIO_PIN_SET){
	  led_on(state2);
	  ClearFault();
	  led_process();
	}
	HAL_GPIO_WritePin(DRVOFF_GPIO_Port, DRVOFF_Pin, GPIO_PIN_RESET);
}
void Activate(){

	DriverActivate();
	InitADC();
	
	//start pid control
	HAL_TIM_Base_Start_IT(&htim3);
	//htim3 -> interrupt -> MotorControlUpdate()
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
	int32_t pwm = PID_Update(&currentPID, GetLawCurrent());

	//set direction
	if(pwm > 0){
		HAL_GPIO_WritePin(PH_GPIO_Port, PH_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(PH_GPIO_Port, PH_Pin, GPIO_PIN_RESET);
		pwm = -pwm;
	}
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
}

	

