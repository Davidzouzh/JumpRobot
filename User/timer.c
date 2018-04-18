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


//�δ�ʱ��systick�ж�
void SysTick_Handler(void) 
{
	//1ms��1�����ڶ�ȡϵͳʱ�����ʱ
	sysTickUptime++;
	
	if(++loop200HzCnt*200 >= 1000)		//��ѭ����5ms��10ms��20ms���ڵ�ʵ��
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


//ϵͳsystick��ʼ��
//config��ϵͳռ���ˣ���init
void SysTick_Init(void)
{
	RCC_ClocksTypeDef clocks;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8  

	RCC_GetClocksFreq(&clocks);
	usTicks = clocks.SYSCLK_Frequency/1000000;	//��ʼ��usTicks

	SysTick_Config(SystemCoreClock/1000);	//SysTick����ϵͳtick��ʱ������ʼ�����жϣ�1ms
}

// Return system uptime in microseconds (rollover in 70minutes)
//���� us
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
//���� ms
uint32_t millis(void)
{
    return sysTickUptime;
}

//��ʱus		    								   
void delay_us(uint32_t nus)
{
	uint32_t t0 = micros();
	while(micros() - t0 < nus);
}

//��ʱms
void delay_ms(uint32_t nms)
{
	uint32_t t0 = micros();
	while(micros() - t0 < nms * 1000);
}


