
#include "valve.h"
#include "usart.h"


/***********************************************
函数名：void Valve_DacFlash(int16_t Valve1_DAC,int16_t Valve2_DAC)
功能：更新2路DAC值
输入参数：Valve1_DAC, Valve2_DAC
输出:无
描述：DAC1,DAC2输出，输入范围0-3300 
备注：
***********************************************/
//设置DAC1,DAC2输出电压
//Valve1_DAC: 0~3300,代表0~3.3V
void Valve_DacFlash(uint16_t Valve1_DAC, uint16_t Valve2_DAC)
{
	float temp1, temp2;
	
	if(Valve1_DAC >= Valve_DACMax)	Valve1_DAC = Valve_DACMax;
	if(Valve2_DAC >= Valve_DACMax)	Valve2_DAC = Valve_DACMax;
	
	if(Valve1_DAC <= 0)	Valve1_DAC = 0;
	if(Valve2_DAC <= 0)	Valve2_DAC = 0;//限定输入在0~Valve_DACMax(3300)之间
	
	temp1 = 4095*Valve1_DAC/Valve_DACMax;
	temp2 = 4095*Valve2_DAC/Valve_DACMax;
	
	//printf("%d\r\n",(uint32_t)temp1);
	DAC->DHR12RD = ((uint32_t)temp2<<16) | (uint32_t)temp1;
}

/***********************************************
函数名：ValveInit(void)
功能：输出DAC1,2初始化
输入参数：无
输出:无
描述：调用该函数，即初始化DAC1,DAC2输出模式
备注：
***********************************************/
void Valve_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能PORTA通道时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //使能DAC通道时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
	
	DAC_InitStructure.DAC_Trigger=DAC_Trigger_None;	//不使用触发功能 TEN1=0
	DAC_InitStructure.DAC_WaveGeneration=DAC_WaveGeneration_None;	//不使用波形发生
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;	//屏蔽、幅值设置
	DAC_InitStructure.DAC_OutputBuffer=DAC_OutputBuffer_Enable ;	//DAC1输出缓存开启 BOFF1=1
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);	 //初始化DAC通道
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC
	DAC_Cmd(DAC_Channel_2, ENABLE);

	DAC_SetDualChannelData(DAC_Align_12b_R, 0, 0);  //12位右对齐数据格式设置DAC值


}


