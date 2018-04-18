#include "tim.h"
#include "servo.h"

#define TickBufNum	1
uint16_t TIM2TickBuf[TickBufNum]={2000};
uint16_t TIM3TickBuf[TickBufNum]={3000};

uint8_t TIM2TickBufCnt=1,TIM3TickBufCnt=1;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

void TIM2TIM3_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = TIM2TickBuf[0];
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;		//0.1ms跳一下，可以记6553ms约6.5s
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period = TIM3TickBuf[0];
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM3, DISABLE);		 
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		Servo_Lock();
		
		TIM_Cmd(TIM2, DISABLE);
		if(TIM2TickBufCnt >= TickBufNum)
		{
			TIM2TickBufCnt=1;
			TIM_TimeBaseStructure.TIM_Period = TIM2TickBuf[TIM2TickBufCnt++];
			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
			TIM_Cmd(TIM2, ENABLE);
		}
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		Servo_Unlock();
		
		TIM_Cmd(TIM3, DISABLE);
		if(TIM3TickBufCnt >= TickBufNum)
		{
			TIM3TickBufCnt=1;
			TIM_TimeBaseStructure.TIM_Period = TIM3TickBuf[TIM3TickBufCnt++];
			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
			TIM_Cmd(TIM3, ENABLE);
		}
	}
}


void Start_Timer(void)
{
	//TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}


