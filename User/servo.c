
#include "servo.h"


void Servo_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		// 复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//时间基准设置
	TIM_TimeBaseStructure.TIM_Period = 1999;  
	TIM_TimeBaseStructure.TIM_Prescaler = 239;		//72Mhz/(719+1)/(999+1)=100Hz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//设置时钟分频系数：不分频  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数溢出模式  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//通道设置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//配置为PWM模式1  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = 0;		//设置跳变值，当计数器计数到这个值时，电平发生跳变  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//当定时器计数值小于CCR1时为高电平  
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	//预装载使能	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	//使能重载寄存器ARR
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	TIM_Cmd(TIM3, ENABLE);		//使能TIM3
}

void Servo_Lock(void)
{
	TIM_SetCompare1(TIM3, PWM_ServoLock);
}

void Servo_Unlock(void)
{
	TIM_SetCompare1(TIM3, PWM_ServoUnlock);
}


