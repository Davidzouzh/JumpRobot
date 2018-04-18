#ifndef CONTROL_H
#define CONTROL_H

#include "stm32f10x.h"


// PID结构体
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
    float Error;
    float PreError;
    float Integ;
	float iLimit;
    float Deriv;
    float Output;
 
}PID_Typedef;

extern PID_Typedef LVDT_displace_PID;
extern PID_Typedef LVDT_rate_PID;

extern float DisplaceSp;

extern float Out2Valve;


void CtrlLVDTDisplace(void);
void CtrlLVDTRate(void);
void CtrlValve(void);

void PID_Default(void);
void PID_Reset(void);

void Set_Displace(float displace);

//调试用函数
void Assign_PID(uint8_t pid_num, uint16_t p, uint16_t i, uint16_t d);
void Print_PID(void);

#endif


