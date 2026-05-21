#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "Motor.h"
#include "Foc.h"
#include "State_Machine.h"

typedef struct
{
	Motor_Data                           motor_data;
	Motor_Speed                          motor_speed;

	Phase_Current                        phase_current;
	Alpha_Beta_Axis_Current              alpha_beta_current;
	Direct_Quadrature_Axis_Current       dq_current;
	Direct_Quadrature_Axis_Current       dq_current_ref;
	Direct_Quadrature_Axis_Voltage       dq_voltage_ctl;
	Direct_Quadrature_Axis_Voltage       dq_voltage_ctl_limit;
	Alpha_Beta_Axis_Voltage              alpha_beta_voltage_ctl;
	Phase_Voltage                        phase_voltage_ctl;

	u8                                   tx_cmd_arr[TLE5012B_TX_BUFFER_SIZE];

	float                                v_bus;
	PID_Structure                        pid_d;
	PID_Structure                        pid_q;
	PID_Structure                        pid_speed;
	Speed_Ramp                           speed_ramp;

	u8                                   motor_run_flag;
	enum Motor_State                     motor_state;
	enum Fault_Flag                      fault_flag;
	u16                                  restart_times;
} System_Context;

extern System_Context xdata *g_ctx;

#endif
