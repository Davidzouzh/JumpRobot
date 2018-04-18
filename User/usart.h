#ifndef USART__
#define USART__


#include "stm32f10x.h"
#include "stdio.h"

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	



#define cmd_out			0				//Һѹ�ײ���ָ���������ء�������
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


