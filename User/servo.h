#ifndef PWM_H
#define PWM_H

#include "stm32f10x.h"

//PWM的周期由这两个值决定
#define PWM_Period			1999	//PWM定时器计数个数
#define PWM_Prescaler		239		//定时器分频系数，
//72MHz/(239+1)=300KHz，定时器以300Khz频率计数，记满1999+1个点一个周期，72Mhz/(239+1)/(1999+1)=150Hz，PWM波的周期就150Hz


//舵机解锁和上锁的PWM值，在0~(PWM_Period+1)之间，通常为(PWM_Period+1)的10%~20%
#define PWM_ServoLock		90
#define PWM_ServoUnlock		150


void Servo_Config(void);

void Servo_Lock(void);
void Servo_Unlock(void);


#endif


