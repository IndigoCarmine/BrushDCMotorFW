/*
 * motor_control.c
 *
 *  Created on: 2023/08/01
 *      Author: taman
 */


#include "motor_control.h"
#include "driver_register.h"
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "encoder.h"
#include "stdint.h"
#include "led.h"
#include "current.h"
#include "config.h"
#include "can.h"

//ampare -> pwm compare value
PID currentPID = {
		.dt = 1,
		.PGain = 13,
		.DGain = -5,
		.epsilon = 2000,
		.max = 20000
};

//position -> ampare
PID positionPID = {
		.dt = 1,
		.PGain= 0.1,
		.epsilon = 100,
		.max = 1000
};

void SetPWMAndDirection(int32_t pwm);
int GetCurrentDirection();

__IO MotorMode motor_mode = Stop;
MotorMode GetMode(){
	return motor_mode;
}
void StateReset(){
	PID_Reset(&currentPID,GetCurrentDirection() * GetLawCurrent());
	PID_Reset(&positionPID,0);
	Encoder_Reset();
}
void ChangeMode(MotorMode mode){
	//if motor_mode is InterlockWaiting, Cannot Change Interlock_Stop.
	if(motor_mode == Interlock_Waiting && mode == Interlock_Stop)return;

	motor_mode = mode;
	if(motor_mode == Stop ||motor_mode == Interlock_Waiting ||motor_mode == Interlock_Stop){
		HAL_GPIO_WritePin(LED_MOTOR_GPIO_Port, LED_MOTOR_Pin, GPIO_PIN_RESET);
		//pid control stop
		HAL_TIM_Base_Stop_IT(&htim3);
		//pwm stop
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		StateReset();
	}else{

		HAL_GPIO_WritePin(LED_MOTOR_GPIO_Port, LED_MOTOR_Pin, GPIO_PIN_SET);
		//pid control start
		HAL_TIM_Base_Start_IT(&htim3);
		//pwm start
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	}
	CAN_Send_Status(motor_mode);
}

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
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim2);
	DriverActivate();
	InitADC();
}




float encoder_position;
int32_t pwm=0;
/*
Position -> Current -> PWM
*/
void MotorControlUpdate(){
	led_on(state1);
//	if(!HAL_GPIO_ReadPin(nFAULT_GPIO_Port, nFAULT_Pin))return;

	switch(motor_mode){
	case Stop:
	{
		//if mode==Stop, this update loop is not executed.
		break;
	}
	case Interlock_Waiting:
	{
		//if mode==Interlock_Waiting, this update loop is not executed.
		break;
	}
	case PWM:
	{
		//TODO
		break;
	}
	case Current:
	{
		pwm = PID_Update(&currentPID, GetCurrentDirection() * GetLawCurrent());
		break;
	}
	case Position:
	{
		currentPID.target = PID_Update(&positionPID, Encoder_GetPosition());
		pwm = PID_Update(&currentPID, GetCurrentDirection() * GetLawCurrent());
		break;
	}
	case Interlock_Position:
	{
		currentPID.target = PID_Update(&positionPID, Encoder_GetPosition());
		pwm = PID_Update(&currentPID, GetCurrentDirection() * GetLawCurrent());
		break;
	}


	default:
		ChangeMode(Stop);
		break;
	}

	SetPWMAndDirection(pwm);
	encoder_position = Encoder_GetPosition();


}


void StopFromLimit(uint32_t GPIO_Pin){
	MotorMode mode = motor_mode;
	//first, stop as soon as possible
	ChangeMode(Stop);

	if(mode == Interlock_Position || mode == Interlock_Waiting){
		ChangeMode(Interlock_Waiting);
		CAN_Send_Partial_Stop();
		// the job keeps doing on main function.

	}
}


int currentDirection = 0;
int lastMotorDirectionSetting = 0;
void SetPWMAndDirection(int32_t pwm){

	//set direction
	if(pwm > 0){
		HAL_GPIO_WritePin(PH_GPIO_Port, PH_Pin, GPIO_PIN_SET);
		currentDirection = 1;
	}else{
		HAL_GPIO_WritePin(PH_GPIO_Port, PH_Pin, GPIO_PIN_RESET);
		pwm = -pwm;
		currentDirection = -1;
	}

	if(pwm> UINT16_MAX)pwm = UINT16_MAX;

	//low pwm cut off
	if(pwm < 300) pwm = 0;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
}

//it is -1 or 1
int GetCurrentDirection(){
	return currentDirection;
}
