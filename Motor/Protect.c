#include "Context.h"
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
    if(g_ctx->dq_current.current_q > OVER_CURRENT_PROTECT_VALUE)
    {
        time_count++;
        if(time_count >= OVER_CURRENT_DETECT_TIME)
        {
            g_ctx->motor_state = motor_fault;
            g_ctx->fault_flag = motor_over_current;
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
    if(g_ctx->motor_speed.motor_speed < 0 && g_ctx->motor_speed.motor_speed > -g_ctx->motor_speed.min_speed)
    {
        time_count++;
        if(time_count >= MOTOR_STALL_DETECT_TIME)
        {
            g_ctx->motor_state = motor_fault;
            g_ctx->fault_flag = motor_stall;
        }
    }
    else if(g_ctx->motor_speed.motor_speed > 0 && g_ctx->motor_speed.motor_speed < g_ctx->motor_speed.min_speed)
    {
        time_count++;
        if(time_count >= MOTOR_STALL_DETECT_TIME)
        {
            g_ctx->motor_state = motor_fault;
            g_ctx->fault_flag = motor_stall;
        }
    }
    else
    {
        time_count = 0;
    }
}