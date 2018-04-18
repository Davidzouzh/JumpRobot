#include "relay.h"


void Relay_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
	
}


void Relay1_Actuate(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
}

void Relay1_Release(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void Relay2_Actuate(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

void Relay2_Release(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}


