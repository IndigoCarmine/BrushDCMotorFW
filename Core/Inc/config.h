/*
 * config.h
 *
 *  Created on: Aug 26, 2023
 *      Author: taman
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include "stdint.h"

//reserved BASE_ID_High==0x0, it is for Specific Usage.
#define BASE_ID_High (0x06)
#define ENCODER_SCALER 40
#define BASE_ID (BASE_ID_High << 2)


#endif /* SRC_CONFIG_H_ */
