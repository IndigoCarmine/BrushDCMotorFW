/*
 * PID.h
 *
 *  Created on: Aug 8, 2023
 *      Author: taman
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include <stdlib.h>

struct PID_s{
	const float PGain;
	const float IGain;
	const float DGain;

	const float dt;
	const float max;

	float ITerm;
	float DTerm;

	float target;

};
typedef struct PID_s PID;

float PID_Update(PID *pid, float now){
	float error = pid->target - now;
	pid->ITerm += pid->IGain * error * pid->dt;
	pid->DTerm = pid->DGain * (error - pid->DTerm) / pid->dt;

	float out = pid->PGain * error +pid->ITerm + pid->DTerm;

	return abs(out) > pid->max ? pid->max * (out / abs(out)) : out;
}

#endif /* INC_PID_H_ */
