#include "timer.h"

volatile uint32_t sysTickUptime = 0;
volatile uint32_t usTicks = 0;


uint8_t  loop200HzFlag=0, loop100HzFlag=0;
volatile uint16_t loop200HzCnt=0, loop100HzCnt=0;


uint8_t Timer1Begin=0;
uint8_t Timer2Begin=0;
uint8_t Timer1Trg=0;
uint8_t Timer2Trg=0;
uint8_t Timer1BufCnt=1;
uint8_t Timer2BufCnt=1;
uint16_t Timer1Tick;
uint16_t Timer2Tick;


#define TimeBufNum		10
//const uint16_t Timer1TimeBuf[TimeBufNum]={800};
//const uint16_t Timer2TimeBuf[TimeBufNum]={200};
const uint16_t Timer1TimeBuf[TimeBufNum]={100,200,200,200,200,200,200,200,200,200};
const uint16_t Timer2TimeBuf[TimeBufNum]={200,200,200,200,200,200,200,200,200,200};




void Start_Timer(void)
{
	Timer1Tick = Timer1TimeBuf[0];
	Timer2Tick = Timer2TimeBuf[0];
	Timer1Begin=1;
	Timer2Begin=1;
}
void Stop_Timer(void)
{
	Timer1Begin=0;
	Timer2Begin=0;
	Timer1Trg=0;
	Timer2Trg=0;
	Timer1BufCnt=1;
	Timer2BufCnt=1;
	Timer1Tick=Timer1TimeBuf[0];
	Timer2Tick=Timer2TimeBuf[0];
}


//滴答定时器systick中断
void SysTick_Handler(void) 
{
	//1ms加1，用于读取系统时间和延时
	sysTickUptime++;
	
	if(++loop200HzCnt*200 >= 1000)		//主循环中5ms，10ms，20ms周期的实现
	{
		loop200HzCnt=0;
		loop200HzFlag=1;
	}
	if(++loop100HzCnt*100 >= 1000)
	{
		loop100HzCnt=0;
		loop100HzFlag=1;
	}

	if(Timer1Begin == 1)
	{
		if(Timer1Tick!=0)
		{
			Timer1Tick--;
		}
		else
		{
			Timer1Trg = 1;
			if(Timer1BufCnt < TimeBufNum)
				Timer1Tick = Timer1TimeBuf[Timer1BufCnt++];
			else
				Timer1Begin = 0;
		}
	}
	if(Timer2Begin == 1)
	{
		if(Timer2Tick!=0)
		{
			Timer2Tick--;
		}
		else
		{
			Timer2Trg = 1;
			if(Timer2BufCnt < TimeBufNum)
				Timer2Tick = Timer2TimeBuf[Timer2BufCnt++];
			else
				Timer2Begin = 0;
		}
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


