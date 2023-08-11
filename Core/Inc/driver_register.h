
#ifndef DRIVER_REGISTER_H__
#define DRIVER_REGISTER_H__


#include "stdint.h"

enum FaultMasks
{
    OUT1_IS_OPEN = 0b10000000,
    OUT2_IS_OPEN = 0b01000000,
    OUT_OF_AMP_LIM = 0b00100000,
    DIVICE_IS_ACTIVE = 0b00010000,
    OUT1_HIGH_SHORT = 0b00001000,
    OUT1_LOW_SHORT = 0b00000100,
    OUT2_HIGH_SHORT = 0b00000010,
    OUT2_LOW_SHORT = 0b00000001,
};
uint8_t ReadFaultSummary();
uint8_t ReadFault();
void ClearFault();

#endif
