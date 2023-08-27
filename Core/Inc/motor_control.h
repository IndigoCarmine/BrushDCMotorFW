/**
 * motor_control.h
 *
 *  Created on: 2023/08/01
 *      Author: taman
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

typedef enum MotorMode_s
{
    Stop,
    Current,
    Position,
    Homing,
}MotorMode;


void MotorControlUpdate();

void ClearFault();
void ChangeMode(MotorMode mode);
void StopFromLimit(uint32_t Limit_GPIO_Pin);
#include "PID.h"

extern PID currentPID;
extern PID positionPID;


#endif /* INC_MOTOR_CONTROL_H_ */

