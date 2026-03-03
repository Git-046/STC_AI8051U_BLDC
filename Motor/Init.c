#include "Init.h"

u8 xdata set_pwm_duty = 0;	//设定占空比：0~1000 对应 0.0~100.0%

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

 void Parameter_Init(void)
 {
     /*Motor Parameter*/
     memset((void*)&motor_data, 0, sizeof(Motor_Data));
     memset((void*)&motor_speed, 0, sizeof(Motor_Speed));
     
     memset((void*)&phase_current, 0, sizeof(Phase_Current));
     memset((void*)&alpha_beta_current, 0, sizeof(Alpha_Beta_Axis_Current));
     memset((void*)&dq_current, 0, sizeof(Direct_Quadrature_Axis_Current));
     
     memset((void*)&dq_current_ref, 0, sizeof(Direct_Quadrature_Axis_Current));
     memset((void*)&dq_voltage_ctl, 0, sizeof(Direct_Quadrature_Axis_Voltage));
     memset((void*)&dq_voltage_ctl_limit, 0, sizeof(Direct_Quadrature_Axis_Voltage));
     memset((void*)&alpha_beta_voltage_ctl, 0, sizeof(Alpha_Beta_Axis_Voltage));
     memset((void*)&phase_voltage_ctl, 0, sizeof(Phase_Voltage));
     
     memset((void*)ADC_Result_Buffer, 0, sizeof(u16)*4);
     
     memset((void*)SPI_TX_Buffer, 0, sizeof(u8)*4);
     memset((void*)SPI_RX_Buffer, 0, sizeof(u8)*6);
     memset((void*)Tx_cmd_arr, 0, sizeof(u8)*TLE5012B_TX_BUFFER_SIZE);
     memset((void*)&tle5012b_data, 0, sizeof(TLE5012B_Data));
     
     motor_speed.max_speed = SPEED_MAX;
     motor_speed.min_speed = SPEED_MIN;
     motor_speed.acceleration_speed = SPEED_ACCERELATION;
     motor_speed.deceleration_speed = SPEED_DECERELATION;
     
	 /*PID Parameter*/
     memset((void*)&pid_structure_d, 0, sizeof(PID_Structure));
     memset((void*)&pid_structure_q, 0, sizeof(PID_Structure));
     memset((void*)&pid_structure_speed, 0, sizeof(PID_Structure));
     
     pid_structure_d.P_gain = PID_ID_KP;
     pid_structure_d.I_gain = PID_ID_KI;
     pid_structure_d.D_gain = PID_ID_KD;
     
     pid_structure_q.P_gain = PID_IQ_KP;
     pid_structure_q.I_gain = PID_IQ_KI;
     pid_structure_q.D_gain = PID_IQ_KD;
     
     pid_structure_speed.P_gain = PID_S_KP;
     pid_structure_speed.I_gain = PID_S_KI;
     pid_structure_speed.D_gain = PID_S_KD;
     
     memset((void*)&speed_ramp, 0, sizeof(Speed_Ramp));
     
     
 }
 