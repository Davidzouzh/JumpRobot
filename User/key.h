#ifndef __KEY_H_
#define __KEY_H_

#include "stm32f10x.h"



#define KEY0	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)		//读取按键
#define KEY1	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)

#define KEY0_PRES		1	//按键扫描返回值
#define KEY1_PRES		2


void KEY_Init(void);
uint8_t KEY_Scan(void);

#endif


