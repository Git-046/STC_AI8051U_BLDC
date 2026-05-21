#ifndef __Motor_H
#define __Motor_H

#include "def.h"

/*=======================���Ӳ������=====================*/
#define	MOTOR_POLE_PAIRS			6			//���������
#define MOTOR_PHASE_RESISTANCE		0.000512	//����裨����
#define MOTOR_PHASE_INDUCTANCE		0.000640	//���У�H��

#define MOTOR_VOLTAGE_RATED			12			//������ѹ��V��
#define MOTOR_BACK_EMF				0.05		//���綯�ƣ�V��

#define SINGLE_RESISTANCE_MODE		1							//���������
#define DOUBLE_RESISTANCE_MODE		2							//˫�������
#define THREE_RESISTANCE_MODE		3							//���������
#define CURRENT_SAMPLE_MODE			(THREE_RESISTANCE_MODE)		//ѡ���������ģʽ
#define PHASE_SAMPLE_RESISTANCE		0.01						//������������裨����
#define BUS_SAMPLE_RESISTANCE		0.01						//ĸ�ߵ����������裨����

#define V_BUS_VOLTAGE_DIVISION_RATIO    8.5                     //ĸ�ߵ�ѹ ��ѹ��
#define ADC_V_REF                       5.0                     //ADC�ο���ѹ

/*=========================================���Ʋ���====================================*/
#define PWM_FREQUENT				20000			//PWM�ز�Ƶ��(20KHz)
#define CURRENT_LOOP_FREQUENT		20000			//�������Ŀ����������ز����ڱ���һ��
#define SPEED_LOOP_FREQUENT			2000			//�ٶȻ��Ŀ�������Ϊ��������10��
#define PID_I_Limit					100				//PID�����޷�

#define SOFTWARE_MODE       0   
#define PWM_MODE            1
#define CLK_MODE            2
#define VSP_MODE            3
#define SPEED_MODE          PWM_MODE

#define PID_ID_KP      0.60              //������D���������
#define PID_ID_KI      0.01              //������D���������
#define PID_ID_KD      0.00              //������D��΢������

#define PID_IQ_KP      0.60              //������Q���������
#define PID_IQ_KI      0.01              //������Q���������
#define PID_IQ_KD      0.00              //������Q��΢������

#define PID_S_KP      0.60              //�ٶȻ���������
#define PID_S_KI      0.01              //�ٶȻ���������
#define PID_S_KD      0.00              //�ٶȻ�΢������

#define START_PWM_DUTY          50          //����ռ�ձ� 5%
#define STOP_PWM_DUTY           30          //ֹͣռ�ձ� 3%
#define FULL_SPEED_PWM_DUTY     980         //ȫ��ռ�ձ� 98%
#define PWM_DUTY_BASE           1000        //ռ�ձȻ�׼

/*===================������в���===================*/
#define SPEED_MIN       200         //��Сת��(RPM)
#define SPEED_MAX       1500        //���ת��(RPM)
#define SPEED_BASE      5000        //ת�ٻ�׼(RPM)

#define SPEED_ACCELERATION      50      //�ٶ�����
#define SPEED_DECELERATION     -50     //�ٶȼ���

/*==================�ṹ�嶨��==================*/
typedef struct
{
	float machine_angle;	//��е�Ƕȣ�rad��
    float electrical_angle; //�����Ƕȣ�rad��
	int32 speed;            //ת�٣�r/min��
	s16 loop;		        //Ȧ��
}Motor_Data;

typedef struct
{
    int32 motor_speed;         //���ת��(rpm)
    int32 target_speed;        //Ŀ��ת��(rpm)
    int32 set_speed;           //�趨ת��(rpm)
    
    int32 max_speed;           //���ת��(rpm)
    int32 min_speed;           //��Сת��(rpm)
    int16 acceleration_speed;  //���ٲ���(rpm)
    int16 deceleration_speed;  //���ٲ���(rpm)
    //uint16 acceleration_time;   //����ʱ��(us)
    uint16 pwm_duty;            //PWMռ�ձ�
    u32    pwm_freq;            //PWMƵ�ʣ�Hz��
}Motor_Speed;

typedef struct
{
	float voltage_u;
	float voltage_v;
	float voltage_w;
}Phase_Voltage;		//U��V��W�����ѹ

typedef struct
{
	float current_u;
	float current_v;
	float current_w;
	float current_u_filtered;
	float current_v_filtered;
	float current_w_filtered;
}Phase_Current;		//U��V��W�������

typedef struct
{
	float voltage_alpha;
	float voltage_beta;
}Alpha_Beta_Axis_Voltage;	//���������ѹ

typedef struct
{
	float current_alpha;
	float current_beta;
}Alpha_Beta_Axis_Current;	//�����������

typedef struct
{
	float voltage_d;
	float voltage_q;
}Direct_Quadrature_Axis_Voltage;	//D��Q���ѹ

typedef struct
{
	float current_d;
	float current_q;
}Direct_Quadrature_Axis_Current;	//D��Q�����


/*=======================================�ű�����tle5012b==================================*/
#define SET_MODE2_CMD							0x5081		//�޸Ľӿ�2ģʽ�Ĵ�����ָ��
#define MODE2_PARAM								0x0809		//д��ű������ӿ�ģʽ2�ľ���ָ��
#define TLE5012B_READ_ANGLE_CMD					0x8021		//��ȡ�Ƕȵ�ָ��
#define TLE5021B_READ_SPEED_AND_LOOP_CMD		0x8032		//��ȡ�ٶȺ���תȦ����ָ��

#define TLE5012B_TX_BUFFER_SIZE		8			//TLE5012Bָ��洢����Ĵ�С��3��16λָ�
#define TLE5012B_RX_BUFFER_SIZE		8			//�洢TLE5012B���ݵĻ������飨3��16λ���ݣ�


/*=========================================��������=========================================*/
void TLE5012B_Init(void);


/*===============ȫ�ֱ�������==============*/



extern u8 xdata SPI_TX_Buffer[8];
extern u8 xdata SPI_RX_Buffer[4];
extern u16 xdata ADC_Result_Buffer[4];	//��������


#endif