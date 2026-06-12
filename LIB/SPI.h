/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef    __SPI_H
#define    __SPI_H

#include "config.h"

//========================================================================
//                               SPI设置
//========================================================================

#define SPI_SSIG_Set(n)         SSIG = n        /* SS引脚功能控制 */
#define SPI_Start(n)            SPEN = n        /* SPI使能控制位 */
#define SPI_FirstBit_Set(n)     DORD = n        /* 数据发送/接收顺序 MSB/LSB */
#define SPI_Mode_Set(n)         MSTR = n        /* SPI主从模式设置 */
#define SPI_CPOL_Set(n)         CPOL = n        /* SPI时钟极性控制 */
#define SPI_CPHA_Set(n)         CPHA = n        /* SPI时钟相位控制 */
#define SPI_Clock_Select(n)     SPCTL = (SPCTL & ~0x03) | (n)    /* SPI时钟频率选择 */

#define SPI_ClearFlag()         {SPIF = 1; WCOL = 1;}    /* 写 1 清除 SPIF和WCOL 标志 */
#define SPI_TOIFClear()         SPITOSR = 0x80           /* 设置 CTOCF 清除超时标志位 TOIF */

#define HSSPI_Enable(n)         HSSPI_CFG2 |= 0x20       //使能SPI高速模式
#define HSSPI_Disable(n)        HSSPI_CFG2 &= ~0x20      //关闭SPI高速模式

//========================================================================
//                              定义声明
//========================================================================

#define SPI_BUF_LENTH   128
#define SPI_BUF_type    edata



sbit  SPI_SS      = P1^4;
sbit  SPI_MOSI    = P1^5;
sbit  SPI_MISO    = P1^6;
sbit  SPI_SCLK    = P1^7;

sbit  SPI_SS_2    = P2^4;
sbit  SPI_MOSI_2  = P2^5;
sbit  SPI_MISO_2  = P2^6;
sbit  SPI_SCLK_2  = P2^7;

sbit  SPI_SS_3    = P4^0;
sbit  SPI_MOSI_3  = P4^1;
sbit  SPI_MISO_3  = P4^2;
sbit  SPI_SCLK_3  = P4^3;

sbit  SPI_SS_4    = P3^5;
sbit  SPI_MOSI_4  = P3^4;
sbit  SPI_MISO_4  = P3^3;
sbit  SPI_SCLK_4  = P3^2;

#define SPI_Mode_Slave      0
#define SPI_Mode_Master     1
#define SPI_CPOL_Low        0
#define SPI_CPOL_High       1
#define SPI_CPHA_1Edge      0
#define SPI_CPHA_2Edge      1
#define SPI_Speed_4         0
#define SPI_Speed_8         1
#define SPI_Speed_16        2
#define SPI_Speed_2         3
#define SPI_MSB             0
#define SPI_LSB             1

#ifndef TO_SCALE_1US
#define TO_SCALE_1US       0         //超时计数时钟源：1us时钟(1MHz时钟)
#endif
#ifndef TO_SCALE_SYSCLK
#define TO_SCALE_SYSCLK    1         //超时计数时钟源：系统时钟
#endif

typedef struct
{
	/* P_SW1 */
	u8    SPI_Pin_Select;	//SPI功能脚选择位
							//SPI_S[1:0]	SS		MOSI	MISO	SCLLK
							//	00			P1.4	P1.5	P1.6	P1.7	
							//	01			P2.4	P2.5	P2.6	P2.7
							//	10			P4.0	P4.1	P4.2	P4.3
							//	11			P3.5	P3.4	P3.3	P3.2
	/* IE2(bit 1) */
	u8    SPI_Interrupt_En;	//SPI中断使能 ENABLE,DISABLE
	
	/* HSSPI_CFG */
	u8    SPI_SS_Hold_Time;		//高速模式时SPI通信中SS控制信号的HOLD时间
	u8    SPI_SS_SetUp_Time;		//高速模式时SPI通信中SS控制信号的SETUP时间
	
	/* HSSPI_CFG2 */
	u8    SPI_MISO_MOSI_Exchange_En;	//MISO、MOSI功能脚位交换使能位
	u8    SPI_High_Speed_En;			//高速SPI使能位
	u8    SPI_FIFO_En;					//FIFO功能使能位
	u8    SPI_SS_Deactive_Time;			//高速模式时SS控制信号的DEACTIVE时间
	
	/* HSSPI_PSCR */
	u8    SPI_Clock_Div;	//高速SPI时钟分频器(工作频率 = 高速SPI输入时钟 / HSSPI_PSCR[7:0] + 1)
	
	/* SPCTL */
	u8    SPI_SSIG;         //片选位, ENABLE(忽略SS引脚功能), DISABLE(SS确定主机从机)
    u8    SPI_Enable;       //SPI功能启动使能, ENABLE,DISABLE
    u8    SPI_MSB_LSB;		//0 先发送/接收数据的高位(SPI_MSB), 1 先发送/接收数据的低位(SPI_LSB)
    u8    SPI_Master_Slave; //主机/从机选择位　SPI_Mode_Master, SPI_Mode_Slave
    u8    SPI_CPOL;         //SPI时钟极性选择　SPI_CPOL_High,   SPI_CPOL_Low
    u8    SPI_CPHA;         //SPI时钟相位选择　SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    u8    SPI_Speed;        //SPI时钟频率分频选择　SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2

    u8    TimeOutEnable;    //从机超时使能, ENABLE,DISABLE
    u8    TimeOutINTEnable; //超时中断使能, ENABLE,DISABLE
    u8    TimeOutScale;     //超时时钟源选择, TO_SCALE_1US,TO_SCALE_SYSCLK
    u32   TimeOutTimer;     //超时时间, 1 ~ 0xffffff
} SPI_InitTypeDef;


void SPI_Configuration_2(SPI_InitTypeDef *SPIx);
void SPI_Configuration(SPI_InitTypeDef *SPIx);
void SPI_SetMode(u8 mode);
void SPI_WriteByte(u8 dat);
u8 SPI_ReadByte(void);

#endif

