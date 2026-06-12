#ifndef _INIT_H_
#define _INIT_H_
#include "AI8051U.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"
#include "stdarg.h"		//NULL
#include "math.h"
#include "Clock.h"
#include "GPIO.h"
#include "PWM.h"
#include "ADC.h"
#include "DMA.h"
#include "SPI.h"
#include "Timer.h"
#include "UART.h"
#include "Delay.h"
#include "config.h"
#include "Motor.h"
#include "Foc.h"
#include "State_Machine.h"
#include "Protect.h"

u8 ADC_Pin_Channel_Select(ADC_InitTypeDef* ADC_InitStructure);
void ADC_Configuration(ADC_InitTypeDef* ADC_InitStructure);
void ADC_Init(void);


void PWM_Init(void);
void PWM_Frequency_ARR_PSCR_Calculator(u32 set_freq, PWMx_InitDefine* PWMx_InitStructure);
u8 PWM_Pin_Configuration(u8 PWMx, PWMx_InitDefine *PWMx_InitStructure);
u8 PWM_Configuration_2(u8 PWMx, PWMx_InitDefine* PWM_Construction);
char* strstr(const char* str1, const char* str2);	//C89标准下没有strstr函数

extern bit B_SPI_Busy;  //发送忙标志
extern bit SPI_RxTimerOut;
extern u8  SPI_RxCnt;
extern u8  SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];
void SPI_Init(void);

extern u16 xdata ADC_Result_Buffer[4];
void DMA_ADC_Init(void);
void DMA_SPI_Init(void);

void Timer_Init(void);

void UART_Init(u8 UARTx);

void System_Init(void);
void Parameter_Init(void);
#endif
