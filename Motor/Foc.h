#include "Motor.h"

#ifndef __FOC_H
#define __FOC_H

//结构体
typedef struct
{
	float error;		//误差
	float pre_error;	//前一次的误差
	float P_gain;		//比例增益
	float I_gain;		//积分增益
	float D_gain;		//微分增益
	float P_out;		//比例环节的输出
	float I_out;		//积分环节的输出
	float D_out;		//微分环节的输出
	float I_sum;		//误差积分
    float I_limit;      //积分限幅
	float I_period;		//积分周期
    u8    I_EN_FLAG;    //积分使能标志位
	float Kc_gain;		//抗积分饱和补偿增益
	float output_limit;	//最大输出
	float output_ref;	//预计的输出量
	float output;		//实际的输出量
}PID_Structure;

typedef struct
{
	uint32_t target_speed;					//目标转速
	uint32_t slope_speed;					//斜坡速度(当前转速)
	uint16_t accelerate_amount;				//加速量
	uint16_t decelerate_amount;				//减速量
	uint16_t accelerate_decelerate_time;	//加减速时间
	uint16_t execution_frequency;			//执行频率(速度环频率)
}Speed_Ramp;

//锟斤拷锟斤拷锟斤拷锟斤拷
void Current_Loop(void);
void PID(PID_Structure xdata *pid_structure);
void SVPWM(Alpha_Beta_Axis_Voltage xdata *alpha_beta_voltage, float v_bus, float T_pwm);
void First_Order_LPF(Phase_Current xdata *phase_current, float alpha);

void Speed_Update(Motor_Speed xdata *speed);
void Motor_Speed_Ramp(Motor_Speed xdata *speed);
void Speed_Loop(void);
    
#endif
