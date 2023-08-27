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
	float PGain;
	float IGain;
	float DGain;

	const float dt;
	float max;

	float ITerm;
	float DTerm;

	float target;

};
typedef struct PID_s PID;

float PID_Update(PID *pid, float now);

#endif /* INC_PID_H_ */
