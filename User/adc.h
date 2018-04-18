#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"


#define ADC_INTERNAL_VREF   1.20		//芯片内部参考电压，理论值1.20v，实际1.16-1.24v
#define ADC_VREF			3.26		//ADC的参考电压，即为芯片供电电压，要实测，不波动!!!!!
#define ADC_MAX_VAL			ADC_VREF	//位移传感器满行程时的电压值
#define LVDT1_MAX			7.50			//定义缸上位移传感器的电阻最大行程，单位cm
#define LVDT2_MAX			7.50
#define LVDT1_ProtectMax	5.0			//定义位移传感器可运动最大范围，单位cm
#define LVDT2_ProtectMax	6.30

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

extern  LVDT_Typedef LVDT1, LVDT2;
extern float  Pressure1, Pressure2;



void ADC_Config(void);
void Get_LVDTDisplaceAndRate(void);
void Get_Pressure(void);


//调试用函数
void Print_LVDTDisplaceAndRate(void);
void Print_Pressure(void);

#endif


