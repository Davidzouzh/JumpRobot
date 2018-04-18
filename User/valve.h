#ifndef VALVE__
#define VALVE__

#include "stm32f10x.h"


#define Valve_DACMax	3300


void Valve_DacFlash(uint16_t Valve1_DAC, uint16_t Valve2_DAC);
void Valve_Init(void);

#endif


