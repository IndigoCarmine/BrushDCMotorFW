/*
 * PID.c
 *
 *  Created on: Aug 26, 2023
 *      Author: taman
 */

#include "PID.h"
#include "math.h"



float PID_Update(PID *pid,float now){

	float error = pid->target - now;
	pid->Integral += (pid->PreError + error)/2*pid->dt;

	float out =
			pid->PGain * error +
			pid->IGain * pid->Integral +
			pid->DGain * (error - pid->PreError)/pid->dt;

	pid->PreError = error;

	if(fabsf(out) > pid->max){
		out = pid->max * (out / fabsf(out));
	}else if(fabsf(out) < pid->epsilon){
		out = 0;
	}

	return out;
}

void PID_Reset(PID* pid,float normal_error){
	pid->Integral = 0;
	pid->PreError = normal_error;
	pid->target = 0;
}
