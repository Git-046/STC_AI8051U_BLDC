/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "PWM.h"
#include "string.h"
#include "stdlib.h"
#include "Clock.h"

/*---------------------------------------
@ function  :   void PWM_Capture(void)
@ describe  :   PWM调速信号捕获函数
@ parameter :   None
@ date      :   2026-2-28
-----------------------------------------*/
void PWM_Capture(void)
{
    
}

void PWM_Init(void)
{
	PWMx_InitDefine PWMx_InitStructure;
	/* 输出比较模式*/
	//1.1 设置PWMn成员属性
	PWMx_InitStructure.PWM_In_Out_Mode = CCAS_OUTPUT;	//设置PWM信号方向(输出/输入方向)(PWM_CCMR1[1:0])
	PWMx_InitStructure.PWM_Output_Channel_Select = ENO1P;	//PWM1P输出使能(PWMx_ENO[7:0])
	strcpy(PWMx_InitStructure.PWM_IO_Pin_Select, "P2.4");	//选择引脚(PWMx_PS[7:0])
	PWMx_InitStructure.PWM_OC_Mode = CCMRn_PWM_MODE2;		//计数器大于比较值，输出高电平(PWMx_CCMR1[6:4])
	PWMx_InitStructure.PWM_Duty = PWMx_InitStructure.PWM_Pscr / 1000 * set_pwm_duty;	//占空比(PWMx_PSCRH、PWMx_PSCRL) 0~Period 对应 0~100%
	PWMx_InitStructure.PWM_Capture_Edge_or_Valid_Level = 1;	//高电平有效(PWMx_CCER1)
	PWM_Configuration_2(PWM1, &PWMx_InitStructure);//根据结构体成员要求，配置对应的寄存器

	PWMx_InitStructure.PWM_Output_Channel_Select = ENO2P;	//PWM2P输出使能
	strcpy(PWMx_InitStructure.PWM_IO_Pin_Select, "P2.2");	//选择引脚
	PWM_Configuration_2(PWM2, &PWMx_InitStructure);
	
	PWMx_InitStructure.PWM_Output_Channel_Select = ENO3P;	//PWM3P输出使能
	strcpy(PWMx_InitStructure.PWM_IO_Pin_Select, "P2.0");	//选择引脚
	PWM_Configuration_2(PWM3, &PWMx_InitStructure);
	
	//1.2 设置PWMA组属性
	PWMx_InitStructure.PWM_Fast_EN = DISABLE;			//快速输出比较(PWMx_CCMRx.2) 不使能
	PWMx_InitStructure.PWM_OC_PreLoad_EN = ENABLE;		//输出比较预装载(PWMx_CCMRx.3) 使能
	PWMx_InitStructure.PWM_ARR_PreLoad_EN = ENABLE;		//计数器预装载(PWMx_CR1[7]) 使能
	PWMx_InitStructure.PWM_Align = PWM_ALIGN_CENTER_3;	//中央对齐模式3(PWMx_CR1[6:5])，计数器向上、向下比较中断标志位均置1
	PWMx_InitStructure.PWM_Couter_Dir = 0;				//计数方向(中央对齐模式不起作用)(PWMx_CR1[4])
	
	PWMx_InitStructure.PWM_Single_Pulse_Mode = DISABLE;	//单脉冲模式(PWMx_CR1[3]) 不使能
	PWMx_InitStructure.PWM_Update_Source = 0;			//更新请求源(PWMx_CR1[2])
	PWMx_InitStructure.PWM_Update_Dis = DISABLE;		//更新事件禁止使能位(PWMx_CR1[1])
	PWMx_InitStructure.PWM_DeadTime = 1;				//设定死区时间 0~255 (PWMx_DTR)
	PWM_Frequency_ARR_PSCR_Calculator(PWMA_CARRIER_FREQ, &PWMx_InitStructure);//自动计算 psc arr，配置PWM波频率
	PWMx_InitStructure.PWM_BrakeEnable		= DISABLE;	//PWM刹车功能(PWMx_BKR[4]) 不使能
	PWMx_InitStructure.PWM_Couter_EN = ENABLE;			//计数器(PWMx_CR1[0]) 使能
	PWMx_InitStructure.PWM_MainOutEnable = ENABLE;		//主输出(PWMx_BKR[7]) 使能
    
    PWMx_InitStructure.PWM_Update_Interrupt_EN = ENABLE;//更新中断使能
    PWMx_InitStructure.PWM_CC_Interrupt_EN = ENABLE;    //捕获/比较中断使能
    
	PWM_Configuration_2(PWMA, &PWMx_InitStructure);
	
	/* 输入捕获模式 */
	//2.1 设置PWMn成员属性
	PWMx_InitStructure.PWM_In_Out_Mode = CCBS_IUTPUT_TI6FP6;	    //设置PWM信号的选择和方向(输入捕获方向PWMx_CCMRn[1:0])
	PWMx_InitStructure.PWM_Output_Channel_Select = PWM_OUT_DISABLE;	//不选择PWM输出通道(PWMx_ENO[7:0])
	memset(PWMx_InitStructure.PWM_IO_Pin_Select, 0, sizeof(PWMx_InitStructure.PWM_IO_Pin_Select));//清空选择的输出引脚(PWMx_PS[7:0])
	PWMx_InitStructure.PWM_Capture_Edge_or_Valid_Level = 0;			//捕获上升沿(PWMx_CCER1)
	PWM_Configuration_2(PWM6, &PWMx_InitStructure);
    
    PWMx_InitStructure.PWM_In_Out_Mode = CCBS_IUTPUT_TI7FP7;	    //设置PWM信号的选择和方向(输入捕获方向PWMx_CCMRn[1:0])
    PWMx_InitStructure.PWM_Capture_Edge_or_Valid_Level = 1;			//捕获下降沿(PWMx_CCER1)
    PWM_Configuration_2(PWM7, &PWMx_InitStructure);
    
    //2.2 设置PWMB组属性
    PWMx_InitStructure.PWM_Fast_EN = DISABLE;			//快速输出比较(PWMx_CCMRx.2) 不使能
	PWMx_InitStructure.PWM_OC_PreLoad_EN = ENABLE;		//输出比较预装载(PWMx_CCMRx.3) 使能
	PWMx_InitStructure.PWM_ARR_PreLoad_EN = ENABLE;		//计数器预装载(PWMx_CR1[7]) 使能
	PWMx_InitStructure.PWM_Align = PWM_ALIGN_EDGE;	    //边沿对齐模式(PWMx_CR1[6:5])，计数器向上、向下比较中断标志位均置1
	PWMx_InitStructure.PWM_Couter_Dir = 0;				//计数方向(0 向上计数    1 向下计数模式)(PWMx_CR1[4])
	
	PWMx_InitStructure.PWM_Single_Pulse_Mode = DISABLE;	//单脉冲模式(PWMx_CR1[3]) 不使能
	PWMx_InitStructure.PWM_Update_Source = 0;			//更新请求源(PWMx_CR1[2])
	PWMx_InitStructure.PWM_Update_Dis = DISABLE;		//更新事件禁止使能位(PWMx_CR1[1])
	PWMx_InitStructure.PWM_DeadTime = 1;				//设定死区时间 0~255 (PWMx_DTR)
	PWM_Frequency_ARR_PSCR_Calculator(PWMA_CARRIER_FREQ, &PWMx_InitStructure);//自动计算 psc arr，配置PWM波频率
	PWMx_InitStructure.PWM_BrakeEnable		= DISABLE;	//PWM刹车功能(PWMx_BKR[4]) 不使能
	PWMx_InitStructure.PWM_Couter_EN = ENABLE;			//计数器(PWMx_CR1[0]) 使能
	PWMx_InitStructure.PWM_MainOutEnable = ENABLE;		//主输出(PWMx_BKR[7]) 使能
    
    PWMx_InitStructure.PWM_Update_Interrupt_EN = ENABLE;//更新中断使能
    PWMx_InitStructure.PWM_CC_Interrupt_EN = ENABLE;    //捕获/比较中断使能
    
	PWM_Configuration_2(PWMB, &PWMx_InitStructure);
}

void PWM_Frequency_ARR_PSCR_Calculator(u32 set_freq, PWMx_InitDefine* PWMx_InitStructure)
{
	/* 中央对齐模式的PWM频率：system_clock/((PSCR+1)*(ARR*2)) */
	/* 边缘对齐模式的PWM频率：system_clock/((psc+1)*(arr+1)) */
	u32 period_cycles, frequency, pwm_clock_source;
	u16 arr, psc;
    
	pwm_clock_source = PWMA_SOURCE_FREQ;
	frequency = set_freq;
	if(frequency > pwm_clock_source || frequency <= 0)
	{
		return;
	}
	period_cycles = pwm_clock_source / frequency;	//period_cycles = (psc+1)*(arr*2)
												//period_cycles = (psc+1)*(arr+1)
	if(PWMx_InitStructure->PWM_Align == PWM_ALIGN_EDGE)	//边缘对齐模式
	{
		if(period_cycles <= 65536ul)
		{
			psc = 0;
			arr = period_cycles - 1;
		}
		else
		{
			psc = (period_cycles - 1) / 65536u;
			arr = (period_cycles + psc) / (psc + 1) - 1;
		}
	}
	else	//中央对齐模式
	{
		if(period_cycles <= 65536ul * 2)
        {
            psc = 0;
            arr = period_cycles / 2 - 1;
        }
        else
        {
            psc = period_cycles / (65536ul * 2);
            arr = period_cycles / ((psc + 1) * 2) - 1;
        }
	}
	
	PWMx_InitStructure->PWM_Pscr = psc;
	PWMx_InitStructure->PWM_Arr = arr;
}

u8 PWM_Pin_Configuration(u8 PWMx, PWMx_InitDefine *PWMx_InitStructure)
{
	u8 Px_ = (u8)atoi(PWMx_InitStructure->PWM_IO_Pin_Select + 1);	//取引脚端口号
	u8 P_x = (u8)atoi(PWMx_InitStructure->PWM_IO_Pin_Select + 3);	//取引脚位号
	
	if(PWMx_InitStructure->PWM_IO_Pin_Select == NULL)
		return 0;
	
	/*根据引脚处理对应的寄存器*/
	if(PWMx <= 4 && Px_ <= 2)
	{
		if(P_x == 0 || P_x == 1)
		{
			if(Px_ == 1)
			{
				PWMA_PS &= ~0x03;	//00
			}
			else if(Px_ == 0)
			{
				PWMA_PS &= ~0x02;	//01
				PWMA_PS |= 0x01;
			}
			else if(Px_ == 2)
			{
				PWMA_PS |= 0x02;	//10
				PWMA_PS &= ~0x01;
			}
		}
		else if(P_x == 2 || P_x == 3)
		{
			if(Px_ == 1)
			{
				PWMA_PS &= ~0x0C;	//00
			}
			else if(Px_ == 0)
			{
				PWMA_PS &= ~0x08;	//01
				PWMA_PS |= 0x04;
			}
			else if(Px_ == 2)
			{
				PWMA_PS |= 0x08;	//10
				PWMA_PS &= ~0x04;
			}
		}
		else if(P_x == 4 || P_x == 5)
		{
			if(Px_ == 1)
			{
				PWMA_PS &= ~0x30;	//00
			}
			else if(Px_ == 0)
			{
				PWMA_PS &= ~0x20;	//01
				PWMA_PS |= 0x10;
			}
			else if(Px_ == 2)
			{
				PWMA_PS |= 0x20;	//10
				PWMA_PS &= ~0x10;
			}
			
		}
		else if(P_x == 6 || P_x == 7)
		{
			if(Px_ == 1)
			{
				PWMA_PS &= ~0xC0;	//00
			}
			else if(Px_ == 0)
			{
				PWMA_PS &= ~0x80;	//01
				PWMA_PS |= 0x40;
			}
			else if(Px_ == 2)
			{
				PWMA_PS |= 0x80;	//10
				PWMA_PS &= ~0x40;
			}	
		}
	}
	else if(PWMx >= 5 && PWMx <= 8)
	{
		/* PWMA_5、 PWMA_6 */
		if(Px_ == 3 && P_x == 3)	//P3.3
		{
			PWMA_PS2 &= ~0x03;	//00
		}
		else if(Px_ == 4 && P_x == 4)	//P4.4
		{
			PWMA_PS2 &= ~0x01;
			PWMA_PS2 |= 0x02;	//01
		}
		else if(Px_ == 5 && P_x == 6)	//P5.6
		{
			PWMA_PS2 |= 0x02;
			PWMA_PS2 &= ~0x01;	//10
		}
		else if(Px_ == 3 && P_x == 4)	//P3.4
		{
			PWMA_PS2 &= ~0x0C;	//00
		}
		else if(Px_ == 4 && P_x == 7)	//P4.7
		{
			PWMA_PS2 &= ~0x08;
			PWMA_PS2 |= 0x04;	//01
		}
		else if(Px_ == 5 && P_x == 7)	//P5.7
		{
			PWMA_PS2 |= 0x08;
			PWMA_PS2 &= ~0x04;	//10
		}
		
		/* PWMB */
		//PWM5
		else if(Px_ == 0 && P_x == 1)	//P0.1
		{
			PWMB_PS &= ~0x03;	//00
		}
		else if(Px_ == 1 && P_x == 1)	//P1.1
		{
			PWMB_PS &= ~0x01;
			PWMB_PS |= 0x02;	//01
		}
		else if(Px_ == 2 && P_x == 1)	//P2.1
		{
			PWMB_PS &= ~0x01;
			PWMB_PS |= 0x02;	//10
		}
		else if(Px_ == 5 && P_x == 0)	//P5.0
		{
			PWMB_PS |= 0x03;	//11
		}
		//PWM6
		else if(Px_ == 0 && P_x == 5)	//P0.5
		{
			PWMB_PS &= ~0x0C;	//00
		}
		else if(Px_ == 1 && P_x == 5)	//P1.5
		{
			PWMB_PS &= ~0x08;
			PWMB_PS |= 0x04;	//01
		}
		else if(Px_ == 2 && P_x == 5)	//P2.5
		{
			PWMB_PS &= ~0x04;
			PWMB_PS |= 0x08;	//10
		}
		else if(Px_ == 5 && P_x == 2)	//P5.2
		{
			PWMB_PS |= 0x0C;	//11
		}
		
		//PWM7
		else if(Px_ == 0 && P_x == 1)	//P0.1
		{
			PWMB_PS &= ~0x30;	//00
		}
		else if(Px_ == 1 && P_x == 1)	//P1.1
		{
			PWMB_PS &= ~0x20;
			PWMB_PS |= 0x10;	//01
		}
		else if(Px_ == 2 && P_x == 1)	//P2.1
		{
			PWMB_PS &= ~0x10;
			PWMB_PS |= 0x20;	//10
		}
		else if(Px_ == 5 && P_x == 0)	//P5.0
		{
			PWMB_PS |= 0x30;	//11
		}
		//PWM8
		else if(Px_ == 0 && P_x == 7)	//P0.7
		{
			PWMB_PS &= ~0xC0;	//00
		}
		else if(Px_ == 1 && P_x == 7)	//P1.7
		{
			PWMB_PS &= ~0x80;
			PWMB_PS |= 0x40;	//01
		}
		else if(Px_ == 2 && P_x == 7)	//P2.7
		{
			PWMB_PS &= ~0x40;
			PWMB_PS |= 0x80;	//10
		}
		else if(Px_ == 5 && P_x == 3)	//P5.3
		{
			PWMB_PS |= 0xC0;	//11
		}
	}
	else
		return FAIL;
	return SUCCESS;
}

u8 PWM_Configuration_2(u8 PWMx, PWMx_InitDefine *PWMx_InitStructure)
{
    /* PWM通道设置 */
	if(PWMx == PWM1)
	{
		PWMA_CC1E_Disable();		//先关闭主通道的 输入捕获/比较输出
		PWMA_CC1NE_Disable();		//先关闭互补通道的 比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMA_CC1S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMA_OC1ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO1P)
			{
				PWMA_CC1E_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO1P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMA_CC1E_Disable();	//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO1P;		//失能对应的PWM输出通道
			}
			/*互补通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO1N)
			{
				PWMA_CC1NE_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO1N;
			}
			else
			{
				PWMA_CC1NE_Disable();		//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO1N;
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMA_OC1ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMA_Duty1(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMA_CC1S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMA_CC1E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMA_CC1P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMA_CC1P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
        //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMA_CC1IE_Enable();
        else
            PWMA_CC1IE_Disable();
	}
    
    else if(PWMx == PWM2)
	{
		PWMA_CC2E_Disable();		//先关闭主通道的 输入捕获/比较输出
		PWMA_CC2NE_Disable();		//先关闭互补通道的 比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMA_CC2S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMA_OC1ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO2P)
			{
				PWMA_CC2E_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO2P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMA_CC2E_Disable();	//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO2P;		//失能对应的PWM输出通道
			}
			/*互补通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO2N)
			{
				PWMA_CC2NE_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO2N;
			}
			else
			{
				PWMA_CC2NE_Disable();		//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO2N;
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMA_OC2ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
//			PWMA_OC1_ReloadEnable(PWMx_InitStructure->PWM_OC_PreLoad_EN);	//输出比较预装载使能位
			
			/*1.4 设置占空比初始值*/
			PWMA_Duty2(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMA_CC2S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMA_CC2E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMA_CC2P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMA_CC2P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
        //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMA_CC2IE_Enable();
        else
            PWMA_CC2IE_Disable();
	}
    
    else if(PWMx == PWM3)
	{
		PWMA_CC3E_Disable();		//先关闭主通道的 输入捕获/比较输出
		PWMA_CC3NE_Disable();		//先关闭互补通道的 比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMA_CC3S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMA_OC3ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO3P)
			{
				PWMA_CC3E_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO3P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMA_CC3E_Disable();	//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO3P;		//失能对应的PWM输出通道
			}
			/*互补通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO3N)
			{
				PWMA_CC3NE_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO3N;
			}
			else
			{
				PWMA_CC3NE_Disable();		//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO3N;
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMA_OC3ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMA_Duty3(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMA_CC3S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMA_CC3E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMA_CC3P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMA_CC3P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
        //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMA_CC3IE_Enable();
        else
            PWMA_CC3IE_Disable();
	}
    else if(PWMx == PWM4)
	{
		PWMA_CC4E_Disable();		//先关闭主通道的 输入捕获/比较输出
		PWMA_CC4NE_Disable();		//先关闭互补通道的 比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMA_CC4S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMA_OC4ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO4P)
			{
				PWMA_CC4E_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO4P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMA_CC4E_Disable();	//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO4P;		//失能对应的PWM输出通道
			}
			/*互补通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO4N)
			{
				PWMA_CC4NE_Enable();		//开启输入捕获/比较输出
				PWMA_ENO |= ENO4N;
			}
			else
			{
				PWMA_CC4NE_Disable();		//关闭输入捕获/比较输出
				PWMA_ENO &= ~ENO4N;
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMA_OC1ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMA_Duty4(PWMx_InitStructure->PWM_Duty);

			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMA_CC4S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMA_CC4E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMA_CC4P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMA_CC4P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
        //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMA_CC4IE_Enable();
        else
            PWMA_CC4IE_Disable();
	}
    
    else if(PWMx == PWM5)
	{
		PWMB_CC5E_Disable();		//先关闭主通道的 输入捕获/比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMB_CC5S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMB_OC5ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO5P)
			{
				PWMB_CC5E_Enable();		//开启输入捕获/比较输出
				PWMB_ENO |= ENO5P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMB_CC5E_Disable();	//关闭输入捕获/比较输出
				PWMB_ENO &= ~ENO5P;		//失能对应的PWM输出通道
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMB_OC5ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMB_Duty5(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMB_CC5S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMB_CC5E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMB_CC5P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMB_CC5P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
         //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMB_CC5IE_Enable();
        else
            PWMB_CC5IE_Disable();
	}
    
    else if(PWMx == PWM6)
	{
		PWMB_CC6E_Disable();		//先关闭主通道的 输入捕获/比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMB_CC6S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMB_OC6ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO6P)
			{
				PWMB_CC6E_Enable();		//开启输入捕获/比较输出
				PWMB_ENO |= ENO6P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMB_CC6E_Disable();	//关闭输入捕获/比较输出
				PWMB_ENO &= ~ENO6P;		//失能对应的PWM输出通道
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMB_OC6ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMB_Duty6(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMB_CC5S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMB_CC5E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMB_CC5P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMB_CC5P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
         //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMB_CC6IE_Enable();
        else
            PWMB_CC6IE_Disable();
	}
	
    else if(PWMx == PWM7)
	{
		PWMB_CC7E_Disable();		//先关闭主通道的 输入捕获/比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMB_CC7S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMB_OC7ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO7P)
			{
				PWMB_CC7E_Enable();		//开启输入捕获/比较输出
				PWMB_ENO |= ENO7P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMB_CC7E_Disable();	//关闭输入捕获/比较输出
				PWMB_ENO &= ~ENO7P;		//失能对应的PWM输出通道
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMB_OC7ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMB_Duty7(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMB_CC7S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMB_CC7E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMB_CC7P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMB_CC7P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}
		else
			return FAIL;
        
        //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMB_CC7IE_Enable();
        else
            PWMB_CC7IE_Disable();
	}
    
    else if(PWMx == PWM8)
	{
		PWMB_CC8E_Disable();		//先关闭主通道的 输入捕获/比较输出
		/* CCnS仅在通道关闭时才是可写的 */
		
		//1.如果PWM通道选择为输出通道模式
		if(PWMx_InitStructure->PWM_In_Out_Mode == 0x00)   //判断是输出还是输入模式
		{
			PWMB_CC8S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);	    //设置 输出/ (输入)捕获方向
			PWMB_OC8ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//设置比较/捕获模式
			
			/*1.1 主通道*/
			if(PWMx_InitStructure->PWM_Output_Channel_Select | ENO8P)
			{
				PWMB_CC8E_Enable();		//开启输入捕获/比较输出
				PWMB_ENO |= ENO8P;		//使能对应的PWM输出通道
			}
			else
			{
				PWMB_CC8E_Disable();	//关闭输入捕获/比较输出
				PWMB_ENO &= ~ENO8P;		//失能对应的PWM输出通道
			}
			
			/*1.2 根据结构体中PWM_Pin_Select选择的脚位自动配置寄存器 */
			if(PWM_Pin_Configuration(PWMx, PWMx_InitStructure) == FALSE)
			{
				printf("PWM_Pin_Configuration failed!\n");
				return FAIL;
			}
			/*1.3 设置输出比较*/
			PWMB_OC8ModeSet(PWMx_InitStructure->PWM_OC_Mode);				//输出比较模式
			
			/*1.4 设置占空比初始值*/
			PWMB_Duty8(PWMx_InitStructure->PWM_Duty);
			return SUCCESS;
		}
        
		//2.如果PWM通道选择为输入捕获模式
		else if(PWMx_InitStructure->PWM_In_Out_Mode != 0x00)
		{
			PWMB_CC8S_Direction(PWMx_InitStructure->PWM_In_Out_Mode);
			PWMB_CC8E_Enable();				//使能输入捕获
			if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 0)
            {
				PWMB_CC8P_CaptureRise();	//捕获上升沿
            }
			else if(PWMx_InitStructure->PWM_Capture_Edge_or_Valid_Level == 1)
            {
				PWMB_CC8P_CaptureFall();	//捕获下降沿
            }
			return SUCCESS;
		}

		else
			return FAIL;
        
         //3.中断相关
        if(PWMx_InitStructure->PWM_CC_Interrupt_EN)
            PWMB_CC8IE_Enable();
        else
            PWMB_CC8IE_Disable();
	}
    
    /* PWM组设置 */
	else if(PWMx == PWMA)
	{
		//1.控制寄存器 PMWx_CR1
		PWMA_CR1 = (PWMA_CR1 & ~0x80) | PWMx_InitStructure->PWM_ARR_PreLoad_EN;	//设置ARR预装载使能位
		PWMA_CR1 = (PWMA_CR1 & ~0x60) | PWMx_InitStructure->PWM_Align;			//设置PWM对齐模式
		PWMA_CR1 = (PWMA_CR1 & ~0x10) | PWMx_InitStructure->PWM_Couter_Dir;		//设置PWM计数器计数方向
		PWMA_CR1 = (PWMA_CR1 & ~0x08) | PWMx_InitStructure->PWM_Single_Pulse_Mode;//设置单脉冲使能位
		PWMA_CR1 = (PWMA_CR1 & ~0x04) | PWMx_InitStructure->PWM_Update_Source;	//设置更新请求源
		PWMA_CR1 = (PWMA_CR1 & ~0x02) | PWMx_InitStructure->PWM_Update_Dis;		//设置更新禁止使能位
		PWMA_CR1 = (PWMA_CR1 & ~0x01) | PWMx_InitStructure->PWM_Couter_EN;		//设置计数器使能位
		
		PWMA_CCPCAPreloaded(PWMx_InitStructure->PWM_CCP_PreLoad_EN);	//捕获/比较预装载控制位(该位只对具有互补输出的通道起作用)
		PWMA_BrakeEnable(PWMx_InitStructure->PWM_BrakeEnable);			//开启/禁止刹车输入
		PWMA_DeadTime(PWMx_InitStructure->PWM_DeadTime);				//死区发生器设置
		PWMA_AutoReload(PWMx_InitStructure->PWM_Arr);					//周期设置
        
        if(PWMx_InitStructure->PWM_Update_Interrupt_EN)
            PWMA_UIEA_Enable();
        else
            PWMA_UIEA_Disable();

		PWMA_BrakeOutputEnable(PWMx_InitStructure->PWM_MainOutEnable);	//主输出使能
		return	SUCCESS;
	}
    else if(PWMx == PWMB)
	{
		//1.控制寄存器 PMWx_CR1
		PWMB_CR1 = (PWMA_CR1 & ~0x80) | PWMx_InitStructure->PWM_ARR_PreLoad_EN;	//设置ARR预装载使能位
		PWMB_CR1 = (PWMA_CR1 & ~0x60) | PWMx_InitStructure->PWM_Align;			//设置PWM对齐模式
		PWMB_CR1 = (PWMA_CR1 & ~0x10) | PWMx_InitStructure->PWM_Couter_Dir;		//设置PWM计数器计数方向
		PWMB_CR1 = (PWMA_CR1 & ~0x08) | PWMx_InitStructure->PWM_Single_Pulse_Mode;//设置单脉冲使能位
		PWMB_CR1 = (PWMA_CR1 & ~0x04) | PWMx_InitStructure->PWM_Update_Source;	//设置更新请求源
		PWMB_CR1 = (PWMA_CR1 & ~0x02) | PWMx_InitStructure->PWM_Update_Dis;		//设置更新禁止使能位
		PWMB_CR1 = (PWMA_CR1 & ~0x01) | PWMx_InitStructure->PWM_Couter_EN;		//设置计数器使能位
		
		PWMB_CCPCBPreloaded(PWMx_InitStructure->PWM_CCP_PreLoad_EN);	//捕获/比较预装载控制位(该位只对具有互补输出的通道起作用)
		PWMB_BrakeEnable(PWMx_InitStructure->PWM_BrakeEnable);			//开启/禁止刹车输入
		PWMB_DeadTime(PWMx_InitStructure->PWM_DeadTime);				//死区发生器设置
		PWMB_AutoReload(PWMx_InitStructure->PWM_Arr);					//周期设置
        if(PWMx_InitStructure->PWM_Update_Interrupt_EN)
            PWMB_UIEA_Enable();
        else
            PWMB_UIEA_Disable();
        
		PWMB_BrakeOutputEnable(PWMx_InitStructure->PWM_MainOutEnable);	//主输出使能
		return	SUCCESS;
	}
	else
		return FAIL;
}

//C89标准下没有strstr函数
char* strstr(const char* str1, const char* str2) 
{
	const char* p1;
	const char* p1_adv;
	const char* p2;
	
    if (*str2 == '\0') 
		return (char*)str1;
  
    p1 = str1;
	
    while (*p1) 
	{
        p1_adv = p1;
        p2 = str2;
        while (*p1_adv && *p2 && *p1_adv == *p2) 
		{
            p1_adv++;
            p2++;
        }
        
        if (*p2 == '\0')
			return (char*)p1;
        p1++;
    } 
    return NULL;
}
/*=================================================================================================*/





//========================================================================
// 函数: PWM_Configuration
// 描述: PWM初始化程序.
// 参数: PWMx: 结构参数,请参考PWM.h里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 PWM_Configuration(u8 PWM, PWMx_InitDefine *PWMx)
{
	if(PWM == PWM1)
	{
		PWMA_CC1E_Disable();		//先关闭主通道的 输入捕获/比较输出
		PWMA_CC1NE_Disable();		//先关闭互补通道的 比较输出
		PWMA_CC1S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC1ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO1P)
		{
			PWMA_CC1E_Enable();			//开启主通道 输入捕获/比较输出
			PWMA_ENO |= ENO1P;
		}
		else
		{
			PWMA_CC1E_Disable();		//关闭主通道 输入捕获/比较输出
			PWMA_ENO &= ~ENO1P;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO1N)
		{
			PWMA_CC1NE_Enable();		//开启互补通道的 比较输出
			PWMA_ENO |= ENO1N;
		}
		else
		{
			PWMA_CC1NE_Disable();		//关闭互补通道的 比较输出
			PWMA_ENO &= ~ENO1N;
		}
		
		PWMA_Duty1(PWMx->PWM_Duty);		//设定通道占空比
		return	SUCCESS;
	}
	
	if(PWM == PWM2)
	{
		PWMA_CC2E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC2NE_Disable();		//关闭比较输出
		PWMA_CC2S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC2ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO2P)
		{
			PWMA_CC2E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO2P;
		}
		else
		{
			PWMA_CC2E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO2P;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO2N)
		{
			PWMA_CC2NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO2N;
		}
		else
		{
			PWMA_CC2NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO2N;
		}
		
		PWMA_Duty2(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM3)
	{
		PWMA_CC3E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC3NE_Disable();		//关闭比较输出
		PWMA_CC3S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC3ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO3P)
		{
			PWMA_CC3E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO3P;
		}
		else
		{
			PWMA_CC3E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO3P;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO3N)
		{
			PWMA_CC3NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO3N;
		}
		else
		{
			PWMA_CC3NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO3N;
		}
		
		PWMA_Duty3(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM4)
	{
		PWMA_CC4E_Disable();		//关闭输入捕获/比较输出
		PWMA_CC4NE_Disable();		//关闭比较输出
		PWMA_CC4S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMA_OC4ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO4P)
		{
			PWMA_CC4E_Enable();			//开启输入捕获/比较输出
			PWMA_ENO |= ENO4P;
		}
		else
		{
			PWMA_CC4E_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO4P;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO4N)
		{
			PWMA_CC4NE_Enable();		//开启输入捕获/比较输出
			PWMA_ENO |= ENO4N;
		}
		else
		{
			PWMA_CC4NE_Disable();		//关闭输入捕获/比较输出
			PWMA_ENO &= ~ENO4N;
		}
		
		PWMA_Duty4(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWMA)
	{
//		PWMA_OC1_ReloadEnable(PWMx->PWM_Reload);	//输出比较的预装载使能
//		PWMA_OC1_FastEnable(PWMx->PWM_Fast);		//输出比较快速功能使能
//		PWMA_CCPCAPreloaded(PWMx->PWM_PreLoad);	    //捕获/比较预装载控制位(该位只对具有互补输出的通道起作用)
//		PWMA_BrakeEnable(PWMx->PWM_BrakeEnable);	//开启/禁止刹车输入

		PWMA_DeadTime(PWMx->PWM_DeadTime);	                //死区发生器设置
		PWMA_AutoReload(PWMx->PWM_Arr);	                    //周期设置
		PWMA_BrakeOutputEnable(PWMx->PWM_MainOutEnable);	//主输出使能设置
        
        if(PWMx->PWM_Update_Interrupt_EN)
            PWMA_UIEA_Enable();
        else
            PWMA_UIEA_Disable();
        
		PWMA_CEN_Enable(PWMx->PWM_CEN_Enable);	//使能计数器
		return	SUCCESS;
	}

	if(PWM == PWM5)
	{
		PWMB_CC5E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC5S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC5ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO5P)
		{
			PWMB_CC5E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO5P;
		}
		else
		{
			PWMB_CC5E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO5P;
		}
		
		PWMB_Duty5(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM6)
	{
		PWMB_CC6E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC6S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC6ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO6P)
		{
			PWMB_CC6E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO6P;
		}
		else
		{
			PWMB_CC6E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO6P;
		}
		
		PWMB_Duty6(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM7)
	{
		PWMB_CC7E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC7S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC7ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO7P)
		{
			PWMB_CC7E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO7P;
		}
		else
		{
			PWMB_CC7E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO7P;
		}
		
		PWMB_Duty7(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWM8)
	{
		PWMB_CC8E_Disable();		//关闭输入捕获/比较输出
		PWMB_CC8S_Direction(CCAS_OUTPUT);		//CCnS仅在通道关闭时才是可写的
		PWMB_OC8ModeSet(PWMx->PWM_OC_Mode);		//设置输出比较模式

		if(PWMx->PWM_Output_Channel_Select & ENO8P)
		{
			PWMB_CC8E_Enable();			//开启输入捕获/比较输出
			PWMB_ENO |= ENO8P;
		}
		else
		{
			PWMB_CC8E_Disable();		//关闭输入捕获/比较输出
			PWMB_ENO &= ~ENO8P;
		}
		
		PWMB_Duty8(PWMx->PWM_Duty);
		return	SUCCESS;
	}
	
	if(PWM == PWMB)
	{
//		PWMB_OC5_ReloadEnable(PWMx->PWM_Reload);	//输出比较的预装载使能
//		PWMB_OC5_FastEnable(PWMx->PWM_Fast);		//输出比较快速功能使能
//		PWMB_CCPCBPreloaded(PWMx->PWM_PreLoad);	//捕获/比较预装载控制位(该位只对具有互补输出的通道起作用)
//		PWMB_BrakeEnable(PWMx->PWM_BrakeEnable);	//开启/禁止刹车输入

		PWMB_DeadTime(PWMx->PWM_DeadTime);	//死区发生器设置
		PWMB_AutoReload(PWMx->PWM_Arr);	//周期设置
		PWMB_BrakeOutputEnable(PWMx->PWM_MainOutEnable);	//主输出使能
        PWMB_BrakeOutputEnable(PWMx->PWM_MainOutEnable);	//主输出使能设置
        
        if(PWMx->PWM_Update_Interrupt_EN)
            PWMB_UIEA_Enable();
        else
            PWMB_UIEA_Disable();
        
		PWMB_CEN_Enable(PWMx->PWM_CEN_Enable);	//使能计数器
		return	SUCCESS;
	}

	return	FAIL;	//错误
}

//========================================================================
// 函数: UpdatePwm
// 描述: PWM占空比更新程序.
// 参数: PWM: PWM通道/组号, PWMx结构参数,请参考PWM.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-10-22
//========================================================================
void UpdatePwm(u8 PWM, PWMx_Duty *PWMx)
{
	switch(PWM)
	{
		case PWM1:
			PWMA_Duty1(PWMx->PWM1_Duty);
		break;

		case PWM2:
			PWMA_Duty2(PWMx->PWM2_Duty);
		break;

		case PWM3:
			PWMA_Duty3(PWMx->PWM3_Duty);
		break;

		case PWM4:
			PWMA_Duty4(PWMx->PWM4_Duty);
		break;

		case PWM5:
			PWMB_Duty5(PWMx->PWM5_Duty);
		break;

		case PWM6:
			PWMB_Duty6(PWMx->PWM6_Duty);
		break;

		case PWM7:
			PWMB_Duty7(PWMx->PWM7_Duty);
		break;

		case PWM8:
			PWMB_Duty8(PWMx->PWM8_Duty);
		break;

		case PWMA:
			PWMA_Duty1(PWMx->PWM1_Duty);
			PWMA_Duty2(PWMx->PWM2_Duty);
			PWMA_Duty3(PWMx->PWM3_Duty);
			PWMA_Duty4(PWMx->PWM4_Duty);
		break;

		case PWMB:
			PWMB_Duty5(PWMx->PWM5_Duty);
			PWMB_Duty6(PWMx->PWM6_Duty);
			PWMB_Duty7(PWMx->PWM7_Duty);
			PWMB_Duty8(PWMx->PWM8_Duty);
		break;
	}
}

//========================================================================
// 函数: ReadPWMA(void)
// 描述: 异步读取PWMA特殊功能寄存器函数. 
// 参数: addr: 读取特殊功能寄存器地址.
// 返回: dat:  读取特殊功能寄存器内容.
// 版本: V1.0, 2022-03-16
//========================================================================
u8 ReadPWMA(u8 addr)
{
	u8 dat;
	
	while (HSPWMA_ADR &	0x80); //等待前一个异步读写完成
	HSPWMA_ADR = addr |	0x80;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMA_ADDR寄存器的最高位写1,表示读数据
	while (HSPWMA_ADR &	0x80); //等待当前异步读取完成
	dat	= HSPWMA_DAT;		   //读取异步数据
	
	return dat;
}

//========================================================================
// 函数: ReadPWMB(void)
// 描述: 异步读取PWMB特殊功能寄存器函数. 
// 参数: addr: 读取特殊功能寄存器地址.
// 返回: dat:  读取特殊功能寄存器内容.
// 版本: V1.0, 2022-03-16
//========================================================================
u8 ReadPWMB(u8 addr)
{
	u8 dat;
	
	while (HSPWMB_ADR &	0x80); //等待前一个异步读写完成
	HSPWMB_ADR = addr |	0x80;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMB_ADDR寄存器的最高位写1,表示读数据
	while (HSPWMB_ADR &	0x80); //等待当前异步读取完成
	dat	= HSPWMB_DAT;		   //读取异步数据
	
	return dat;
}

//========================================================================
// 函数: WritePWMA(void)
// 描述: 异步读取PWMA特殊功能寄存器函数. 
// 参数: addr: 写入特殊功能寄存器地址.
// 参数: dat:  写入特殊功能寄存器内容.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void WritePWMA(u8	addr, u8 dat)
{
	while (HSPWMA_ADR &	0x80); //等待前一个异步读写完成
	HSPWMA_DAT = dat;		   //准备需要写入的数据
	HSPWMA_ADR = addr &	0x7f;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMA_ADDR寄存器的最高位写0,表示写数据
}

//========================================================================
// 函数: WritePWMB(void)
// 描述: 异步读取PWMB特殊功能寄存器函数. 
// 参数: addr: 写入特殊功能寄存器地址.
// 参数: dat:  写入特殊功能寄存器内容.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void WritePWMB(u8	addr, u8 dat)
{
	while (HSPWMB_ADR &	0x80); //等待前一个异步读写完成
	HSPWMB_DAT = dat;		   //准备需要写入的数据
	HSPWMB_ADR = addr &	0x7f;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMB_ADDR寄存器的最高位写0,表示写数据
}

//========================================================================
// 函数: HSPWM_Configuration
// 描述: HSPWM初始化程序.
// 参数: PWMx: 结构参数,请参考PWM.h里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2022-04-05
//========================================================================
void HSPWM_Configuration(u8 PWM, HSPWMx_InitDefine *PWMx, PWMx_Duty *DUTYx)
{
	u8 tmpENO,tmpCCER1,tmpCCER2;
	
	if(PWM == PWMA)
	{
		HSPWMA_CFG = 0x03;			//使能PWMA相关寄存器异步访问功能

		if(PWMx->PWM_Output_Channel_Select & ENO1P)
		{
			tmpENO |= ENO1P;
			tmpCCER1 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO1P;
			tmpCCER1 &= ~0x01;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO1N)
		{
			tmpENO |= ENO1N;
			tmpCCER1 |= 0x04;
		}
		else
		{
			tmpENO &= ~ENO1N;
			tmpCCER1 &= ~0x04;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO2P)
		{
			tmpENO |= ENO2P;
			tmpCCER1 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO2P;
			tmpCCER1 &= ~0x10;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO2N)
		{
			tmpENO |= ENO2N;
			tmpCCER1 |= 0x40;
		}
		else
		{
			tmpENO &= ~ENO2N;
			tmpCCER1 &= ~0x40;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO3P)
		{
			tmpENO |= ENO3P;
			tmpCCER2 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO3P;
			tmpCCER2 &= ~0x01;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO3N)
		{
			tmpENO |= ENO3N;
			tmpCCER2 |= 0x04;
		}
		else
		{
			tmpENO &= ~ENO3N;
			tmpCCER2 &= ~0x04;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO4P)
		{
			tmpENO |= ENO4P;
			tmpCCER2 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO4P;
			tmpCCER2 &= ~0x10;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO4N)
		{
			tmpENO |= ENO4N;
			tmpCCER2 |= 0x40;
		}
		else
		{
			tmpENO &= ~ENO4N;
			tmpCCER2 &= ~0x40;
		}
		
		//通过异步方式设置PWMA的相关寄存器
		WritePWMA((u8)&PWMA_CCER1, 0x00);
		WritePWMA((u8)&PWMA_CCER2, 0x00);
		WritePWMA((u8)&PWMA_CCMR1, CCMRn_PWM_MODE1);			//通道模式配置
		WritePWMA((u8)&PWMA_CCMR2, CCMRn_PWM_MODE1);
		WritePWMA((u8)&PWMA_CCMR3, CCMRn_PWM_MODE1);
		WritePWMA((u8)&PWMA_CCMR4, CCMRn_PWM_MODE1);
		WritePWMA((u8)&PWMA_CCER1, tmpCCER1);			//配置通道输出使能和极性
		WritePWMA((u8)&PWMA_CCER2, tmpCCER2);
		WritePWMA((u8)&PWMA_ENO, tmpENO);	//使能PWM信号输出端口
		WritePWMA((u8)&PWMA_CCR1H, (u8)(DUTYx->PWM1_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((u8)&PWMA_CCR1L, (u8)DUTYx->PWM1_Duty);
		WritePWMA((u8)&PWMA_CCR2H, (u8)(DUTYx->PWM2_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((u8)&PWMA_CCR2L, (u8)DUTYx->PWM2_Duty);
		WritePWMA((u8)&PWMA_CCR3H, (u8)(DUTYx->PWM3_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((u8)&PWMA_CCR3L, (u8)DUTYx->PWM3_Duty);
		WritePWMA((u8)&PWMA_CCR4H, (u8)(DUTYx->PWM4_Duty >> 8));	//设置输出PWM的占空比
		WritePWMA((u8)&PWMA_CCR4L, (u8)DUTYx->PWM4_Duty);
		WritePWMA((u8)&PWMA_ARRH,	(u8)(PWMx->PWM_Arr >> 8));	//设置输出PWM的周期
		WritePWMA((u8)&PWMA_ARRL,	(u8)(PWMx->PWM_Arr));
		WritePWMA((u8)&PWMA_DTR, PWMx->PWM_DeadTime);					//设置互补对称输出PWM的死区
		WritePWMA((u8)&PWMA_BKR, PWMx->PWM_MainOutEnable<<7);	//使能主输出
		WritePWMA((u8)&PWMA_CR1, PWMx->PWM_CEN_Enable);				//开始PWM计数
	}
	else if(PWM == PWMB)
	{
		HSPWMB_CFG = 0x03;			//使能PWMB相关寄存器异步访问功能

		if(PWMx->PWM_Output_Channel_Select & ENO5P)
		{
			tmpENO |= ENO5P;
			tmpCCER1 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO5P;
			tmpCCER1 &= ~0x01;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO6P)
		{
			tmpENO |= ENO6P;
			tmpCCER1 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO6P;
			tmpCCER1 &= ~0x10;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO7P)
		{
			tmpENO |= ENO7P;
			tmpCCER2 |= 0x01;
		}
		else
		{
			tmpENO &= ~ENO7P;
			tmpCCER2 &= ~0x01;
		}
		if(PWMx->PWM_Output_Channel_Select & ENO8P)
		{
			tmpENO |= ENO8P;
			tmpCCER2 |= 0x10;
		}
		else
		{
			tmpENO &= ~ENO8P;
			tmpCCER2 &= ~0x10;
		}
		
		//通过异步方式设置PWMB的相关寄存器
		WritePWMB((u8)&PWMB_CCER1, 0x00);
		WritePWMB((u8)&PWMB_CCER2, 0x00);
		WritePWMB((u8)&PWMB_CCMR1, CCMRn_PWM_MODE1);			//通道模式配置
		WritePWMB((u8)&PWMB_CCMR2, CCMRn_PWM_MODE1);
		WritePWMB((u8)&PWMB_CCMR3, CCMRn_PWM_MODE1);
		WritePWMB((u8)&PWMB_CCMR4, CCMRn_PWM_MODE1);
		WritePWMB((u8)&PWMB_CCER1, tmpCCER1);			//配置通道输出使能和极性
		WritePWMB((u8)&PWMB_CCER2, tmpCCER2);
		WritePWMB((u8)&PWMB_ENO, tmpENO);	//使能PWM信号输出端口
		WritePWMB((u8)&PWMB_CCR5H, (u8)(DUTYx->PWM5_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((u8)&PWMB_CCR5L, (u8)DUTYx->PWM5_Duty);
		WritePWMB((u8)&PWMB_CCR6H, (u8)(DUTYx->PWM6_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((u8)&PWMB_CCR6L, (u8)DUTYx->PWM6_Duty);
		WritePWMB((u8)&PWMB_CCR7H, (u8)(DUTYx->PWM7_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((u8)&PWMB_CCR7L, (u8)DUTYx->PWM7_Duty);
		WritePWMB((u8)&PWMB_CCR8H, (u8)(DUTYx->PWM8_Duty >> 8));	//设置输出PWM的占空比
		WritePWMB((u8)&PWMB_CCR8L, (u8)DUTYx->PWM8_Duty);
		WritePWMB((u8)&PWMB_ARRH,	(u8)(PWMx->PWM_Arr >> 8));	//设置输出PWM的周期
		WritePWMB((u8)&PWMB_ARRL,	(u8)(PWMx->PWM_Arr));
		WritePWMB((u8)&PWMB_DTR, PWMx->PWM_DeadTime);					//设置互补对称输出PWM的死区
		WritePWMB((u8)&PWMB_BKR, PWMx->PWM_MainOutEnable<<7);	//使能主输出
		WritePWMB((u8)&PWMB_CR1, PWMx->PWM_CEN_Enable);				//开始PWM计数
	}
}

//========================================================================
// 函数: UpdateHSPwm
// 描述: HSPWM占空比更新程序.
// 参数: PWM: PWM通道/组号, PWMx结构参数,请参考PWM.h里的定义.
// 返回: none.
// 版本: V1.0, 2022-04-05
//========================================================================
void UpdateHSPwm(u8 PWM, PWMx_Duty *PWMx)
{
	switch(PWM)
	{
		case PWM1:
			WritePWMA((u8)&PWMA_CCR1H, (u8)(PWMx->PWM1_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR1L, (u8)PWMx->PWM1_Duty);
		break;

		case PWM2:
			WritePWMA((u8)&PWMA_CCR2H, (u8)(PWMx->PWM2_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR2L, (u8)PWMx->PWM2_Duty);
		break;

		case PWM3:
			WritePWMA((u8)&PWMA_CCR3H, (u8)(PWMx->PWM3_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR3L, (u8)PWMx->PWM3_Duty);
		break;

		case PWM4:
			WritePWMA((u8)&PWMA_CCR4H, (u8)(PWMx->PWM4_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR4L, (u8)PWMx->PWM4_Duty);
		break;

		case PWM5:
			WritePWMB((u8)&PWMB_CCR5H, (u8)(PWMx->PWM5_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR5L, (u8)PWMx->PWM5_Duty);
		break;

		case PWM6:
			WritePWMB((u8)&PWMB_CCR6H, (u8)(PWMx->PWM6_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR6L, (u8)PWMx->PWM6_Duty);
		break;

		case PWM7:
			WritePWMB((u8)&PWMB_CCR7H, (u8)(PWMx->PWM7_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR7L, (u8)PWMx->PWM7_Duty);
		break;

		case PWM8:
			WritePWMB((u8)&PWMB_CCR8H, (u8)(PWMx->PWM8_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR8L, (u8)PWMx->PWM8_Duty);
		break;

		case PWMA:
			WritePWMA((u8)&PWMA_CCR1H, (u8)(PWMx->PWM1_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR1L, (u8)PWMx->PWM1_Duty);
			WritePWMA((u8)&PWMA_CCR2H, (u8)(PWMx->PWM2_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR2L, (u8)PWMx->PWM2_Duty);
			WritePWMA((u8)&PWMA_CCR3H, (u8)(PWMx->PWM3_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR3L, (u8)PWMx->PWM3_Duty);
			WritePWMA((u8)&PWMA_CCR4H, (u8)(PWMx->PWM4_Duty >> 8));	//设置输出PWM的占空比
			WritePWMA((u8)&PWMA_CCR4L, (u8)PWMx->PWM4_Duty);
		break;

		case PWMB:
			WritePWMB((u8)&PWMB_CCR5H, (u8)(PWMx->PWM5_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR5L, (u8)PWMx->PWM5_Duty);
			WritePWMB((u8)&PWMB_CCR6H, (u8)(PWMx->PWM6_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR6L, (u8)PWMx->PWM6_Duty);
			WritePWMB((u8)&PWMB_CCR7H, (u8)(PWMx->PWM7_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR7L, (u8)PWMx->PWM7_Duty);
			WritePWMB((u8)&PWMB_CCR8H, (u8)(PWMx->PWM8_Duty >> 8));	//设置输出PWM的占空比
			WritePWMB((u8)&PWMB_CCR8L, (u8)PWMx->PWM8_Duty);
		break;
	}
}

/*********************************************************/
