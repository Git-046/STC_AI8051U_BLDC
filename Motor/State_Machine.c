#include "AI8051U.h"
#include "State_Machine.h"
#include "Motor.h"

u8 xdata Motor_Run_Flag = 0;
enum Motor_State xdata motor_state;		//状态机电机状态
enum Fault_Flag xdata fault_flag;		//电机错误码

void State_Machine(void)
{
	switch(motor_state)
	{
        case motor_init:
            Parameter_Init();
            break;
        case motor_idle:
            
			break;
		case motor_start:
			break;
		case motor_run:
			break;
		case motor_fault:
			break;
		default:
			break;
	}
}

void Motor_Start(void)
{
	if(fault_flag == 0)
    {
        Motor_Run_Flag = 1;
    }
}

void Motor_Run(void)
{
	
}

void Motor_Fault(void)
{
            
}

void Motor_Idle(void)
{
	
}

void Motor_Run_Stop_Control(void)
{
    
//    motor_speed.target_speed = 0;
    
    if(Motor_Run_Flag == 0 && (PWMA_ENO | 0x15) != 0)
    {
        PWMA_ENO &= ~0x15;  //驱动PWM输出禁用
    }
    else if(Motor_Run_Flag == 1 && (PWMA_ENO | 0x15) == 0)
    {
        PWMA_ENO |= 0x15;   //驱动PWM输出使能
    }
}

void Speed_Control_Singal_Detect(void)
{
    
}