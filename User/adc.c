#include "adc.h"
#include "timer.h"
#include "usart.h"



__IO uint32_t ADCConvertedValue[10];


void ADC_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	DMA_InitTypeDef DMA_InitStructure;

//开启时钟，TIM2，ADC1，ADC2，GPIOA，AFIO
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M,否则将导致ADC准确度下降! 

//GPIOC配置                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	

//TIM2配置
	TIM_TimeBaseStructure.TIM_Period = 3599;
	TIM_TimeBaseStructure.TIM_Prescaler = 1;//72Mhz/(3599+1)/(1+1)=1000Hz,也就是1ms1个点，5ms就能采样出5个点
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

//TIM2通道2配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 1;//待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//输出比较极性低
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能CCR2上的预装载寄存器


//DMA1通道1配置
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//指定外设为源地址
	DMA_InitStructure.DMA_BufferSize = 10;	//设置DMA在传输时缓冲区的长度 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//设置DMA外设存储器地址不变 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//设置DMA内存递增模式 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//设置外设数据宽度32bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	//设置存储器数据宽度32bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//设置DMA的传输模式：连续不断的循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;	//设置DMA的优先级别为极高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	//开启DMA1通道1
	DMA_Cmd(DMA1_Channel1, ENABLE);

//ADC1配置
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	//ADC1 和 ADC2 工作在同步规则和同步注入模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//转换由TIM2通道2外部触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 2;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);
	//ADC1转换通道队列配置
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);		//ADC1通道10，PC0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_28Cycles5);		//ADC2通道11，PC1

//ADC2配置
	ADC_DeInit(ADC2);
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC2, &ADC_InitStructure);
	//ADC2转换通道队列配置
	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_28Cycles5);		//ADC2通道12，PC2
	ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 2, ADC_SampleTime_28Cycles5);		//ADC2通道13，PC3
	

	ADC_Cmd(ADC1, ENABLE);	//使能ADC1
	ADC_ResetCalibration(ADC1);	//使能复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	ADC_StartCalibration(ADC1);	//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	//等待校准结束

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);	//使能ADC1的经外部触发启动转换功能
	//ADC_TempSensorVrefintCmd(ENABLE);	//使能温度传感器和内部参考电压通道

	ADC_Cmd(ADC2, ENABLE);
	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC2));
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2));

	ADC_ExternalTrigConvCmd(ADC2, ENABLE);

	ADC_DMACmd(ADC1, ENABLE);	//使能ADC1的DMA传输
  
	TIM_Cmd(TIM2, ENABLE);		//使能TIM2

}


//定义一个位移传感器的信息结构体
LVDT_Typedef LVDT1, LVDT2;


//获得髋关节、膝关节的角度和角速度  
void Get_LVDTDisplaceAndRate(void)  
{  
    uint8_t i;
    uint16_t sum1=0, sum2=0;
	
	float dt = 0.01f;
	static uint32_t tPrev=0;
	uint32_t now;
	now=micros();
    dt=(tPrev>0) ? (now-tPrev)/1000000.0f : 0;	//dt的单位是s
    tPrev=now;
  
    for (i=0;i<5;i++) 
    {
        sum1 += ADCConvertedValue[i*2]&0xFFFF;
		sum2 += ADCConvertedValue[i*2+1]&0xFFFF;
    }
	
	LVDT1.AD = sum1/5;
	LVDT2.AD = sum2/5;
	
	LVDT1.Val = (LVDT1.AD/4096.0) * ADC_VREF;
	LVDT2.Val = (LVDT2.AD/4096.0) * ADC_VREF;
    
	if(!LVDT1.Ready || !LVDT2.Ready)
	{
		LVDT1.Bias = (LVDT1.Val/ADC_MAX_VAL) * LVDT1_MAX;
		LVDT1.Displace = 0;
		LVDT1.Rate = 0;
		LVDT1.Ready = 1;

		LVDT2.Bias = (LVDT2.Val/ADC_MAX_VAL) * LVDT2_MAX;
		LVDT2.Displace = 0;
		LVDT2.Rate = 0;
		LVDT2.Ready = 1;
		return;
	}
	
	LVDT1.Old = LVDT1.Displace;
	LVDT2.Old = LVDT2.Displace;
	
	LVDT1.Displace = (LVDT1.Val/ADC_MAX_VAL) * LVDT1_MAX - LVDT1.Bias;	//单位cm
	LVDT2.Displace = (LVDT2.Val/ADC_MAX_VAL) * LVDT2_MAX - LVDT2.Bias;
	
	LVDT1.Rate = (LVDT1.Displace - LVDT1.Old) / dt;	//单位cm/s
	LVDT2.Rate = (LVDT2.Displace - LVDT2.Old) / dt;
	
}


float Pressure1=0, Pressure2=0;

void Get_Pressure(void)
{
	uint8_t i;
    uint16_t adc=0,adc2=0;
	for (i=0;i<5;i++)
    {
        adc += ADCConvertedValue[i*2]>>16;
		adc2 += ADCConvertedValue[i*2+1]>>16;
    }
	adc = adc/5;
	adc2 = adc2/5;
	//p = 0.1132*v+0.7890
	//v = 8.8339*p+6.8905
	//单位：v-V, p-MPa
	//Pressure = 8.0205*3.26*adc/4096-5.9537;
	Pressure1 = 6.3797*3.26*adc/4096+0.1610;
	Pressure2 = 6.3797*3.26*adc2/4096+0.1610;
}


void Print_LVDTDisplaceAndRate(void)
{
	Get_LVDTDisplaceAndRate();
	printf("\r\nDisplace:1-%f, 2-%f\r\n", LVDT1.Displace, LVDT2.Displace);
}

void Print_Pressure(void)
{
	Get_Pressure();
	printf("\r\nPressure: 1-%f, 2-%f\r\n", Pressure1, Pressure2);
}


