#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "timer.h"	
#include "usart.h"		

#include "controller.h"
#include "adc.h"
#include "pidctrl.h"
#include "relay.h"
#include "servo.h"
#include "usart.h"


extern void led_set(u8 sta);
extern void test_fun(void(*ledset)(u8),u8 sta); 
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(u16 nms)",
 	(void*)delay_us,"void delay_us(u32 nus)",
	
	(void*)CMD_Action,"void CMD_Action(void)",
	(void*)CMD_BACK,"void CMD_BACK(void)",
	(void*)CMD_OUT,"void CMD_OUT(void)",
	(void*)CMD_Reset_System,"void CMD_Reset_System(void)",
	
	(void*)Print_LVDTDisplaceAndRate,"void Print_LVDTDisplaceAndRate(void)",
	(void*)Print_Pressure,"void Print_Pressure(void)",
	(void*)Set_Displace,"void Set_Displace(float displace)",
	
	(void*)Assign_PID,"void Assign_PID(uint8_t pid_num, uint16_t p, uint16_t i, uint16_t d)",
	(void*)Print_PID,"void Print_PID(void)",
	
	(void*)Relay1_Actuate,"void Relay1_Actuate(void)",
	(void*)Relay1_Release,"void Relay1_Release(void)",
	(void*)Relay2_Actuate,"void Relay2_Actuate(void)",
	(void*)Relay2_Release,"void Relay2_Release(void)",
	
	(void*)Servo_Lock,"void Servo_Lock(void)",
	(void*)Servo_Unlock,"void Servo_Unlock(void)",
	
	(void*)Start_MeasureDistance,"void Start_MeasureDistance(void)",
	(void*)Stop_MeasureDistance,"void Stop_MeasureDistance(void)",
	(void*)Reset_MeasureDistance,"void Reset_MeasureDistance(void)",
	(void*)Print_MeasureDistance,"void Print_MeasureDistance(void)",
	
	
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   


