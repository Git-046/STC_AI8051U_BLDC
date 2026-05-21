#include "Context.h"
#include "AI8051U.h"
#include "Motor.h"
#include "def.h"
#include "string.h"

#include "State_Machine.h"

u8 xdata SPI_TX_Buffer[8];
u8 xdata SPI_RX_Buffer[4];

void TLE5012B_Init()
{
	//0.Ԥ�ȴ洢TLE5012B�Ŀ���ָ��
	g_ctx->tx_cmd_arr[0] = SET_MODE2_CMD >> 2;
	g_ctx->tx_cmd_arr[1] = SET_MODE2_CMD;
	
	g_ctx->tx_cmd_arr[2] = MODE2_PARAM >> 2;
	g_ctx->tx_cmd_arr[3] = MODE2_PARAM;
	
	g_ctx->tx_cmd_arr[4] = TLE5012B_READ_ANGLE_CMD >> 2;
	g_ctx->tx_cmd_arr[5] = TLE5012B_READ_ANGLE_CMD;
	
	g_ctx->tx_cmd_arr[6] = TLE5021B_READ_SPEED_AND_LOOP_CMD >> 2;
	g_ctx->tx_cmd_arr[7] = TLE5021B_READ_SPEED_AND_LOOP_CMD;
	
	//1.�趨�ű���������ģʽ
	SPI_TX_Buffer[0] = g_ctx->tx_cmd_arr[0];
	SPI_TX_Buffer[1] = g_ctx->tx_cmd_arr[1];
	while(SPI_RX_Buffer[0] == 0 && SPI_RX_Buffer[1] == 0) ;	//�ȴ����ذ�ȫ��
	
	SPI_RX_Buffer[0] = 0;
	SPI_RX_Buffer[1] = 0;
	SPI_TX_Buffer[0] = g_ctx->tx_cmd_arr[2];
	SPI_TX_Buffer[1] = g_ctx->tx_cmd_arr[3];
	while(SPI_RX_Buffer[0] == 0 && SPI_RX_Buffer[1] == 0) ;	//�ȴ����ذ�ȫ��
}
