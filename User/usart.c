#include "usart.h"	  



//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������      
    USART1->DR = (u8) ch; 
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void USART1_Config(void)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = 115200;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

uint8_t command = cmd_zero;

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)
					USART_RX_STA=0;//���մ���,���¿�ʼ
				else
				{
					USART_RX_STA|=0x8000;	//���������
					if( USART_RX_BUF[0]=='c' && USART_RX_BUF[1]=='m' && USART_RX_BUF[2]=='d' )
					{
						switch(USART_RX_BUF[4])
						{
							case 'o':	//��� cmd_out
								command = cmd_out;
								break;
							
							case 'b':	//���� cmd_back
								command = cmd_back;
								break;

							default:
								command = cmd_zero;
								break;
						}
					}
				}
				
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
    } 

}


//������115200
void USART2_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	//ʱ��ʹ��
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
	
	//�������� 
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	//�ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//�жϱ�־����
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	//�����־λ��ʹ��
	USART_ClearFlag(USART2,USART_FLAG_TC);	
	USART_Cmd(USART2, ENABLE);
	
}


uint8_t TxBuffer2[256];
uint8_t RxBuffer2[256];
uint8_t cnt2=0;
uint8_t TxCnt2=0;
uint8_t RxCnt2=0;

uint8_t StartReceiveFlag=0;
uint16_t count=0;
uint16_t distance[1000];


void Usart2_CopeSerial2Data(uint8_t ucData)
{
	uint8_t i,checksum=0;
	
	if(StartReceiveFlag == 0)
	{
		return;
	}
	
	RxBuffer2[RxCnt2++]=ucData;
	if( RxBuffer2[0]!=0x5A ) //����ͷ���ԣ������¿�ʼѰ��0x5A����ͷ
	{
		RxCnt2=0;
		return;
	}
	if( RxCnt2==2 && RxBuffer2[1]!=0x5A ) //����ͷ���ԣ������¿�ʼѰ��0x5A����ͷ
	{
		RxBuffer2[0]=0;
		RxCnt2=0;
		return;
	}
	if(RxCnt2<8)//��Ŀ����
	{
		return;
	}
	for(i=0;i<7;i++)
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
		distance[count++] = RxBuffer2[4]<<8 | RxBuffer2[5];
		RxCnt2=0;
	}
	
}


//USART2�ж�
void USART2_IRQHandler(void)
{
	//��������
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		USART_SendData(USART2, TxBuffer2[TxCnt2++]); 
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
		if(TxCnt2 == cnt2)
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
	//��������
	else if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Usart2_CopeSerial2Data((unsigned char)USART2->DR);//��������
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	USART_ClearITPendingBit(USART2, USART_IT_ORE);
}



void USART2_Put_Char(uint8_t DataToSend)
{
	TxBuffer2[cnt2++] = DataToSend;  
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}


void Start_MeasureDistance(void)
{
	StartReceiveFlag=1;
	count=0;
}

void Stop_MeasureDistance(void)
{
	StartReceiveFlag=0;
}

void Reset_MeasureDistance(void)
{
	StartReceiveFlag=0;
	count=0;
}

void Set_MeasureDistance(void)
{
	USART2_Put_Char(0xA5);//���������������
	USART2_Put_Char(0x45);
	USART2_Put_Char(0xEA);
	
	USART2_Put_Char(0xA5);//���ٲ���ģʽ
	USART2_Put_Char(0x51);
	USART2_Put_Char(0xF6);
	
	USART2_Put_Char(0xA5);//������115200
	USART2_Put_Char(0xAF);
	USART2_Put_Char(0x54);
	
	USART2_Put_Char(0xA5);//���浱ǰ����
	USART2_Put_Char(0x25);
	USART2_Put_Char(0xCA);
}


#endif	


