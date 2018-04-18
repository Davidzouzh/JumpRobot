#include "usart.h"
#include "timer.h"


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle;
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕      
    USART1->DR = (u8) ch; 
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void USART1_Config(void)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = 115200;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 

}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)
					USART_RX_STA=0;//接收错误,重新开始
				else
				{
					USART_RX_STA|=0x8000;	//接收完成了
				}	
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
    } 

}


//波特率115200
void USART2_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	//时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//串口配置 
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	//中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//中断标志设置
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	//清除标志位并使能
	USART_ClearFlag(USART2,USART_FLAG_TC);	
	USART_Cmd(USART2, ENABLE);
	
}

//USART2串口用
uint8_t TxBuffer2[256];
uint8_t RxBuffer2[256];
uint8_t cnt2=0;
uint8_t TxCnt2=0;
uint8_t RxCnt2=0;

//激光测距用
uint8_t StartMeasureFlag=0;		//启停标志位
uint16_t DistanceBufCnt=0;		//测量的数据个数
uint16_t DistanceBuf[1000];		//测量的数据缓冲区

//对串口2接受到的数据进行解包
void Usart2_CopeSerial2Data(uint8_t ucData)
{
	uint8_t i,checksum=0;
	
	if(StartMeasureFlag != 1)
	{
		return;
	}
	
	RxBuffer2[RxCnt2++]=ucData;
	if( RxBuffer2[0]!=0x5A ) //数据头不对，则重新开始寻找0x5A数据头
	{
		RxCnt2=0;
		return;
	}
	if( RxCnt2==2 && RxBuffer2[1]!=0x5A ) //第2个数据头不对，则重新开始寻找0x5A数据头
	{
		RxCnt2=0;
		return;
	}
	if(RxCnt2<8)			//数目不够
	{
		return;
	}
	for(i=0;i<7;i++)		//检校和检查
	{
		checksum += RxBuffer2[i];
	}
	checksum = checksum&0xFF;
	if( checksum!=RxBuffer2[7] )
	{
		RxCnt2=0;
		return;
	}
	else
	{
		DistanceBuf[DistanceBufCnt++] = RxBuffer2[4]<<8 | RxBuffer2[5];		//计算距离
		RxCnt2=0;
	}
	
}

//USART2中断
void USART2_IRQHandler(void)
{
	//发送数据
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		USART_SendData(USART2, TxBuffer2[TxCnt2++]); 
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
		if(TxCnt2 == cnt2)
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
	//接收数据
	else if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Usart2_CopeSerial2Data((unsigned char)USART2->DR);//处理数据
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	USART_ClearITPendingBit(USART2, USART_IT_ORE);
}

//串口2发送字节
void USART2_Put_Char(uint8_t DataToSend)
{
	TxBuffer2[cnt2++] = DataToSend;  
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}



uint32_t MeasureStartTime,Measure_T=0;

//激光测距启动测量
void Start_MeasureDistance(void)
{
	StartMeasureFlag=1;
	DistanceBufCnt=0;
	
	MeasureStartTime = micros();	//获取起始时间点
}

//激光测距停止测量
void Stop_MeasureDistance(void)
{
	StartMeasureFlag=0;
	
	Measure_T = (micros()-MeasureStartTime)/DistanceBufCnt;		//计算测量周期
}

//复位激光测距传感器
void Reset_MeasureDistance(void)
{
	StartMeasureFlag=0;
	DistanceBufCnt=0;
}

//打印出激光测距的数据
void Print_MeasureDistance(void)
{
	uint16_t i;
	
	printf("\r\n*****************************************\r\n");
	printf("measure T is %d us.\r\n", Measure_T);		//测量周期
	printf("measure point number is %d.\r\n", DistanceBufCnt);		//点的个数
	printf("The distance is\r\n");		//数据点，单位ms
	for(i=0;i<DistanceBufCnt;i++)
		printf("%d, ", DistanceBuf[i]);
	
	printf("\r\n*****************************************\r\n");
}

//设置激光测距传感器，激光测距传感器会自行保存设置，基本不用
void Set_MeasureDistance(void)
{
	USART2_Put_Char(0xA5);//连续输出距离数据
	USART2_Put_Char(0x45);
	USART2_Put_Char(0xEA);
	
	USART2_Put_Char(0xA5);//快速测量模式
	USART2_Put_Char(0x51);
	USART2_Put_Char(0xF6);
	
	USART2_Put_Char(0xA5);//波特率115200
	USART2_Put_Char(0xAF);
	USART2_Put_Char(0x54);
	
	USART2_Put_Char(0xA5);//保存当前配置
	USART2_Put_Char(0x25);
	USART2_Put_Char(0xCA);
}


#endif	


