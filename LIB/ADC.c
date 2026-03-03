/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/
#include "AI8051U.h"
#include "ADC.h"
#include "PWM.h"
#include "Delay.h"
#include "def.h"
#include "string.h"

void ADC_Init()
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Speed 		= ADC_SPEED_2X1T;		//设置ADC时钟 SYSclk/2/1
	ADC_InitStructure.ADC_Align_Mode 	= ADC_RIGHT_JUSTIFIED;	//设置ADC采样数据对齐方式
	ADC_InitStructure.ADC_SMPduty 		= 31;	//模拟信号采样时间控制, 0~31（注意： SMPDUTY 一定不能设置小于 10）
	ADC_InitStructure.ADC_CsSetup 		= 1;	//通道选择时间控制(0,1)
	ADC_InitStructure.ADC_CsHold		= 3;	//ADC 通道选择保持时间控制 0,1(默认),2,3
	strcpy(ADC_InitStructure.ADC_Channel_Pin, "P0.0");	//ADC 通道引脚选择（相电流）
	ADC_InitStructure.ADC_PWMA_Triggle_EN = ENABLE;	    //A组PWM波触发ADC采样使能
	ADC_InitStructure.ADC_PWMB_Triggle_EN = DISABLE;    //B组PWM波触发ADC采样使能
	/*根据结构体设置的参数设置ADC*/
	ADC_Configuration(&ADC_InitStructure);
    
    strcpy(ADC_InitStructure.ADC_Channel_Pin, "P0.1");	//ADC 通道引脚选择（相电流）
    ADC_Configuration(&ADC_InitStructure);
    strcpy(ADC_InitStructure.ADC_Channel_Pin, "P0.2");	//ADC 通道引脚选择（相电流）
    ADC_Configuration(&ADC_InitStructure);
    
    strcpy(ADC_InitStructure.ADC_Channel_Pin, "P0.3");	//ADC 通道引脚选择（母线电压）
    ADC_Configuration(&ADC_InitStructure);
}


void ADC_Configuration(ADC_InitTypeDef* ADC_InitStructure)
{
	//1. 打开ADC电源
	ADC_PowerControl(ENABLE);
	delay_ms(2);	//延时等待ADC电源稳定
	
	//2. 选择ADC采集引脚
	ADC_CONTR = (ADC_CONTR & ~0x0F) | ADC_Pin_Channel_Select(ADC_InitStructure);
	
	//3. 设置触发ADC采样的信号源
	if(ADC_InitStructure->ADC_PWMA_Triggle_EN == ENABLE)
	{
		PWMA_CR2 = (PWMA_CR2 & ~0x70) | 0x20;	//PWMA更新周期事件为TRGO，用于周期触发ADC
	}
	else if(ADC_InitStructure->ADC_PWMB_Triggle_EN == ENABLE)
	{
		PWMB_CR2 = (PWMB_CR2 & 0x70) | 0x20;	//PWMB更新周期事件为TRGO，用于周期触发ADC
	}
	
	//4. 配置数据对齐方式、ADC时钟、ADC时序
	ADC_Inilize(ADC_InitStructure);
}

u8 ADC_Pin_Channel_Select(ADC_InitTypeDef* ADC_InitStructure)
{
	u8 Px_ = (u8)atoi(ADC_InitStructure->ADC_Channel_Pin + 1);	//取引脚端口号
	u8 P_x = (u8)atoi(ADC_InitStructure->ADC_Channel_Pin + 3);	//取引脚位号
	u8 pin_mask = 0;	//配置寄存器的掩码
	if(Px_ == 1)		//P1.x		//	ADC_CONTR[3:0]	ADC通道		ADC_CONTR[3:0]	ADC通道
	{								//		0000		P1.0			1000		P0.0
		pin_mask = P_x;				//		0001		P1.1			1001		P0.1
	}								//		0010		P1.2			1010		P0.2
	else if(Px_ == 0)	//P0.x		//		0011		P1.3			1011		P0.3
	{								//		0100		P1.4	 		1100		P0.4
		pin_mask = P_x;				//		0101		P1.5			1101		P0.5
		pin_mask |= 0x08;			//		0110		P1.6			1110		P0.6
	}								//		0111		P1.7			1111		P0.7
	return pin_mask;	//返回ADC选择的通道引脚所对应的寄存器掩码 ADC_CONTR[3:0]
}

//========================================================================
// 函数: u8	ADC_Inilize(ADC_InitTypeDef *ADCx)
// 描述: ADC初始化程序.
// 参数: ADCx: 结构参数,请参考adc.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	ADC_Inilize(ADC_InitTypeDef *ADCx)
{
	ADCCFG = (ADCCFG & ~ADC_SPEED_2X16T) | ADCx->ADC_Speed;	//设置ADC工作时钟频率
	ADC_ALIGN_MODE(ADCx->ADC_Align_Mode);					//AD转换结果对齐方式

	if(ADCx->ADC_SMPduty > 31)	return FAIL;	//错误
	if(ADCx->ADC_CsSetup > 1)	return FAIL;	//错误
	if(ADCx->ADC_CsHold > 3)	return FAIL;	//错误

	ADCTIM = (ADCx->ADC_CsSetup << 7) | (ADCx->ADC_CsHold << 5) | ADCx->ADC_SMPduty ;//设置 ADC 内部时序，ADC采样时间建议设最大值
	return SUCCESS;
}


//========================================================================
// 函数: void	ADC_PowerControl(u8 pwr)
// 描述: ADC电源控制程序.
// 参数: pwr: 电源控制,ENABLE或DISABLE.
// 返回: none.
// 版本: V1.0, 2012-10-22
//========================================================================
void	ADC_PowerControl(u8 pwr)
{
	if(pwr == ENABLE)	ADC_POWER = 1;
	else				ADC_POWER = 0;
}

//========================================================================
// 函数: u16	Get_ADCResult(u8 channel)
// 描述: 查询法读一次ADC转换结果.
// 参数: channel: 选择要转换的ADC通道.
// 返回: ADC转换结果.
// 版本: V1.0, 2012-10-22
//========================================================================
u16	Get_ADCResult(u8 channel)	//channel = 0~15
{
	u16	adc;
	u8	i;

	if(channel > ADC_CH15)	return	4096;	//错误,返回4096,调用的程序判断	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //设置ADC转换通道
	ADC_START = 1;//启动ADC转换
	NOP(10);			//对ADC_CONTR操作后要4T之后才能访问

	for(i=0; i<250; i++)		//超时返回，正常i等于10以内就可以转换完成
	{
		if(ADC_FLAG)
		{
			ADC_FLAG = 0;	//清除ADC转换结束标志
			if(RESFMT)		//转换结果右对齐。 
			{
				adc = ((u16)ADC_RES << 8) | ADC_RESL;
			}
			else		//转换结果左对齐。 
			{
				adc = (u16)ADC_RES;
				adc = (adc << 4) | ((ADC_RESL >> 4) & 0x0f);
			}
			return	adc;
		}
	}
	return	4096;	//错误,返回4096,调用的程序判断
}
