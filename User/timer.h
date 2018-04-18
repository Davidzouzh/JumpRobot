#ifndef TIMER__
#define TIMER__

#include "stm32f10x.h"

extern uint8_t  loop200HzFlag,loop100HzFlag,loop50HzFlag,loop10HzFlag;

//滴答定时器初始化
//config被系统占用了，用init
void SysTick_Init(void);

//获取当前时间，us，ms
uint32_t micros(void);
uint32_t millis(void);

//延时，占用CPU的延时
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
	


#endif


