#ifndef USART_H
#define USART_H


#include "stm32f10x.h"
#include "stdio.h"

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	


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


