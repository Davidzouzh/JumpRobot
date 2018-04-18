
#include "key.h"
#include "timer.h"




void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

//按键处理函数
//返回按键值
uint8_t KEY_Scan(void)
{	 
	if(KEY0 == SET || KEY1 == RESET)
	{
		delay_ms(10);
		if(KEY0 == SET)
			return KEY0_PRES;
		if(KEY1 == RESET)
			return KEY1_PRES;
		while(KEY0 == SET && KEY1 == RESET);
	}
	return 0;
}

