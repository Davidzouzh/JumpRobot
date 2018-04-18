#ifndef CONTROL__
#define CONTROL__

#include "stm32f10x.h"



#define  LVDT_ProtectMax			50					//定义位移传感器最大范围，单位cm

#define  VALVE_DEADLINE_OUT			700						//比例阀的死区
#define  VALVE_DEADLINE_IN			1000


// PID结构体
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
    float Error;
    float PreError;
    float PrePreError;
    float Increment;
    float Integ;
	float iLimit;
    float Deriv;
    float Output;
 
}PID_Typedef;

extern PID_Typedef LVDT_displace_PID;
extern PID_Typedef LVDT_rate_PID;

extern float DisplaceSp;
extern float Out2Valve;//这个pid输出值全局变量化只是为了能在主函数里查看


void CtrlLVDTDisplace(void);
void CtrlLVDTRate(void);
void CtrlValve(void);

void PID_Default(void);
void PID_Reset(void);
void PID_assign(uint8_t pid_num, uint16_t p, uint16_t i, uint16_t d);
void PID_Printf(void);


#endif

