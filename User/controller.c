/*
controller.c
编写者：邹志华
编译环境：MDK-Lite  Version: 5.23
初版时间: 2018-04-17
功能：
	系统功能的主要实现，以及控制指令
	
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
#include "timer.h"
#include "usart.h"
#include "adc.h"
#include "valve.h"
#include "curve.h"
#include "pidctrl.h"
#include "servo.h"
#include "relay.h"
#include "tim.h"
#include "usmart.h"


//用于监测程序运行时间
uint32_t startTime[5],execTime[5];
uint32_t realExecPrd[5];

//函数的堆栈里放不下，一定要定义为全局变量存储在ram里才行
//用于输出数据
float OUT_Displace[CURVE_LEN], OUT_Displace2[CURVE_LEN];
float OUT_PID[CURVE_LEN];
float OUT_Pressure1[CURVE_LEN], OUT_Pressure2[CURVE_LEN];

//系统初始化函数
void System_Config(void)
{
	SysTick_Init();		//滴答定时器初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组初始化
	
	//外设初始化
	USART1_Config();	//串口1初始化，蓝牙串口模块
	USART2_Config();	//串口2初始化，激光测距传感器
	ADC_Config();		//ADC初始化，位移传感器和压强传感器
	DAC_Config();		//DAC初始化，阀
	Servo_Config();		//舵机初始化，初始化一路PWM
	Relay_Config();		//电磁继电器初始化，初始化两路开关量
	TIM1TIM8_Config();	//定时器1 8的初始化
	usmart_dev.init(72);	//usmart调试组件初始化
	
	//系统初始化
	Valve_DacFlash(0,2000);		//将液压缸缩回
	delay_ms(1000);
	Valve_DacFlash(0,0);
	
	Servo_Lock();		//舵机上锁

	Get_LVDTDisplaceAndRate();	//初始化位移传感器的零点

	PID_Default();				//PID赋默认值
	
	//Set_Displace(0);			//设置默认位移值
	
	printf("\r\nsystem init success.\r\n\r\n");
}

//信息打印函数
void Print_Information(void)
{
	uint16_t i;
	printf("*****************************************\r\n");	//打印软件执行时间
	printf("realExecPrd is %d, %d, %d.\r\n",realExecPrd[0],realExecPrd[1],realExecPrd[2]);
	printf("execTime is %d, %d, %d.\r\n",execTime[0],execTime[1],execTime[2]);
	printf("*****************************************\r\n");
	printf("curve_sp is\r\n");		//打印期望曲线
	for(i=0; i<CURVE_LEN; i++)
	 printf("%f,  ", curve[i]);
	printf("\r\n");
	printf("Displace is\r\n");		//打印实际位移曲线
	for(i=0; i<CURVE_LEN; i++)
	 printf("%f,  ", OUT_Displace[i]);
	printf("\r\n");
	printf("Displace2 is\r\n");
	for(i=0; i<CURVE_LEN; i++)
	 printf("%f,  ", OUT_Displace2[i]);
	printf("\r\n");
	
	printf("*****************************************\r\n");
	printf("OUT_PID is\r\n");		//打印PID的输出值
	for(i=0; i<CURVE_LEN; i++)
		printf("%f,  ", OUT_PID[i]);
	printf("\r\n");
	
	printf("*****************************************\r\n");
	printf("Pressure1 is\r\n");		//测量的液压1
	for(i=0; i<CURVE_LEN; i++)
	 printf("%f,  ", OUT_Pressure1[i]);
	printf("\r\n");
	printf("Pressure2 is\r\n");		//测量的液压2
	for(i=0; i<CURVE_LEN; i++)
	 printf("%f, ", OUT_Pressure2[i]);
	printf("\r\n");
	printf("*****************************************\r\n");
}

//指令：执行curve.h中的目标曲线
//可伸出可缩回，曲线怎么给就怎么动，前提初始位置要对，如要从最大位置缩回，则要先设置到伸出最大位置，默认是缩回的
void CMD_Action(void)
{
	uint16_t i=0;
	
//动作起始	
	Start_MeasureDistance();	//启动激光测距
	Start_Timer();				//启动定时器
	Relay1_Release();			//释放电磁铁
	while(1)
	{
		realExecPrd[0] = micros()-startTime[0];		//软件执行时间测量：计算间隔时间
		startTime[0] = micros();		//软件执行时间测量：获取开始时间
		
		Get_LVDTDisplaceAndRate();		//获取位移传感器数据（位移/速度）
		Get_Pressure();					//获取液压
		
		execTime[0]=micros()-startTime[0];	//软件执行时间测量：计算执行时间
		
		if(loop200HzFlag)//每5ms执行一次
		{
			loop200HzFlag=0;	//清除标志位
			
			realExecPrd[1] = micros()-startTime[1];
			startTime[1] = micros();
			
			CtrlLVDTRate();				//角速度环PID
			CtrlValve();				//控制比例阀

			execTime[1] = micros()-startTime[1];
		}
		
		if(loop100HzFlag)//每10ms执行一次
		{
			loop100HzFlag=0;
			
			realExecPrd[2] = micros()-startTime[2];
			startTime[2] = micros();
			
			CtrlLVDTDisplace();			//角度环PID
			
			DisplaceSp = curve[i];		//期望曲线赋值

			OUT_Displace[i] = LVDT1.Displace;	//缓存用于输出打印的数据
			OUT_Displace2[i] = LVDT2.Displace;
			OUT_PID[i] = Out2Valve;
			OUT_Pressure1[i] = Pressure1;
			OUT_Pressure2[i] = Pressure2;
			i++;
			
			if(i >= CURVE_LEN)		//曲线进行到结尾，要停止比例阀
			{
				i=0;
				break;
			}
			if(LVDT1.Displace >= LVDT1_ProtectMax || LVDT2.Displace >= LVDT2_ProtectMax)	//超程保护
			{
				break;
			}
			
			execTime[2] = micros()-startTime[2];
		}

	}//end of while(1)

	//停止
	Valve_DacFlash(0,0);	//关阀
	Stop_MeasureDistance();		//停止测量
	
	TIM_Cmd(TIM1, DISABLE);
	TIM_Cmd(TIM8, DISABLE);

	printf("\r\nOUT done.\r\n\r\n");

	Print_Information();	//信息打印
	delay_ms(500);		//延迟500ms是为了缓冲一下，不让串口的数据乱码
	Print_MeasureDistance();	//激光测距的数据输出打印

	//系统复位(需在信息打印之后)
	PID_Reset();		//清楚PID的积分值
	Reset_MeasureDistance();	//复位激光测距传感器
	
}

//指令：缩回
void CMD_BACK(void)
{
	while(1)
	{
		Get_LVDTDisplaceAndRate();
		if(LVDT1.Displace > 0.5)
		{
			Valve_DacFlash(0,2000);
		}	
		else
		{
			Valve_DacFlash(0,0);
			break;
		}
		
		delay_ms(50);
	}//end of while(1)
	printf("\r\n%f\r\n", LVDT1.Displace);
	printf("BACK done.\r\n");
}

//指令：伸出到最大长度
void CMD_OUT(void)
{
	while(1)
	{
		Get_LVDTDisplaceAndRate();
		if(LVDT1.Displace < LVDT1_ProtectMax-0.5)
		{
			Valve_DacFlash(2000,0);
		}	
		else
		{
			Valve_DacFlash(0,0);
			break;
		}
		delay_ms(20);
	}//end of while(1)
	printf("\r\n%f\r\n", LVDT1.Displace);
	printf("OUT done.\r\n");
}


//指令：系统复位
//只做3件事，1-初始化位移传感器的零点，2-清除PID的积分值，3-复位激光测距传感器
void CMD_Reset_System(void)
{
	Valve_DacFlash(0,2000);		//将液压缸缩回
	delay_ms(1000);
	Valve_DacFlash(0,0);

	Get_LVDTDisplaceAndRate();	//初始化位移传感器的零点

	PID_Reset();				//清除PID的积分值
	
	Reset_MeasureDistance();	//复位激光测距传感器
	
	printf("\r\nReset system done.\r\n");
}

