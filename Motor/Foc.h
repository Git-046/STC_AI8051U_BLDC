#include "Motor.h"

#ifndef __FOC_H
#define __FOC_H

//�ṹ��
typedef struct
{
	float error;		//���
	float pre_error;	//ǰһ�ε����
	float P_gain;		//��������
	float I_gain;		//��������
	float D_gain;		//΢������
	float P_out;		//�������ڵ����
	float I_out;		//���ֻ��ڵ����
	float D_out;		//΢�ֻ��ڵ����
	float I_sum;		//������
    float I_limit;      //�����޷�
	float I_period;		//��������
    u8    I_EN_FLAG;    //����ʹ�ܱ�־λ
	float Kc_gain;		//�����ֱ��Ͳ�������
	float output_limit;	//������
	float output_ref;	//Ԥ�Ƶ������
	float output;		//ʵ�ʵ������
}PID_Structure;

typedef struct
{
	uint32_t target_speed;					//Ŀ��ת��
	uint32_t slope_speed;					//б���ٶ�(��ǰת��)
	uint16_t accelerate_amount;				//������
	uint16_t decelerate_amount;				//������
	uint16_t accelerate_decelerate_time;	//�Ӽ���ʱ��
	uint16_t execution_frequency;			//ִ��Ƶ��(�ٶȻ�Ƶ��)
}Speed_Ramp;



//��������
void Current_Loop(void);
void PID(PID_Structure xdata *pid_structure);
void SVPWM(Alpha_Beta_Axis_Voltage xdata *alpha_beta_voltage, float v_bus, float T_pwm);
void First_Order_LPF(Phase_Current xdata *phase_current, float alpha);

void Speed_Update(Motor_Speed xdata *speed);
void Motor_Speed_Ramp(Motor_Speed xdata *speed);
void Speed_Loop(void);
    
#endif
