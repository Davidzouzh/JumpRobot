#include "pidctrl.h"
#include "timer.h"
#include "usart.h"

#include "adc.h"
#include "valve.h"



float DisplaceSp=0;		//期望位移值
float Out2Valve=0;		//PID计算后输出给阀的计算值

//----PID结构体实例化----
PID_Typedef LVDT_displace_PID;
PID_Typedef LVDT_rate_PID;


//描述：位置式PID的主要实现
static void PID_Postion_Cal(PID_Typedef *PID, float target, float measure, int32_t dertT)
{
	float termI=0;
	
	PID->Error = target - measure;		//误差=期望值-测量值

	PID->Deriv = (PID->Error - PID->PreError)/dertT;		//求微分

	PID->Integ += (PID->Error) * dertT;			//求积分
	
	termI = (PID->Integ) + (PID->Error) * dertT;			//求积分
	//if(termI > -PID->iLimit && termI < PID->iLimit && PID->Output > -PID->iLimit && PID->Output < PID->iLimit)       //在iLimit内才进行积分环节
		PID->Integ = termI;
	//else
		//PID->Integ = 0;
	
	PID->Output = (PID->P * PID->Error) + (PID->I * PID->Integ) + (PID->D * PID->Deriv);    //PID:比例环节+积分环节+微分环节

	PID->PreError = PID->Error;

}

//描述：对比例阀控制中的串级PID中的位置环控制
void CtrlLVDTDisplace(void)
{
	float dt = 0.01f;
	static uint32_t tPrev=0;
	uint32_t now;
	now=micros();
    dt=(tPrev>0) ? (now-tPrev)/1000000.0f : 0;	//dt的单位是s
    tPrev=now;
	
	PID_Postion_Cal(&LVDT_displace_PID, DisplaceSp, LVDT1.Displace, dt);

	Out2Valve = LVDT_displace_PID.Output;

}

//描述：对比例阀控制中的串级PID中的速度环控制
void CtrlLVDTRate(void)
{	
	float dt = 0.01f;
	static uint32_t tPrev=0;
	uint32_t now;
	now=micros();
    dt=(tPrev>0) ? (now-tPrev)/1000000.0f : 0;	//dt的单位是s
    tPrev=now;
	
	PID_Postion_Cal(&LVDT_rate_PID, LVDT_displace_PID.Output, LVDT1.Rate, dt);	
	
	//Out2Valve = LVDT_rate_PID.Output;

}

//描述：输出PWM，控制比例阀
void CtrlValve(void)
{
	uint16_t valve1,valve2;
	if(Out2Valve>0)
	{
		valve1 = (uint16_t)Out2Valve + VALVE_DEADLINE_OUT;
		valve2 = 0;
	}
	else
	{
		valve1 = 0;
		valve2 = (uint16_t)(-Out2Valve) + VALVE_DEADLINE_IN;
	}

	Valve_DacFlash(valve1, valve2);
	
}


//描述：PID赋缺省值
void PID_Default(void)
{
	LVDT_displace_PID.P = 200.0;		//位置环
	LVDT_displace_PID.I = 0.0;
	LVDT_displace_PID.D = 0.0;
	
	LVDT_displace_PID.Integ = 0.0;//积分初值
	LVDT_displace_PID.iLimit = 10000;//积分限制值

	
	LVDT_rate_PID.P = 100.0;			//速度环
	LVDT_rate_PID.I = 0.0;
	LVDT_rate_PID.D = 0.0;
	
	LVDT_rate_PID.Integ = 0.0;//积分初值
	LVDT_rate_PID.iLimit = 10000;//积分限制值


}

//描述：复位PID，主要清除积分值
void PID_Reset(void)
{
	LVDT_displace_PID.Integ = 0.0;
	LVDT_rate_PID.Integ = 0.0;
}

//设置到目标位移值时用的是PID的P控制，需根据效果更改此值
#define Set_Displace_P		10

//设置液压缸到一定位置
void Set_Displace(float displace)
{
	if(displace>LVDT1_ProtectMax || displace<0)
	{
		printf("\r\nout of range,set displace fail.\r\n");
		return;
	}
	do
	{
		Get_LVDTDisplaceAndRate();
		if( LVDT1.Displace < displace )
		{
			Valve_DacFlash(Set_Displace_P*(int32_t)(displace-LVDT1.Displace)+VALVE_DEADLINE_OUT, 0);
		}
		else
		{
			Valve_DacFlash(0, Set_Displace_P*(int32_t)(LVDT1.Displace-displace)+VALVE_DEADLINE_IN);
		}
	}while(	LVDT1.Rate<0.2 && LVDT1.Rate>-0.2 &&
		(LVDT1.Displace-displace)<0.2 && (LVDT1.Displace-displace)>-0.2 );//速度小于0.2，且静态偏差小于0.2则认为初始化完成
	
	Valve_DacFlash(0,0);
	printf("\r\ndisplace is %f.\r\n", LVDT1.Displace);
	if(	LVDT1.Rate<0.2 && LVDT1.Rate>-0.2 &&
		(LVDT1.Displace-displace)<0.2 && (LVDT1.Displace-displace)>-0.2 )
	{
		
	}
	else
	{
		printf("displace is not right.\r\n");
	}


}


//描述：PID赋值，主要由usmart组件调用
//注意：参数 pid_num
//		0-位置环
//		1-速度环
void Assign_PID(uint8_t pid_num, uint16_t p, uint16_t i, uint16_t d)
{
	switch(pid_num)
	{
		case 0:
			LVDT_displace_PID.P = (float)p;		//位置环
			LVDT_displace_PID.I = (float)i;
			LVDT_displace_PID.D = (float)d;
			printf("\r\nLVDT displace_PID assign success.\r\n");
			break;
			
		case 1:
			LVDT_rate_PID.P = (float)p;			//速度环
			LVDT_rate_PID.I = (float)i;
			LVDT_rate_PID.D = (float)d;
			printf("\r\nLVDT rate_PID assign success.\r\n");
			break;
			
		default:
			printf("\r\npid_num error...\r\n");
			break;
	}	
}

//描述：打印PID参数
void Print_PID(void)
{
	printf("\r\n***************************************************\r\n");
	printf("LVDT\r\n");
	printf("displace pid:\r\n");
	printf("p          i          d\r\n");
	printf("%f,    %f,     %f\r\n", LVDT_displace_PID.P, LVDT_displace_PID.I , LVDT_displace_PID.D);
	printf("rate pid:\r\n");
	printf("p          i          d\r\n");
	printf("%f,    %f,     %f\r\n", LVDT_rate_PID.P, LVDT_rate_PID.I , LVDT_rate_PID.D);
	printf("***************************************************\r\n");

}


