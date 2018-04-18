#ifndef PWM_H
#define PWM_H

#include "stm32f10x.h"

#define PWM_ServoLock		90 		//舵机解锁和上锁的PWM值
#define PWM_ServoUnlock		150

#define UnlockDisplace		0			//舵机解锁的时机，以期望位移值为准


void PWM_Config(void);
void Servo_Lock(void);
void Servo_Unlock(void);


#endif

