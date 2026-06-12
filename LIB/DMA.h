/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef	__DMA_H
#define	__DMA_H

#include "config.h"

//========================================================================
//                              定义声明
//========================================================================

#define DMA_ADC_TRIG()				DMA_ADC_CR |= 0x40
#define DMA_ADCIF_CLR()				DMA_ADC_STA &= ~0x01

#define DMA_M2M_TRIG()				DMA_M2M_CR |= 0x40
#define DMA_M2MIF_CLR()				DMA_M2M_STA &= ~0x01

#define DMA_UR1T_TRIG()				DMA_UR1T_CR |= 0x40
#define DMA_UR2T_TRIG()				DMA_UR2T_CR |= 0x40
#define DMA_UR3T_TRIG()				DMA_UR3T_CR |= 0x40
#define DMA_UR4T_TRIG()				DMA_UR4T_CR |= 0x40

#define DMA_UR1R_TRIG()				DMA_UR1R_CR |= 0x20
#define DMA_UR2R_TRIG()				DMA_UR2R_CR |= 0x20
#define DMA_UR3R_TRIG()				DMA_UR3R_CR |= 0x20
#define DMA_UR4R_TRIG()				DMA_UR4R_CR |= 0x20

#define DMA_SPI_TRIG_M()			DMA_SPI_CR |= 0x40		//SPI触发主机模式
#define DMA_SPI_TRIG_S()			DMA_SPI_CR |= 0x20		//SPI触发从机模式
#define SET_SPI_DMA_LEN(n)			{DMA_SPI_AMTH = (n>>8); DMA_SPI_AMT = (n);}

#define DMA_UR1R_CLRFIFO()			DMA_UR1R_CR |= 0x01	    //清空 DMA FIFO
#define DMA_UR2R_CLRFIFO()			DMA_UR2R_CR |= 0x01
#define DMA_UR3R_CLRFIFO()			DMA_UR3R_CR |= 0x01
#define DMA_UR4R_CLRFIFO()			DMA_UR4R_CR |= 0x01
#define DMA_SPI_CLRFIFO()			DMA_SPI_CR |= 0x01
#define DMA_LCM_CLRFIFO()			DMA_LCM_CR |= 0x01

#define DMA_ADC_CLR_STA()			DMA_ADC_STA = 0
#define DMA_SPI_CLR_STA()			DMA_SPI_STA = 0
#define DMA_M2M_CLR_STA()			DMA_M2M_STA = 0
#define DMA_UR1T_CLR_STA()			DMA_UR1T_STA = 0
#define DMA_UR1R_CLR_STA()			DMA_UR1R_STA = 0
#define DMA_UR2T_CLR_STA()			DMA_UR2T_STA = 0
#define DMA_UR2R_CLR_STA()			DMA_UR2R_STA = 0
#define DMA_UR3T_CLR_STA()			DMA_UR3T_STA = 0
#define DMA_UR3R_CLR_STA()			DMA_UR3R_STA = 0
#define DMA_UR4T_CLR_STA()			DMA_UR4T_STA = 0
#define DMA_UR4R_CLR_STA()			DMA_UR4R_STA = 0

#define SET_M2M_TX_FIFO(n)			{DMA_M2M_TXAH = (n>>8); DMA_M2M_TXAL = (n);}
#define SET_M2M_RX_FIFO(n)			{DMA_M2M_RXAH = (n>>8); DMA_M2M_RXAL = (n);}

#define SET_DMA_ADC_CR(n)			DMA_ADC_CR = (n)
#define SET_DMA_SPI_CR(n)			DMA_SPI_CR = (n)
#define SET_DMA_M2M_CR(n)			DMA_M2M_CR = (n)
#define SET_DMA_UR1T_CR(n)			DMA_UR1T_CR = (n)
#define SET_DMA_UR1R_CR(n)			DMA_UR1R_CR = (n)
#define SET_DMA_UR2T_CR(n)			DMA_UR2T_CR = (n)
#define SET_DMA_UR2R_CR(n)			DMA_UR2R_CR = (n)
#define SET_DMA_UR3T_CR(n)			DMA_UR3T_CR = (n)
#define SET_DMA_UR3R_CR(n)			DMA_UR3R_CR = (n)
#define SET_DMA_UR4T_CR(n)			DMA_UR4T_CR = (n)
#define SET_DMA_UR4R_CR(n)			DMA_UR4R_CR = (n)

#define SET_LCM_DMA_LEN(n)			{DMA_LCM_AMTH = (n>>8); DMA_LCM_AMT = (n);}
#define DMA_LCM_TRIG_WC()			DMA_LCM_CR |= 0xC0		//触发 LCM_DMA 发命令操作
#define DMA_LCM_TRIG_WD()			DMA_LCM_CR |= 0xA0		//触发 LCM_DMA 发数据操作
#define DMA_LCM_TRIG_RC()			DMA_LCM_CR |= 0x90		//触发 LCM_DMA 读命令操作
#define DMA_LCM_TRIG_RD()			DMA_LCM_CR |= 0x88		//触发 LCM_DMA 读数据操作

#define DMA_I2CT_TRIG()				DMA_I2CT_CR |= 0x40
#define DMA_I2CR_TRIG()				DMA_I2CR_CR |= 0x40
#define DMA_I2CR_CLRFIFO()			DMA_I2CR_CR |= 0x01	//清空 DMA FIFO
#define SET_I2CT_DMA_LEN(n)			{DMA_I2CT_AMTH = (n>>8); DMA_I2CT_AMT = (n);}
#define SET_I2CR_DMA_LEN(n)			{DMA_I2CR_AMTH = (n>>8); DMA_I2CR_AMT = (n);}
#define SET_I2C_DMA_ST(n)			{DMA_I2C_ST2 = (n>>8); DMA_I2C_ST1 = (n);}
#define I2C_DMA_Enable()			DMA_I2C_CR |= 0x01
#define I2C_DMA_Disable()			DMA_I2C_CR &= ~0x01

//========================================================================
//                              常量声明
//========================================================================

#define DMA_DISABLE				0x00
#define DMA_ENABLE				0x80
#define M2M_TRIG				0x40
#define ADC_TRIG				0x40
#define UR_T_TRIG				0x40
#define UR_R_TRIG				0x20
#define SPI_TRIG_M				0x40
#define SPI_TRIG_S				0x20
#define CLR_FIFO				0x01

#define ADC_1_Times				0
#define ADC_2_Times				8
#define ADC_4_Times				9
#define ADC_8_Times				10
#define ADC_16_Times			11
#define ADC_32_Times			12
#define ADC_64_Times			13
#define ADC_128_Times			14
#define ADC_256_Times			15

#define M2M_ADDR_INC			0
#define M2M_ADDR_DEC			1

#define SPI_SS_P14				0
#define SPI_SS_P24				1
#define SPI_SS_P40				2
#define SPI_SS_P35				3

#define ADC_DMA_CHANNEL_0		1
#define ADC_DMA_CHANNEL_1		(1<<1)
#define ADC_DMA_CHANNEL_2		(1<<2)
#define ADC_DMA_CHANNEL_3		(1<<3)
#define ADC_DMA_CHANNEL_4		(1<<4)
#define ADC_DMA_CHANNEL_5		(1<<5)
#define ADC_DMA_CHANNEL_6		(1<<6)
#define ADC_DMA_CHANNEL_7		(1<<7)
#define ADC_DMA_CHANNEL_8		(1<<8)
#define ADC_DMA_CHANNEL_9		(1<<9)
#define ADC_DMA_CHANNEL_10		(1<<10)
#define ADC_DMA_CHANNEL_11		(1<<11)
#define ADC_DMA_CHANNEL_12		(1<<12)
#define ADC_DMA_CHANNEL_13		(1<<13)
#define ADC_DMA_CHANNEL_14		(1<<14)
#define ADC_DMA_CHANNEL_15		(1<<15)

//========================================================================
//                              变量声明
//========================================================================

typedef struct
{
	u8	DMA_Enable;				//DMA使能  	ENABLE,DISABLE
	u16	DMA_Channel;			//ADC通道使能寄存器, 1:使能, bit15~bit0 对应 ADC15~ADC0
	u16	DMA_Buffer_Address;		//ADC转换数据存储地址
	u8	DMA_Times;				//每个通道转换次数, ADC_1_Times,ADC_2_Times,ADC_4_Times,ADC_8_Times,ADC_16_Times,ADC_32_Times,ADC_64_Times,ADC_128_Times,ADC_256_Times
	u16 DMA_Scan_Times;			//设定DMA循环扫描次数(实际扫描次数为设定值n的n+1次，当DAM_ADC_AMT寄存器设置为0xFFFF时为无限循环)
} DMA_ADC_InitTypeDef;

typedef struct
{
	u8	DMA_Enable;				//DMA使能  	ENABLE,DISABLE
	u16	SPI_DMA_Rx_Buffer;			//接收数据存储地址
	u16	SPI_DMA_Tx_Buffer;			//发送数据存储地址
	u16	SPI_DMA_Length;				//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
	u8	DMA_SRC_Dir;			//数据源地址改变方向  	M2M_ADDR_INC,M2M_ADDR_DEC
	u8	DMA_DEST_Dir;			//数据目标地址改变方向 	M2M_ADDR_INC,M2M_ADDR_DEC
} DMA_M2M_InitTypeDef;

typedef struct
{
	u8	DMA_TX_Enable;			//DMA使能  	ENABLE,DISABLE
	u16	DMA_TX_Length;			//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
	u16	DMA_TX_Buffer;			//发送数据存储地址

	u8	DMA_RX_Enable;			//DMA使能  	ENABLE,DISABLE
	u16	DMA_RX_Length;			//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
	u16	DMA_RX_Buffer;			//接收数据存储地址
} DMA_UART_InitTypeDef;

typedef struct
{
//	u8  SPI_Mode;
	u8  SPI_DMA_Interrupt_En;	//SPI_DMA中断使能		ENABLE,DISABLE
	u8  SPI_DMA_TX_En;			//SPI_DMA发送数据控制位	ENABLE,DISABLE
	u8  SPI_DMA_RX_En;			//SPI_DMA接收数据控制位	ENABLE,DISABLE
	u8  SPI_DMA_Interrupt_Priority;	//SPI_DMA中断优先级[3:2](从低到高:00 01 10 11)
	u8  SPI_DMA_BusData_Priority;	//SPI_DMA总线数据优先级[1:0](从低到高:00 01 10 11)
	
	u8	SPI_DMA_Enable;				//DMA使能  			ENABLE,DISABLE
	u8  SPI_DMA_Master_Mode_Triggle;//主机模式触发控制位 ENABLE,DISABLE
	u8  SPI_DMA_Slave_Mode_Triggle;	//从机模式触发控制位 ENABLE,DISABLE
	u8  SPI_DMA_FIFO_Clear_En;		//SPI_DMA操作前清空FIFO  ENABLE,DISABLE
	
	u16	SPI_DMA_Rx_Buffer;		//接收数据存储地址
	u16	SPI_DMA_Tx_Buffer;		//发送数据存储地址
	
	u16	SPI_DMA_Length;				//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
	u8	SPI_DMA_AUTO_SS;			//自动控制SS脚使能  ENABLE,DISABLE
	u8	SPI_DMA_SS_Sel;				//自动控制SS脚选择 	00_P1.4    01_P2.4    10_P4.0    11_P3.5
	u16 SPI_DMA_Transport_Time_Gap;	//设置SPI_DMA传输的间隔时间(基于系统时钟 0最快)
	
} DMA_SPI_InitTypeDef;

/* 磁编码器读取数据结构体 */
typedef struct
{
	float Angle;			//转子角度
	float Angle_Speed;		//角速度
	u16 Revolutions;		//圈数
	u16 Safety_Word;		//安全字
} Magnetic_Encoder_Data;

typedef struct
{
	u8	DMA_Enable;				//DMA使能  	ENABLE,DISABLE
	u16	SPI_DMA_Rx_Buffer;			//接收数据存储地址
	u16	SPI_DMA_Tx_Buffer;			//发送数据存储地址
	u16	SPI_DMA_Length;				//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
} DMA_LCM_InitTypeDef;

typedef struct
{
	u8	DMA_TX_Enable;			//DMA使能  	ENABLE,DISABLE
	u16	DMA_TX_Length;			//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
	u16	DMA_TX_Buffer;			//发送数据存储地址

	u8	DMA_RX_Enable;			//DMA使能  	ENABLE,DISABLE
	u16	DMA_RX_Length;			//DMA传输总字节数  	(0~65535) + 1, 不要超过芯片 xdata 空间上限
	u16	DMA_RX_Buffer;			//接收数据存储地址
} DMA_I2C_InitTypeDef;

//========================================================================
//                              外部声明
//========================================================================
extern bit DmaADCFlag;
extern bit DmaM2MFlag;
extern bit DmaTx1Flag;
extern bit DmaRx1Flag;
extern bit DmaTx2Flag;
extern bit DmaRx2Flag;
extern bit DmaTx3Flag;
extern bit DmaRx3Flag;
extern bit DmaTx4Flag;
extern bit DmaRx4Flag;
extern bit DmaI2CTFlag;
extern bit DmaI2CRFlag;
extern bit SpiTxFlag;
extern bit SpiRxFlag;
extern bit SpiSendFlag;
extern bit UartSendFlag;
extern bit DmaLcmFlag;
extern bit LcmFlag;
extern u16 LCM_Cnt;

extern u8 xdata DmaTxBuffer[256];
extern u8 xdata DmaRxBuffer[256];

void DMA_ADC_Inilize(DMA_ADC_InitTypeDef xdata *DMA);
void DMA_M2M_Inilize(DMA_M2M_InitTypeDef *DMA);
void DMA_SPI_Inilize(DMA_SPI_InitTypeDef xdata *DMA);
void DMA_LCM_Inilize(DMA_LCM_InitTypeDef *DMA);
void DMA_I2C_Inilize(DMA_I2C_InitTypeDef *DMA);
void DMA_UART_Inilize(u8 UARTx, DMA_UART_InitTypeDef *DMA);

#endif
