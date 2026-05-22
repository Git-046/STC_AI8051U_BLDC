#include "def.h"

#ifndef __Motor_H
#define __Motor_H

/*=======================电机硬件参数=====================*/
#define	MOTOR_POLE_PAIRS			6			//电机极对数
#define MOTOR_PHASE_RESISTANCE		0.000512	//相电阻（Ω）
#define MOTOR_PHASE_INDUCTANCE		0.000640	//相电感（H）

#define MOTOR_VOLTAGE_RATED			12			//电机额定电压（V）
#define MOTOR_BACK_EMF				0.05		//反电动势（V）

#define SINGLE_RESISTANCE_MODE		1							//单电阻采样
#define DOUBLE_RESISTANCE_MODE		2							//双电阻采样
#define THREE_RESISTANCE_MODE		3							//三电阻采样
#define CURRENT_SAMPLE_MODE			(THREE_RESISTANCE_MODE)		//选择电流采样模式
#define PHASE_SAMPLE_RESISTANCE		0.01						//相电流采样电阻（Ω）
#define BUS_SAMPLE_RESISTANCE		0.01						//母线电流采样电阻（Ω）

#define V_BUS_VOLTAGE_DIVISION_RATIO    8.5                     //母线电压 分压比
#define ADC_V_REF                       5.0                     //ADC参考电压

/*=========================================控制参数====================================*/
#define PWM_FREQUENT				20000			//PWM载波频率(20KHz)
#define CURRENT_LOOP_FREQUENT		20000			//电流环的控制周期与载波周期保持一致
#define SPEED_LOOP_FREQUENT			2000			//速度环的控制周期为电流环的10倍
#define PID_I_Limit					100				//PID积分限幅

#define SOFTWARE_MODE       0   
#define PWM_MODE            1
#define CLK_MODE            2
#define VSP_MODE            3
#define SPEED_MODE          PWM_MODE

#define PID_ID_KP      0.60              //电流环D轴比例增益
#define PID_ID_KI      0.01              //电流环D轴积分增益
#define PID_ID_KD      0.00              //电流环D轴微分增益

#define PID_IQ_KP      0.60              //电流环Q轴比例增益
#define PID_IQ_KI      0.01              //电流环Q轴积分增益
#define PID_IQ_KD      0.00              //电流环Q轴微分增益

#define PID_S_KP      0.60              //速度环比例增益
#define PID_S_KI      0.01              //速度环积分增益
#define PID_S_KD      0.00              //速度环微分增益

#define START_PWM_DUTY          50          //启动占空比 5%
#define STOP_PWM_DUTY           30          //停止占空比 3%
#define FULL_SPEED_PWM_DUTY     980         //全速占空比 98%
#define PWM_DUTY_BASE           1000        //占空比基准

/*===================电机运行参数===================*/
#define SPEED_MIN       200         //最小转速(RPM)
#define SPEED_MAX       1500        //最大转速(RPM)
#define SPEED_BASE      5000        //转速基准(RPM)

#define SPEED_ACCERELATION      50      //速度增量
#define SPEED_DECERELATION		50     	//速度减量

/*==================结构体定义==================*/
typedef struct
{
	float machine_angle;	//机械角度（rad）
    float electrical_angle; //电气角度（rad）
	int32 speed;            //转速（r/min）
	s16 loop;		        //圈数
}Motor_Data;

typedef struct
{
    int32 motor_speed;         //电机转速(rpm)
    int32 target_speed;        //目标转速(rpm)
    int32 set_speed;           //设定转速(rpm)
    
    int32 max_speed;           //最大转速(rpm)
    int32 min_speed;           //最小转速(rpm)
    int16 acceleration_speed;  //加速步长(rpm)
    int16 deceleration_speed;  //减速步长(rpm)
    //uint16 acceleration_time;   //加速时间(us)
    uint16 pwm_duty;            //PWM占空比
    u32    pwm_freq;            //PWM频率（Hz）
}Motor_Speed;

typedef struct
{
	float voltage_u;
	float voltage_v;
	float voltage_w;
}Phase_Voltage;		//U、V、W三相电压

typedef struct
{
	float current_u;
	float current_v;
	float current_w;
	float current_u_filtered;
	float current_v_filtered;
	float current_w_filtered;
	float last_value;
}Phase_Current;		//U、V、W三相电流

typedef struct
{
	float voltage_alpha;
	float voltage_beta;
}Alpha_Beta_Axis_Voltage;	//α、β轴电压

typedef struct
{
	float current_alpha;
	float current_beta;
}Alpha_Beta_Axis_Current;	//α、β轴电流

typedef struct
{
	float voltage_d;
	float voltage_q;
}Direct_Quadrature_Axis_Voltage;	//D、Q轴电压

typedef struct
{
	float current_d;
	float current_q;
}Direct_Quadrature_Axis_Current;	//D、Q轴电流


/*=======================================磁编码器tle5012b==================================*/
#define SET_MODE2_CMD							0x5081		//修改接口2模式寄存器的指令
#define MODE2_PARAM								0x0809		//写入磁编码器接口模式2的具体指令
#define TLE5012B_READ_ANGLE_CMD					0x8021		//读取角度的指令
#define TLE5021B_READ_SPEED_AND_LOOP_CMD		0x8032		//读取速度和旋转圈数的指令

#define TLE5012B_TX_BUFFER_SIZE		8			//TLE5012B指令存储数组的大小
#define TLE5012B_RX_BUFFER_SIZE		8			//存储TLE5012B数据的缓存数组（3锟斤拷16位锟斤拷锟捷ｏ拷


/*=========================================磁编码器初始化=========================================*/
void TLE5012B_Init(void);

extern u16 xdata ADC_Result_Buffer[4];	//电流采样

extern u8 xdata SPI_TX_Buffer[8];	//向磁编码器请求数据发送的控制指令(0x8021请求角度数据  0x8032请求角速度和圈数数据)
extern u8 xdata SPI_RX_Buffer[4];	//存储从磁编码器接收到的数据

#endif