/*
 * PID.c
 *
 *  Created on: Aug 26, 2023
 *      Author: taman
 */

#include "PID.h"

float PID_Update(PID *pid, float now){
	float error = pid->target - now;
	pid->ITerm += pid->IGain * error * pid->dt;
	pid->DTerm = pid->DGain * (error - pid->DTerm) / pid->dt;

	float out = pid->PGain * error +pid->ITerm + pid->DTerm;

	return abs(out) > pid->max ? pid->max * (out / abs(out)) : out;
}
