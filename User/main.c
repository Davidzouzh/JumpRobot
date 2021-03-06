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


#include "controller.h"
#include "usmart.h"


int main(void)
{
	System_Config();		//系统初始化
	
	while(1)
	{
		usmart_dev.scan();	//扫描指令
	}
}


