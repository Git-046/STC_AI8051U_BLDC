#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "Motor.h"
#include "Foc.h"
#include "State_Machine.h"

typedef struct
{
	Motor_Data                           motor_data;				//电机数据(机械角度、电气角度、转速、圈数)
	Motor_Speed                          motor_speed;				//电机转速相关数据(目标转速、设定转速、最大转速、最小转速、加速步长、减速步长、PWM占空比、PWM频率)

	Phase_Current                        phase_current;				//U、V、W三相电流(电流采样电阻)
	Alpha_Beta_Axis_Current              alpha_beta_current;		//α、β轴电流( Clarke变换后 )
	Direct_Quadrature_Axis_Current       dq_current;				//D、Q轴电流( Park变换后 )
	Direct_Quadrature_Axis_Current       dq_current_ref;			//D、Q轴电流需求量
	Direct_Quadrature_Axis_Voltage       dq_voltage_ctl;			//D、Q轴电压控制指令(PID)
	Direct_Quadrature_Axis_Voltage       dq_voltage_ctl_limit;		//电压圆限幅后的D、Q轴电压控制指令
	Alpha_Beta_Axis_Voltage              alpha_beta_voltage_ctl;	//α、β轴电压控制量(反Park变换)
	Phase_Voltage                        phase_voltage_ctl;			//U、V、W三相电压(反Clark变换)


	u8                                   tx_cmd_arr[TLE5012B_TX_BUFFER_SIZE];	//所有需要使用到的指令(修改接口模式寄存器的指令  请求角度数据的指令  请求角速度和圈数数据的指令)

	float                                v_bus;						//母线电压
	PID_Structure                        pid_d;						//D轴电流PID
	PID_Structure                        pid_q;						//Q轴电流PID
	PID_Structure                        pid_speed;					//速度PID

	Speed_Ramp                           speed_ramp;				//速度斜坡

	u8                                   motor_run_flag;			//电机运行标志位(0:电机停止  1:电机运行)
	enum Motor_State                     motor_state;				//电机状态机(初始、空闲、启动、运行、错误)
	enum Fault_Flag                      fault_flag;				//错误状态码(正常、堵转、过流、欠压、过压、缺相)

	u16                                  restart_times;				//电机重启次数(当发生错误时，尝试重启电机，重启次数超过设定值后不再尝试重启)
} System_Context;

extern System_Context xdata *g_ctx;

#endif
