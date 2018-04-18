#ifndef RELAY_H
#define RELAY_H

#include "stm32f10x.h"

void Relay_Config(void);

void Relay1_Actuate(void);
void Relay1_Release(void);
void Relay2_Actuate(void);
void Relay2_Release(void);

#endif


