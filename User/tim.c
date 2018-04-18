#include "tim.h"
#include "servo.h"
#include "relay.h"


//初始化了两个定时器来计时，可以设置一组间隔时间，定时到执行动作，单个定时器只能执行一种动作，执行的动作在TIM2_Action()和TIM5_Action()函数中


#define TickBufNum	3
uint16_t TIM1TickBuf[TickBufNum]={200,300,500};//计时时间组，数组的个数由TickBufNum决定，数组存的是距上一次动作的间隔时间，
uint16_t TIM8TickBuf[TickBufNum]={200,400,600};
//单位ms，可以记6553ms约6.5s

uint8_t TIM1TickBufCnt=1;
uint8_t TIM8TickBufCnt=1;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

//定时器1 8初始化
void TIM1TIM8_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM8, ENABLE); //时钟使能

	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = TIM1TickBuf[0]*10;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;		//0.1ms跳一下，可以记6553ms约6.5s
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period = TIM8TickBuf[0]*10;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;		//0.1ms跳一下，可以记6553ms约6.5s
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
 
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM1, DISABLE);
	TIM_Cmd(TIM8, DISABLE);	
}


//定时器1定时时间到要执行的动作
void TIM1_Action(void)
{
	//Servo_Unlock();
	Relay2_Release();
}


//定时器8定时时间到要执行的动作
void TIM8_Action(void)
{
	//Servo_Lock();
	Relay2_Actuate();
}

//定时器1的中断服务函数
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)	//查询中断标志位
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);		//清除中断标志位
		
		TIM1_Action();		//定时到执行的函数
		
//		//关闭定时器1，装载下一个计时时间，再开启定时器



	}
}

//定时器8的中断服务函数
void TIM8_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		
		TIM8_Action();
		
	}
}


//启动定时器，在动作开始时就要启动
void Start_Timer(void)
{
	TIM_Cmd(TIM1, ENABLE);
	delay_ms(100);
	TIM_Cmd(TIM8, ENABLE);
}
void Stop_Timer(void)
{
	TIM_Cmd(TIM1, DISABLE);
	TIM_Cmd(TIM8, DISABLE);
}

