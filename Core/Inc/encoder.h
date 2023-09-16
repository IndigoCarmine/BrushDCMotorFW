/*
 * encoder.h
 *
 *  Created on: Aug 8, 2023
 *      Author: taman
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_


float Encoder_GetPosition();

void Encoder_OverflowCallback();
void Encoder_Reset();
#endif /* INC_ENCODER_H_ */
