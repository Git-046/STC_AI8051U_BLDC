#include "Context.h"
#include "Init.h"


void System_Init(void)
{
	//1. 初始化系统时钟
	HSPllClkConfig(MCKSEL_HIRC, PLL_SEL, 1);
	
	//2. 初始化外设
    GPIO_Init();
    ADC_Init();
	PWM_Init();
    SPI_Init();
	DMA_ADC_Init();
	DMA_SPI_Init();
}

/*------------------------------------------------------------
@ function  :   void Parameter_Init(void)
@ describe  :   参数初始化
@ parameter :   None
@ date      :   2026-3-1
-------------------------------------------------------------*/
void Parameter_Init(void)
{
    g_ctx->restart_times = MOTOR_RESTART_TIMES;

    /*Motor Parameter*/
    memset((void*)&g_ctx->motor_data, 0, sizeof(Motor_Data));
    memset((void*)&g_ctx->motor_speed, 0, sizeof(Motor_Speed));

    memset((void*)&g_ctx->phase_current, 0, sizeof(Phase_Current));
    memset((void*)&g_ctx->alpha_beta_current, 0, sizeof(Alpha_Beta_Axis_Current));
    memset((void*)&g_ctx->dq_current, 0, sizeof(Direct_Quadrature_Axis_Current));

    memset((void*)&g_ctx->dq_current_ref, 0, sizeof(Direct_Quadrature_Axis_Current));
    memset((void*)&g_ctx->dq_voltage_ctl, 0, sizeof(Direct_Quadrature_Axis_Voltage));
    memset((void*)&g_ctx->dq_voltage_ctl_limit, 0, sizeof(Direct_Quadrature_Axis_Voltage));
    memset((void*)&g_ctx->alpha_beta_voltage_ctl, 0, sizeof(Alpha_Beta_Axis_Voltage));
    memset((void*)&g_ctx->phase_voltage_ctl, 0, sizeof(Phase_Voltage));

    memset((void*)ADC_Result_Buffer, 0, sizeof(u16)*4);

    memset((void*)SPI_TX_Buffer, 0, sizeof(u8)*8);
    memset((void*)SPI_RX_Buffer, 0, sizeof(u8)*4);
    memset((void*)g_ctx->tx_cmd_arr, 0, sizeof(u8)*TLE5012B_TX_BUFFER_SIZE);

    g_ctx->motor_speed.max_speed = SPEED_MAX;
    g_ctx->motor_speed.min_speed = SPEED_MIN;
    g_ctx->motor_speed.acceleration_speed = SPEED_ACCELERATION;
    g_ctx->motor_speed.deceleration_speed = SPEED_DECELERATION;

    /*PID Parameter*/
    memset((void*)&g_ctx->pid_d, 0, sizeof(PID_Structure));
    memset((void*)&g_ctx->pid_q, 0, sizeof(PID_Structure));
    memset((void*)&g_ctx->pid_speed, 0, sizeof(PID_Structure));

    g_ctx->pid_d.P_gain = PID_ID_KP;
    g_ctx->pid_d.I_gain = PID_ID_KI;
    g_ctx->pid_d.D_gain = PID_ID_KD;

    g_ctx->pid_q.P_gain = PID_IQ_KP;
    g_ctx->pid_q.I_gain = PID_IQ_KI;
    g_ctx->pid_q.D_gain = PID_IQ_KD;

    g_ctx->pid_speed.P_gain = PID_S_KP;
    g_ctx->pid_speed.I_gain = PID_S_KI;
    g_ctx->pid_speed.D_gain = PID_S_KD;

    memset((void*)&g_ctx->speed_ramp, 0, sizeof(Speed_Ramp));
    g_ctx->speed_ramp.accelerate_amount = SPEED_ACCELERATION;
    g_ctx->speed_ramp.decelerate_amount = SPEED_DECELERATION;
}
 