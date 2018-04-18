

#include "timer.h"



volatile uint32_t sysTickUptime = 0;
volatile uint32_t usTicks = 0;


uint8_t  loop200HzFlag=0, loop100HzFlag=0, loop50HzFlag=0, loop10HzFlag=0;
volatile uint16_t loop200HzCnt=0, loop100HzCnt=0, loop50HzCnt=0, loop10HzCnt=0;

//滴答定时器systick中断
void SysTick_Handler(void) 
{
	//1ms加1，用于读取系统时间和延时
	sysTickUptime++;
	
	if(++loop200HzCnt*200 >= 1000)
	{
		loop200HzCnt=0;
		loop200HzFlag=1;
	}
	if(++loop100HzCnt*100 >= 1000)
	{
		loop100HzCnt=0;
		loop100HzFlag=1;
	}
	if(++loop50HzCnt*50 >= 1000)
	{
		loop50HzCnt=0;
		loop50HzFlag=1;
	}
	if(++loop10HzCnt*10 >= 1000)
	{
		loop10HzCnt=0;
		loop10HzFlag=1;
	}
	
}


//系统systick初始化
//config被系统占用了，用init
void SysTick_Init(void)
{
		RCC_ClocksTypeDef clocks;
		SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8  

    RCC_GetClocksFreq(&clocks);
    usTicks = clocks.SYSCLK_Frequency/1000000;	//初始化usTicks
	
		SysTick_Config(SystemCoreClock/1000);	//SysTick开启系统tick定时器并初始化其中断，1ms
}

// Return system uptime in microseconds (rollover in 70minutes)
//返回 us
uint32_t micros(void)
{
    register uint32_t ms, cycle_cnt;
    do {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    } while (ms != sysTickUptime);
    return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

// Return system uptime in milliseconds (rollover in 49 days)
//返回 ms
uint32_t millis(void)
{
    return sysTickUptime;
}

//延时us		    								   
void delay_us(uint32_t nus)
{
	uint32_t t0 = micros();
	while(micros() - t0 < nus);
}

//延时ms
void delay_ms(uint32_t nms)
{
	uint32_t t0 = micros();
	while(micros() - t0 < nms * 1000);
}


