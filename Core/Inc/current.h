/*
 * current.h
 *
 *  Created on: 2023/08/11
 *      Author: taman
 */

#ifndef INC_CURRENT_H_
#define INC_CURRENT_H_
#include "stdint.h"

//unit:Ampare
float GetCurrent();

uint16_t GetLawCurrent();

// set DMA and start ADC
void InitADC();

#endif /* INC_CURRENT_H_ */
