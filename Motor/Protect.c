#include "AI8051U.h"
#include "Protect.h"
#include "Motor.h"
#include "State_Machine.h"

void Current_Limit(float xdata *Q_axis_current)
{
    if(*Q_axis_current > CURRENT_LIMIT_VALUE)
    {
        *Q_axis_current = CURRENT_LIMIT_VALUE;
    }        
    else if(*Q_axis_current < -CURRENT_LIMIT_VALUE)
    {
        *Q_axis_current = -CURRENT_LIMIT_VALUE;
    }
}

void Over_Current_Protect(void)
{
    static u16 time_count = 0;
    if(dq_current.current_q > OVER_CURRENT_PROTECT_VALUE)
    {
        time_count++;
        if(time_count >= OVER_CURRENT_DETECT_TIME)
        {
            motor_state = motor_fault;
            fault_flag = motor_over_current;
        }
    }
    else
    {
        time_count = 0;
    }
}

void Motor_Stall_Protect(void)
{
    static u16 time_count;
    if(motor_speed.motor_speed < 0 && motor_speed.motor_speed > -motor_speed.min_speed)
    {
        time_count++;
        if(time_count >= MOTOR_STALL_DETECT_TIME)
        {
            motor_state = motor_fault;
            fault_flag = motor_stall;
        }
    }
    else if(motor_speed.motor_speed > 0 && motor_speed.motor_speed < motor_speed.min_speed)
    {
        time_count++;
        if(time_count >= MOTOR_STALL_DETECT_TIME)
        {
            motor_state = motor_fault;
            fault_flag = motor_stall;
        }
    }
    else
    {
        time_count = 0;
    }
}