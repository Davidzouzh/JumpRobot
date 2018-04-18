#ifndef USART__
#define USART__


#include "stm32f10x.h"
#include "stdio.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	



#define cmd_out			0				//液压缸操作指令，伸出、缩回、不操作
#define cmd_back		1
#define cmd_zero		2
extern uint8_t command;

extern uint8_t StartReceiveFlag;
extern uint16_t count;
extern uint16_t distance[1000];

void USART1_Config(void);
void USART2_Config(void);
void USART2_Put_Char(uint8_t DataToSend);


void Start_MeasureDistance(void);
void Stop_MeasureDistance(void);
void Reset_MeasureDistance(void);
void Set_MeasureDistance(void);

#endif


