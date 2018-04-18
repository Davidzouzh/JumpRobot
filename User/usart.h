#ifndef USART_H
#define USART_H


#include "stm32f10x.h"
#include "stdio.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	


extern uint8_t StartMeasureFlag;
extern uint16_t DistanceBufCnt;
extern uint16_t DistanceBuf[1000];

extern uint32_t Measure_T;

void USART1_Config(void);
void USART2_Config(void);
void USART2_Put_Char(uint8_t DataToSend);

void Start_MeasureDistance(void);
void Stop_MeasureDistance(void);
void Reset_MeasureDistance(void);
void Print_MeasureDistance(void);

void Set_MeasureDistance(void);

#endif


