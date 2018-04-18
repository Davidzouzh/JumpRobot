/*
main.c
编写者：邹志华
编译环境：MDK-Lite  Version: 5.23
初版时间: 2018-04-17
功能：
	主函数
	
引脚定义：
	控制比例阀		PA4			伸出端
					PA5			缩回端
	-------------------------------------
	位移传感器		PC0
					PC1
	-------------------------------------
	压力传感器		PC2			1
					PC3			2
	-------------------------------------
	激光测距模块	USART2(PA2,PA3)
	蓝牙串口		USART1(PA9,PA10)
	-------------------------------------
	舵机			PA6
	电磁继电器		PB10		1
					PB11		2
	-------------------------------------
	SWD下载口		PA13,PA14
-------------------------------------------
*/


#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"
#include "valve.h"
#include "servo.h"
#include "relay.h"
#include "usmart.h"
#include "pidctrl.h"




void System_Config(void)
{
	SysTick_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//外设初始化
	USART1_Config();
	USART2_Config();
	ADC_Config();
	DAC_Config();
	Servo_Config();
	Relay_Config();
	usmart_dev.init(72);
	
	//系统初始化
	Valve_DacFlash(0,2000);		//将液压缸缩回
	delay_ms(1000);
	Valve_DacFlash(0,0);
	
	Servo_Lock();

	Get_LVDTDisplaceAndRate();	//初始化位移传感器的零点

	PID_Default();				//PID赋默认值
	
	//Set_Displace(0);			//设置默认位移值
	
	printf("\r\nsystem init success.\r\n\r\n");
}



int main(void)
{
	System_Config();		//系统初始化
	
	while(1)
	{
		usmart_dev.scan();	//扫描指令
	}
}


