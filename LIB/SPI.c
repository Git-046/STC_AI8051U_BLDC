/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "SPI.h"

bit B_SPI_Busy; //发送忙标志
bit SPI_RxTimerOut;
bit SPI_TimerOutEn;
u8 	SPI_RxCnt;
u8  SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];



void SPI_Init()
{
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Pin_Select = 0x00;				//脚位选择
	
	SPI_InitStructure.SPI_Interrupt_En = DISABLE;			//禁用SPI中断
	
	SPI_InitStructure.SPI_SS_Hold_Time = 4;		//高速模式时SPI通信中SS控制信号的HOLD时间
	SPI_InitStructure.SPI_SS_SetUp_Time = 4;	//高速模式时SPI通信中SS控制信号的SETUP时间
	
	SPI_InitStructure.SPI_MISO_MOSI_Exchange_En = DISABLE;	//禁用引脚功能交换
	SPI_InitStructure.SPI_High_Speed_En = ENABLE;			//高速SPI使能
	SPI_InitStructure.SPI_FIFO_En = ENABLE;					//高速SPI的FIFO功能使能
	SPI_InitStructure.SPI_SS_Deactive_Time = 4;				//高速模式时SS控制信号的DEACTIVE时间
	
	SPI_InitStructure.SPI_Clock_Div = 0;					//高速SPI时钟分频器
	
	SPI_InitStructure.SPI_SSIG = DISABLE;					//忽略SS引脚功能
	SPI_InitStructure.SPI_Enable = ENABLE;					//使能SPI功能
	SPI_InitStructure.SPI_MSB_LSB = 0;						//0 先发送/接收数据的高位(SPI_MSB), 1 先发送/接收数据的低位(SPI_LSB)
	SPI_InitStructure.SPI_Master_Slave = SPI_Mode_Master;	//设定主机模式
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;				//设置时钟极性
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;			//设置时钟相位
	SPI_InitStructure.SPI_Speed = SPI_Speed_2;				//设置SPI时钟分频
	
	SPI_InitStructure.TimeOutEnable = DISABLE;		//SPI从机超时功能
	SPI_InitStructure.TimeOutINTEnable = DISABLE;	//SPI从机超时中断
	SPI_InitStructure.TimeOutScale = 1;				//SPI从机计数器时钟源选择	
	SPI_InitStructure.TimeOutTimer = 0x10;			//SPI超时长度控制寄存器
	
	SPI_Configuration(&SPI_InitStructure);
}

//========================================================================
// 函数: void SPI_Configuration(SPI_InitTypeDef *SPIx)
// 描述: SPI初始化程序.
// 参数: SPIx: 结构参数,请参考spi.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void SPI_Configuration(SPI_InitTypeDef *SPIx)
{
	P_SW1 = P_SW1 & ~0x0C | SPIx->SPI_Pin_Select;	//SPI功能脚选择位
	IE2  = IE2 & ~0x02 | SPIx->SPI_Interrupt_En;	//SPI中断使能
	
	HSSPI_CFG = HSSPI_CFG & ~0xF0 | SPIx->SPI_SS_Hold_Time;		//高速模式时SPI通信中SS控制信号的HOLD时间
	HSSPI_CFG = HSSPI_CFG & ~0x0F | SPIx->SPI_SS_SetUp_Time;	//高速模式时SPI通信中SS控制信号的SETUP时间
	
	HSSPI_CFG2 = HSSPI_CFG2 & ~0x40 | SPIx->SPI_MISO_MOSI_Exchange_En;	//MISO、MOSI功能脚位交换使能位
	HSSPI_CFG2 = HSSPI_CFG2 & ~0x20 | SPIx->SPI_High_Speed_En;			//高速SPI使能位
	HSSPI_CFG2 = HSSPI_CFG2 & ~0x10 | SPIx->SPI_FIFO_En;				//FIFO功能使能位
	HSSPI_CFG2 = HSSPI_CFG2 & ~0x0F | SPIx->SPI_SS_Deactive_Time;		//高速模式时SS控制信号的DEACTIVE时间
	
	HSSPI_PSCR = SPIx->SPI_Clock_Div;	//高速SPI时钟分频器(工作频率 = 高速SPI输入时钟 / HSSPI_PSCR[7:0] + 1)
	
	SPCTL = SPCTL & ~0x80 | SPIx->SPI_SSIG;		//片选位
	SPCTL = SPCTL & ~0x40 | SPIx->SPI_Enable;	//SPI功能启动使能
	SPCTL = SPCTL & ~0x20 | SPIx->SPI_MSB_LSB;	//0 先发送/接收数据的高位(SPI_MSB), 1 先发送/接收数据的低位(SPI_LSB)
	SPCTL = SPCTL & ~0x10 | SPIx->SPI_Master_Slave;	//主机/从机选择位
	SPCTL = SPCTL & ~0x08 | SPIx->SPI_CPOL;		//SPI时钟极性选择
	SPCTL = SPCTL & ~0x04 | SPIx->SPI_CPHA;		//SPI时钟相位选择
	SPCTL = SPCTL & ~0x03 | SPIx->SPI_Speed;	//SPI时钟频率分频选择
	
	SPITOCR = SPITOCR & ~0x80 | SPIx->TimeOutEnable;	//从机超时使能
	SPITOCR = SPITOCR & ~0x40 | SPIx->TimeOutINTEnable;	//超时中断使能
	SPITOCR = SPITOCR & ~0x20 | SPIx->TimeOutScale;	//超时时钟源选择
	
	SPITOTL = (u8)SPIx->TimeOutTimer;		//超时时间
	SPITOTH = (u8)(SPIx->TimeOutTimer >> 8);
	SPITOTE = (u8)(SPIx->TimeOutTimer >> 16);
	
	SPI_RxTimerOut = 0;
    B_SPI_Busy = 0;
    SPI_RxCnt = 0;
	
//    if(SPIx->SPI_SSIG == ENABLE) SSIG = 1;     //conform Master or Slave by SPI_Mode(ignore SS)
//    else                         SSIG = 0;     //conform Master or Slave by SS pin.
//    SPI_Start(SPIx->SPI_Enable);
//    SPI_FirstBit_Set(SPIx->SPI_MSB_LSB);
//    SPI_Mode_Set(SPIx->SPI_Master_Slave);
//    SPI_CPOL_Set(SPIx->SPI_CPOL);
//    SPI_CPHA_Set(SPIx->SPI_CPHA);
//    SPI_Clock_Select(SPIx->SPI_Speed);

//    if(SPIx->TimeOutScale == TO_SCALE_SYSCLK)   SPITOCR |= 0x20;  //超时计数时钟源：系统时钟
//    else
//    {
//        SET_TPS();          //设置系统等待时间单元，用于控制EEPROM操作、SPI/I2C超时时间以及休眠唤醒等待时间
//        SPITOCR &= ~0x20;   //超时计数时钟源：1us时钟(1MHz时钟)
//    }

//    if((SPIx->TimeOutTimer > 0) && (SPIx->TimeOutTimer <= 0xffffff))
//    {
//        SPITOTL = (u8)SPIx->TimeOutTimer;
//        SPITOTH = (u8)(SPIx->TimeOutTimer>>8);
//        SPITOTE = (u8)(SPIx->TimeOutTimer>>16); //写 SPITOTE 后，新的TM值才会生效
//    }
//    
//    if((SPIx->SPI_Master_Slave == SPI_Mode_Slave) && (SPIx->TimeOutEnable == ENABLE))
//    {
//        SPI_TimerOutEn = 1;
//        SPITOCR |= 0x80;    //从机接收超时功能使能
//    }
//    else
//    {
//        SPI_TimerOutEn = 0;
//        SPITOCR &= ~0x80;   //从机接收超时功能禁止
//    }

//    if(SPIx->TimeOutINTEnable == ENABLE) SPITOCR |= 0x40;   //从机接收超时中断使能
//    else                                 SPITOCR &= ~0x40;  //从机接收超时中断禁止
}

//========================================================================
// 函数: void SPI_SetMode(u8 mode)
// 描述: SPI设置主从模式函数.
// 参数: mode: 指定模式, 取值 SPI_Mode_Master 或 SPI_Mode_Slave.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void SPI_SetMode(u8 mode)
{
    if(mode == SPI_Mode_Slave)
    {
        MSTR = 0;     //重新设置为从机待机
        SSIG = 0;     //SS引脚确定主从
        if(SPI_TimerOutEn)
        {
            SPI_TOIFClear();
            SPITOCR |= 0x80;    //从机接收超时功能使能
        }
    }
    else
    {
        SPITOCR &= ~0x80;   //从机接收超时功能禁止
        MSTR = 1;     //使能 SPI 主机模式
        SSIG = 1;     //忽略SS引脚功能
    }
}

//========================================================================
// 函数: void SPI_WriteByte(u8 dat)
// 描述: SPI发送一个字节数据.
// 参数: dat: 要发送的数据.
// 返回: none.
// 版本: V1.0, 2020-09-14
//========================================================================
void SPI_WriteByte(u8 dat)  //SPI发送一个字节数据
{
    if(ESPI)
    {
        B_SPI_Busy = 1;
        SPDAT = dat;
        while(B_SPI_Busy);  //中断模式
    }
    else
    {
        SPDAT = dat;
        while(SPIF == 0);   //查询模式
        SPI_ClearFlag();    //清除SPIF和WCOL标志
    }
}

//========================================================================
// 函数: void SPI_ReadByte(u8 dat)
// 描述: SPI查询模式读取一个字节数据.
// 参数: none.
// 返回: 读取的数据.
// 版本: V1.0, 2020-09-14
//========================================================================
u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    if(ESPI)
    {
        B_SPI_Busy = 1;
        while(B_SPI_Busy);  //中断模式
    }
    else
    {
        while(SPIF == 0);   //查询模式
        SPI_ClearFlag();    //清除SPIF和WCOL标志
    }
    return (SPDAT);
}

//SPI中断处理函数
void SPI_Interrupt(void) interrupt SPI_VECTOR
{
	if(ESPI)
	{
		B_SPI_Busy = 0;
		SPIF = 0;
	}
}
