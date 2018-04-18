/*
main.c
编写者：邹志华
编译环境：MDK-Lite  Version: 5.23
初版时间: 2018-03-20
功能：
	主函数
	
引脚定义：
	控制比例阀		PA4			伸出端
					PA5			缩回端
	-------------------------------------
	位移传感器		PC0
					PC1
	-------------------------------------
	按键			PA0			伸出
					PC13		缩回
	-------------------------------------
	压力传感器		PC2			1
					PC3			2
	-------------------------------------
	舵机			PA6	
-------------------------------------------
*/
#include "stm32f10x.h"

#include "timer.h"

#include "usart.h"
#include "adc.h"
#include "curve.h"
#include "valve.h"
#include "pwm.h"
#include "control.h"

#include "usmart.h"
#include "stdlib.h"



//以下数据用于输出打印
//用于监测程序运行时间
uint32_t startTime[5],execTime[5];
uint32_t realExecPrd[5];

//用于输出数据
float OUT_Displace[CURVE_LEN], OUT_Displace2[CURVE_LEN];
float OUT_PID[CURVE_LEN];
float OUT_Pressure1[CURVE_LEN], OUT_Pressure2[CURVE_LEN];


#define ServoActionTime		50		//自按下开关开始计时，梯度 5ms

#define DefaultDisplace		10		//液压缸初始化时的位置


void Set_DefaultDisplace(void);

//主函数入口
int main(void)
{
	uint8_t i=0,n=0;
	
	uint32_t ActionStartTime,ActionExecTime,T;
	
	SysTick_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	USART1_Config();
	USART2_Config();
	adc_Init();
	PWM_Config();
	Valve_Init();
	usmart_dev.init(72); 		//初始化USMART
	
	//Servo_Unlock();
	
	Valve_DacFlash(0,2000);		//将液压缸缩回
	delay_ms(1000);
	Valve_DacFlash(0,0);
	
	Servo_Lock();

	
	LVDT.Ready=0;
	LVDT2.Ready=0;
	do
	{
		Get_LVDTDisplaceAndRate();	//初始化位移传感器的零点
	}while(LVDT.Ready == 0 && LVDT2.Ready == 0);

	PID_Default();				//PID赋默认值
	
	Set_DefaultDisplace();
	
	delay_ms(200);
	if(	LVDT.Rate<0.2 && LVDT.Rate>-0.2 &&
		(LVDT.Displace-DefaultDisplace)<0.2 && (LVDT.Displace-DefaultDisplace)>-0.2 )
	{
		printf("system init success.\r\n");
	}
	else
	{
		printf("DefaultDisplace is not right.\r\n");
	}
	

	while(1)
	{
		switch(command)
		{
			case cmd_out:			//液压缸伸出操作
				printf("OUT.\r\n\r\n");
				ActionStartTime = millis();
				Start_MeasureDistance();
				while(1)
				{
					realExecPrd[0] = micros()-startTime[0];
					startTime[0] = micros();
					
					Get_LVDTDisplaceAndRate();		//获取位移传感器数据（位移/速度）
					Get_Pressure();					//获取液压
					
					execTime[0]=micros()-startTime[0];
					
					if(loop200HzFlag)//每5ms执行一次
					{
						loop200HzFlag=0;
						
						realExecPrd[1]=micros()-startTime[1];
						startTime[1]=micros();
						
						CtrlLVDTRate();				//角速度环PID
						CtrlValve();				//控制比例阀
						
						if( (millis()-ActionStartTime) >= ServoActionTime )
						{
							Servo_Unlock();
							//Servo_Lock();
						}

						execTime[1]=micros()-startTime[1];
					}
					
					if(loop100HzFlag)//每10ms执行一次
					{
						loop100HzFlag=0;
						
						realExecPrd[2]=micros()-startTime[2];
						startTime[2]=micros();
						
						CtrlLVDTDisplace();			//角度环PID
						
						DisplaceSp = curve[i];		//期望曲线赋值

						OUT_Displace[i] = LVDT.Displace;	//用于输出打印
						OUT_Displace2[i] = LVDT2.Displace;
						OUT_PID[i] = Out2Valve;
						OUT_Pressure1[i] = Pressure1;
						OUT_Pressure2[i++] = Pressure2;
						
						if(i >= CURVE_LEN)						//曲线进行到结尾，要回到开始，并停止比例阀
						{
							i=0;
							command = cmd_zero;
							break;
						}
						if(LVDT.Displace >= LVDT_ProtectMax)	//超程保护，大于限制时停止伸出操作
						{
							command = cmd_zero;
							break;
						}
						
						execTime[2] = micros()-startTime[2];
					}
				
				}//end of while(1)
				
				
				Valve_DacFlash(0,0);
				Stop_MeasureDistance();
				
				ActionExecTime = millis() - ActionStartTime;
				T = ActionExecTime/count;
				
				printf("OUT done.\r\n");
				
				printf("*****************************************\r\n");	//打印软件执行时间
				printf("realExecPrd is %d, %d, %d.\r\n",realExecPrd[0],realExecPrd[1],realExecPrd[2]);
				printf("execTime is %d, %d, %d.\r\n",execTime[0],execTime[1],execTime[2]);
				printf("*****************************************\r\n");
				// printf("curve_sp is\r\n");		//打印期望曲线
				// for(n=0; n<CURVE_LEN; n++)
				 // printf("%f,  ", curve[n]);
				// printf("\r\n");
				// printf("Displace is\r\n");		//打印实际位移曲线
				// for(n=0; n<CURVE_LEN; n++)
				 // printf("%f,  ", OUT_Displace[n]);
				// printf("\r\n");
				// printf("Displace2 is\r\n");
				// for(n=0; n<CURVE_LEN; n++)
				 // printf("%f,  ", OUT_Displace2[n]);
				// printf("\r\n");
				
				// printf("*****************************************\r\n");
				// printf("OUT_PID is\r\n");		//打印PID的输出值
				// for(n=0; n<CURVE_LEN; n++)
					// printf("%f,  ", OUT_PID[n]);
				// printf("\r\n");
				
				// printf("*****************************************\r\n");
				// printf("Pressure1 is\r\n");		//测量的液压1
				// for(n=0; n<CURVE_LEN; n++)
				 // printf("%f,  ", OUT_Pressure1[n]);
				// printf("\r\n");
				// printf("Pressure2 is\r\n");		//测量的液压2
				// for(n=0; n<CURVE_LEN; n++)
				 // printf("%f,  ", OUT_Pressure2[n]);
				// printf("\r\n");
				
				printf("*****************************************\r\n");
				printf("measure T is %d\r\n", T);
				printf("The distance is\r\n");
				printf("*****************************************\r\n");
				for(i=0;i<count;i++)
					printf(" %d,", distance[i]);
				printf("*****************************************\r\n");
				
				printf("\r\n\r\n");
				
				PID_Reset();
				Reset_MeasureDistance();
				
				command = cmd_zero;
				break;
				
				
			case cmd_back:				//液压缸缩回操作
				printf("BACK.\r\n\r\n");
				Valve_DacFlash(0,0);
				printf("Displace is\r\n");
				while(1)
				{
					Get_LVDTDisplaceAndRate();
					if(LVDT.Displace > 0.5)
					{
						Valve_DacFlash(0,2000);
					}	
					else
					{
						Valve_DacFlash(0,0);
						break;
					}
					printf("%f\r\n", LVDT.Displace);
					delay_ms(50);
				}
				Servo_Lock();
				printf("BACK done.\r\n\r\n");
				command = cmd_zero;
				break;
				
				
			default:					//不操作
				Valve_DacFlash(0,0);
				break;

		}
	
	}
}


void Set_DefaultDisplace(void)
{
	do
	{
		if( LVDT.Displace < DefaultDisplace )
		{
			Valve_DacFlash(160*abs((int32_t)(LVDT.Displace-DefaultDisplace)), 0);
		}
		else
		{
			Valve_DacFlash(0, 160*abs((int32_t)(LVDT.Displace-DefaultDisplace)));
		}
	}while(	LVDT.Rate<0.2 && LVDT.Rate>-0.2 &&
		(LVDT.Displace-DefaultDisplace)<0.2 && (LVDT.Displace-DefaultDisplace)>-0.2 );//速度为0，且静态偏差小于0.2则认为初始化完成
	
	Valve_DacFlash(0,0);
}

