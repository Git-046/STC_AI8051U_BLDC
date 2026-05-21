#include "AI8051U.h"
#include "Motor.h"
#include "def.h"
#include "string.h"
#include "State_Machine.h"

/*定义全局变量*/
Motor_Data xdata motor_data;
Motor_Speed xdata motor_speed;

Phase_Current xdata phase_current;					    //U、V、W三相电流(电流采样电阻)
Alpha_Beta_Axis_Current xdata alpha_beta_current;	    //α、β轴电流(Clark变化)
Direct_Quadrature_Axis_Current xdata dq_current;		//D、Q轴电流(Park变换)

Direct_Quadrature_Axis_Current xdata dq_current_ref;            //D、Q轴电流需求量
Direct_Quadrature_Axis_Voltage xdata dq_voltage_ctl;            //D、Q轴电压控制指令(PID)
Direct_Quadrature_Axis_Voltage xdata dq_voltage_ctl_limit;      //电压圆限幅后的电压控制指令
Alpha_Beta_Axis_Voltage xdata alpha_beta_voltage_ctl;	        //α、β轴电压控制量(反Park变换)
Phase_Voltage xdata phase_voltage_ctl;					        //U、V、W三相电压(反Clark变换)

extern u16 xdata ADC_Result_Buffer[4];	//电流采样

u8 xdata SPI_TX_Buffer[4];							//向磁编码器请求数据发送的控制指令(0x8021请求角度数据  0x8032请求角速度和圈数数据)
u8 xdata SPI_RX_Buffer[6];							//从磁编码器获取到的数据
u8 xdata Tx_cmd_arr[TLE5012B_TX_BUFFER_SIZE];		//TLE5012B控制指令数组
u8 xdata Rx_data_arr[TLE5012B_RX_BUFFER_SIZE];		//磁编码器数据数组
TLE5012B_Data tle5012b_data;						//从磁编码器读取到的结构数据

void TLE5012B_Init()
{
	//0.预先存储TLE5012B的控制指令
	Tx_cmd_arr[0] = SET_MODE2_CMD >> 2;
	Tx_cmd_arr[1] = SET_MODE2_CMD;
	
	Tx_cmd_arr[2] = MODE2_PARAM >> 2;
	Tx_cmd_arr[3] = MODE2_PARAM;
	
	Tx_cmd_arr[4] = TLE5012B_READ_ANGLE_CMD >> 2;
	Tx_cmd_arr[5] = TLE5012B_READ_ANGLE_CMD;
	
	Tx_cmd_arr[6] = TLE5021B_READ_SPEED_AND_LOOP_CMD >> 2;
	Tx_cmd_arr[7] = TLE5021B_READ_SPEED_AND_LOOP_CMD;
	
	//1.设定磁编码器工作模式
	SPI_TX_Buffer[0] = Tx_cmd_arr[0];
	SPI_TX_Buffer[1] = Tx_cmd_arr[1];
	while(SPI_RX_Buffer[0] == 0 && SPI_RX_Buffer[1] == 0) ;	//等待返回安全字
	
	SPI_RX_Buffer[0] = 0;
	SPI_RX_Buffer[1] = 0;
	SPI_TX_Buffer[0] = Tx_cmd_arr[2];
	SPI_TX_Buffer[1] = Tx_cmd_arr[3];
	while(SPI_RX_Buffer[0] == 0 && SPI_RX_Buffer[1] == 0) ;	//等待返回安全字
}
