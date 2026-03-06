#include "AI8051U.h"
#include "Protect.h"
#include "Motor.h"
#include "Timer.h"
#include "State_Machine.h"
#include "Foc.h"

u8 xdata Motor_Run_Flag = 0;
enum Motor_State xdata motor_state;		//状态机电机状态
enum Fault_Flag xdata fault_flag;		//电机错误码
enum Time_Count xdata time_count;
u16 restart_times;

void State_Machine(void)
{
	switch(motor_state)
	{
        case motor_init:
            Motor_Init();
            break;
        
        case motor_idle:
            Motor_Idle();
			break;
        
		case motor_start:
            Motor_Start();
			break;
        
		case motor_run:
            Motor_Run();
			break;
        
		case motor_fault:
            Motor_Fault();
			break;
        
		default:
			break;
	}
}

void Motor_Init(void)
{
    Parameter_Init();           //参数初始化
    motor_state = motor_idle;
}

void Motor_Idle(void)
{
	if(SPEED_MODE == PWM_MODE)
    {
        if(motor_speed.pwm_duty >=START_PWM_DUTY)
        {
            motor_state = motor_start;
        }
    }
}

void Motor_Start(void)
{
	if(fault_flag == 0)
    {
        Motor_Run_Flag = 1;
        motor_state = motor_run;
    }
}

void Motor_Run(void)
{
	/* 速度环 */
    Speed_Loop();
}

void Motor_Fault(void)
{
    if(MOTOR_RESTART_EN == 1)
    {
        //1.堵转重启
        if(MOTOR_STALL_RESTART_EN == 1)
        {
            if(Timer_Delay_s(MOTOR_STALL_RECOVER_TIME) == 1)  //等待延时结束
            {
                Stall_Restart();   //堵转重启检测
            }
        }
        //2.
    }
}



void Motor_Run_Stop_Control(void)
{
    
    motor_speed.target_speed = 0;
    
    if(Motor_Run_Flag == 0 && (PWMA_ENO | 0x15) != 0)
    {
        PWMA_ENO &= ~0x15;  //驱动PWM输出禁用
    }
    else if(Motor_Run_Flag == 1 && (PWMA_ENO | 0x15) == 0)
    {
        PWMA_ENO |= 0x15;   //驱动PWM输出使能
    }
}

void Stall_Restart(void)
{
    if(motor_state = motor_fault && fault_flag == motor_stall)
    {
        motor_state = motor_run;
        fault_flag = motor_normal;
    }
}