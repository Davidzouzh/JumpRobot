
#ifndef ADC__H
#define ADC__H

#include "stm32f10x.h"


#define ADC_INTERNAL_VREF   1.20		//芯片内部参考电压，理论值1.20v，实际1.16-1.24v
#define ADC_VREF			3.26		//ADC的参考电压，即为芯片供电电压，要实测，不波动!!!!!
#define LVDT_MAX			50			//定义缸上位移传感器的电阻最大行程，单位cm

//位移传感器信息结构体
typedef struct
{
	uint8_t		Ready;
	uint16_t	AD;				//AD值
	float		Val;			//电压值
	float		Displace;		//位移值，单位cm
	float		Bias;			//偏移值
	float		Old;			//存储上一次的值，为计算速度用
	float		Rate;			//速度，单位cm/s
	
}LVDT_Typedef;

extern LVDT_Typedef LVDT;
extern float Pressure1, Pressure2;

void adc_Init(void);
void Get_LVDTDisplaceAndRate(void);
void Get_Pressure(void);

#endif

