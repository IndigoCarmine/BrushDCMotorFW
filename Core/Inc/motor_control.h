 /**
 * motor_control.h
 *
 *  Created on: 2023/08/01
 *      Author: taman
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_
#include "stdint.h"

typedef enum MotorMode_s
{
    Stop,
	PWM,
    Current,
    Position,
	Interlock_Position,
	Interlock_Waiting,
	Interlock_Stop
}MotorMode;


void MotorControlUpdate();

void ClearFault();
void ChangeMode(MotorMode mode);
MotorMode GetMode();
void StopFromLimit(uint32_t Limit_GPIO_Pin);

void Activate();

#include "PID.h"

extern PID currentPID;
extern PID positionPID;


#endif /* INC_MOTOR_CONTROL_H_ */

