#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"

extern uint8_t  loop200HzFlag,loop100HzFlag;

//�δ�ʱ����ʼ��
//config��ϵͳռ���ˣ���init
void SysTick_Init(void);

//��ȡ��ǰʱ�䣬us��ms
uint32_t micros(void);
uint32_t millis(void);

//��ʱ��ռ��CPU����ʱ
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);




extern uint8_t Timer1Trg,Timer2Trg;

void Start_Timer(void);
void Stop_Timer(void);

#endif


