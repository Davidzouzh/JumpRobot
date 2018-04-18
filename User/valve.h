#ifndef VALVE_H
#define VALVE_H

#include "stm32f10x.h"


#define Valve_DACMax	3300		//DAC输出的最大电压值，不可更改

#define VALVE_DEADLINE_OUT			700						//比例阀的死区
#define VALVE_DEADLINE_IN			1000

void DAC_Config(void);
void Valve_DacFlash(uint16_t Valve1_DAC, uint16_t Valve2_DAC);


#endif


