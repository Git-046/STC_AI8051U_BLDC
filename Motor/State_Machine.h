#include "def.h"

#ifndef __STATE_MACHINE_H
#define __STATE_MACHINE_H

enum Motor_State		//状态机
{
    motor_init  = 0,    //初始
    motor_idle  = 1,	//空闲
	motor_start = 2,	//启动
	motor_run   = 3,	//运行
	motor_fault = 4,	//错误
};

enum Fault_Flag			//错误状态码
{
    motor_normal = 0,           //正常
	motor_stall = 1,			//堵转
	motor_over_current = 2,		//过流
	motor_under_voltage = 3,	//低压
	motor_over_voltage = 4,		//过压
	motor_lost_phase = 5,		//缺相
};

enum Time_Count
{
    time_us = 0,
    time_ms = 1,
    time_s  = 2,
};

void State_Machine(void);
void Motor_Init(void);
void Motor_Start(void);
void Motor_Run(void);
void Motor_Fault(void);
void Motor_Idle(void);

void Stall_Restart(void);

void Parameter_Init(void);
void Motor_Run_Stop_Control(void);

extern u8 xdata Motor_Run_Flag;                             //电机运行状态位
extern enum Motor_State xdata motor_state;					//状态机电机状态
extern enum Fault_Flag xdata fault_flag;					//电机错误码
extern enum Time_Count xdata time_count;                    //时间计数
extern u16 restart_times;                                   //电机重启次数

#endif