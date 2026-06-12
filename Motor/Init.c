#include "Context.h"
#include "Init.h"


void System_Init(void)
{
	//1. 初始化系统时钟
	HSPllClkConfig(MCKSEL_HIRC, PLL_SEL, 1);
	
	//2. 初始化外设
    GPIO_Init();
    ADC_Init();
	PWM_Init();
    SPI_Init();
	DMA_ADC_Init();
	DMA_SPI_Init();
	Timer_Init();
    UART_Init(UART2);
    
    //3. 初始化参数
    Parameter_Init();
}

/*------------------------------------------------------------
@ function  :   void Parameter_Init(void)
@ describe  :   参数初始化
@ parameter :   None
@ date      :   2026-3-1
-------------------------------------------------------------*/
void Parameter_Init(void)
{
    g_ctx->restart_times = MOTOR_RESTART_TIMES;

    /*Motor Parameter*/
    memset((void*)&g_ctx->motor_data, 0, sizeof(Motor_Data));
    memset((void*)&g_ctx->motor_speed, 0, sizeof(Motor_Speed));

    memset((void*)&g_ctx->phase_current, 0, sizeof(Phase_Current));
    memset((void*)&g_ctx->alpha_beta_current, 0, sizeof(Alpha_Beta_Axis_Current));
    memset((void*)&g_ctx->dq_current, 0, sizeof(Direct_Quadrature_Axis_Current));

    memset((void*)&g_ctx->dq_current_ref, 0, sizeof(Direct_Quadrature_Axis_Current));
    memset((void*)&g_ctx->dq_voltage_ctl, 0, sizeof(Direct_Quadrature_Axis_Voltage));
    memset((void*)&g_ctx->dq_voltage_ctl_limit, 0, sizeof(Direct_Quadrature_Axis_Voltage));
    memset((void*)&g_ctx->alpha_beta_voltage_ctl, 0, sizeof(Alpha_Beta_Axis_Voltage));
    memset((void*)&g_ctx->phase_voltage_ctl, 0, sizeof(Phase_Voltage));

    memset((void*)ADC_Result_Buffer, 0, sizeof(u16)*4);

    memset((void*)SPI_TX_Buffer, 0, sizeof(u8)*8);
    memset((void*)SPI_RX_Buffer, 0, sizeof(u8)*4);
    memset((void*)g_ctx->tx_cmd_arr, 0, sizeof(u8)*TLE5012B_TX_BUFFER_SIZE);

    g_ctx->motor_speed.max_speed = SPEED_MAX;
    g_ctx->motor_speed.min_speed = SPEED_MIN;
    g_ctx->motor_speed.acceleration_speed = SPEED_ACCERELATION;
    g_ctx->motor_speed.deceleration_speed = SPEED_DECERELATION;

    /*PID Parameter*/
    memset((void*)&g_ctx->pid_d, 0, sizeof(PID_Structure));
    memset((void*)&g_ctx->pid_q, 0, sizeof(PID_Structure));
    memset((void*)&g_ctx->pid_speed, 0, sizeof(PID_Structure));

    g_ctx->pid_d.P_gain = PID_ID_KP;
    g_ctx->pid_d.I_gain = PID_ID_KI;
    g_ctx->pid_d.D_gain = PID_ID_KD;

    g_ctx->pid_q.P_gain = PID_IQ_KP;
    g_ctx->pid_q.I_gain = PID_IQ_KI;
    g_ctx->pid_q.D_gain = PID_IQ_KD;

    g_ctx->pid_speed.P_gain = PID_S_KP;
    g_ctx->pid_speed.I_gain = PID_S_KI;
    g_ctx->pid_speed.D_gain = PID_S_KD;

    g_ctx->pid_d.output_limit = PID_I_Limit;
    g_ctx->pid_d.I_limit = PID_I_Limit;
    g_ctx->pid_d.I_period = 1.0 / CURRENT_LOOP_FREQUENT;
    g_ctx->pid_d.Kc_gain = 1.0;

    g_ctx->pid_q.output_limit = PID_I_Limit;
    g_ctx->pid_q.I_limit = PID_I_Limit;
    g_ctx->pid_q.I_period = 1.0 / CURRENT_LOOP_FREQUENT;
    g_ctx->pid_q.Kc_gain = 1.0;

    g_ctx->pid_speed.output_limit = PID_I_Limit;
    g_ctx->pid_speed.I_limit = PID_I_Limit;
    g_ctx->pid_speed.I_period = 1.0 / SPEED_LOOP_FREQUENT;
    g_ctx->pid_speed.Kc_gain = 1.0;

    memset((void*)&g_ctx->speed_ramp, 0, sizeof(Speed_Ramp));
    g_ctx->speed_ramp.accelerate_amount = SPEED_ACCERELATION;
    g_ctx->speed_ramp.decelerate_amount = SPEED_DECERELATION;
}

/*---------------------------------------------------------
@ function  :   void GPIO_Init()
@ describe  :   GPIO Initialization
@ parameter :   None
@ date      :   2026-6-11
----------------------------------------------------------*/
void GPIO_Init()
{
	//1.设置PWM波输出对应的IO口
	GPIO_InitTypeDef GPIOx_InitStructure = {0};
	
	GPIOx_InitStructure.Mode = GPIO_OUT_PP;			//推挽输出
	GPIOx_InitStructure.Port = GPIO_P2;				//P2.x
	GPIOx_InitStructure.Pin  = GPIO_Pin_4;			//P2.4
	GPIOx_InitStructure.Pull_Up_En = DISABLE;
	GPIOx_InitStructure.Pull_Down_En = DISABLE;
	GPIOx_InitStructure.Smit_Triggle_En = DISABLE;
	GPIOx_InitStructure.Fast_Switch_Speed_En = ENABLE;
	GPIOx_InitStructure.Strengthen_Current_En = ENABLE;
	GPIOx_InitStructure.Digital_Input_En = DISABLE;
	GPIOx_InitStructure.User_Control_En = DISABLE;
	GPIO_Inilize_2(&GPIOx_InitStructure);
	
	GPIOx_InitStructure.Port = GPIO_P2;				//P2.x
	GPIOx_InitStructure.Pin  = GPIO_Pin_2;			//P2.2
	GPIO_Inilize_2(&GPIOx_InitStructure);
	
	GPIOx_InitStructure.Port = GPIO_P2;				//P2.x
	GPIOx_InitStructure.Pin  = GPIO_Pin_0;			//P2.0
	GPIO_Inilize_2(&GPIOx_InitStructure);
	
	//2.设置ADC采样(电流采样)的IO口
	GPIOx_InitStructure.Mode = GPIO_HighZ;			//浮空输入
//	GPIOx_InitStructure.Port = GPIO_P2;				//P2.x
//	GPIOx_InitStructure.Pin  = GPIO_Pin_0;			//P2.0
	GPIO_Inilize_2(&GPIOx_InitStructure);
	
	/* ... */
	
	//3.设置磁编码器通信接口
	GPIOx_InitStructure.Mode = GPIO_OUT_PP;			//推挽输出
	GPIOx_InitStructure.Port = GPIO_P1;				//P1.x
	GPIOx_InitStructure.Pin  = GPIO_Pin_5;			//P1.5
	GPIO_Inilize_2(&GPIOx_InitStructure);			//MOSI
	
	GPIOx_InitStructure.Mode = GPIO_HighZ;			//浮空输入
	GPIOx_InitStructure.Port = GPIO_P1;				//P1.x
	GPIOx_InitStructure.Pin  = GPIO_Pin_6;			//P1.6
	GPIO_Inilize_2(&GPIOx_InitStructure);			//MISO
	
	GPIOx_InitStructure.Mode = GPIO_OUT_PP;			//推挽输出
	GPIOx_InitStructure.Port = GPIO_P1;				//P1.x
	GPIOx_InitStructure.Pin  = GPIO_Pin_7;			//P1.7
	GPIO_Inilize_2(&GPIOx_InitStructure);			//CLK
	
}

/*---------------------------------------------------------
@ function  :   u8 GPIO_Inilize_2(GPIO_InitTypeDef *GPIOx)
@ describe  :   GPIO Initialization with more parameters
@ parameter :   GPIOx: 结构参数,请参考头文件里的定义.
@ date      :   2026-6-11
----------------------------------------------------------*/
u8 GPIO_Inilize_2(GPIO_InitTypeDef *GPIOx)
{
    if(GPIOx->Port > GPIO_P7)        return FAIL;    //错误
    if(GPIOx->Mode > GPIO_OUT_PP)    return FAIL;    //错误
	
    if(GPIOx->Port == GPIO_P0)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P0M1 &= ~GPIOx->Pin,    P0M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P0M1 |=  GPIOx->Pin,    P0M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P0M1 |=  GPIOx->Pin,    P0M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P0M1 &= ~GPIOx->Pin,    P0M0 |=  GPIOx->Pin;     //推挽输出
		
		P0PU = GPIOx->Pull_Up_En == ENABLE ? P0PU | GPIOx->Pin : P0PU & ~GPIOx->Pin;			//上拉电阻
		P0PD = GPIOx->Pull_Down_En == ENABLE ? P0PD | GPIOx->Pin : P0PD & ~GPIOx->Pin;			//下拉电阻
		P0NCS = GPIOx->Smit_Triggle_En == ENABLE ? P0NCS |GPIOx->Pin : P0NCS & ~GPIOx->Pin;		//端口施密特触发
		P0SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P0SR | GPIOx->Pin : P0SR & ~GPIOx->Pin;	//端口电电平切换速度
		P0DR = GPIOx->Strengthen_Current_En == ENABLE ? P0DR | GPIOx->Pin : P0DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P0IE = GPIOx->Digital_Input_En == ENABLE ? P0IE | GPIOx->Pin : P0IE & ~GPIOx->Pin;		//端口电平输入使能
		P0BP = GPIOx->User_Control_En == ENABLE ? P0BP | GPIOx->Pin : P0BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P1)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P1M1 &= ~GPIOx->Pin,    P1M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P1M1 |=  GPIOx->Pin,    P1M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P1M1 |=  GPIOx->Pin,    P1M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P1M1 &= ~GPIOx->Pin,    P1M0 |=  GPIOx->Pin;     //推挽输出
		
		P1PU = GPIOx->Pull_Up_En == ENABLE ? P1PU | GPIOx->Pin : P1PU & ~GPIOx->Pin;			//上拉电阻
		P1PD = GPIOx->Pull_Down_En == ENABLE ? P1PD | GPIOx->Pin : P1PD & ~GPIOx->Pin;			//下拉电阻
		P1NCS = GPIOx->Smit_Triggle_En == ENABLE ? P1NCS |GPIOx->Pin : P1NCS & ~GPIOx->Pin;		//端口施密特触发
		P1SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P1SR | GPIOx->Pin : P1SR & ~GPIOx->Pin;	//端口电电平切换速度
		P1DR = GPIOx->Strengthen_Current_En == ENABLE ? P1DR | GPIOx->Pin : P1DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P1IE = GPIOx->Digital_Input_En == ENABLE ? P1IE | GPIOx->Pin : P1IE & ~GPIOx->Pin;		//端口电平输入使能
		P1BP = GPIOx->User_Control_En == ENABLE ? P1BP | GPIOx->Pin : P1BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P2)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P2M1 &= ~GPIOx->Pin,    P2M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P2M1 |=  GPIOx->Pin,    P2M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P2M1 |=  GPIOx->Pin,    P2M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P2M1 &= ~GPIOx->Pin,    P2M0 |=  GPIOx->Pin;     //推挽输出
		
		P2PU = GPIOx->Pull_Up_En == ENABLE ? P2PU | GPIOx->Pin : P2PU & ~GPIOx->Pin;			//上拉电阻
		P2PD = GPIOx->Pull_Down_En == ENABLE ? P2PD | GPIOx->Pin : P2PD & ~GPIOx->Pin;			//下拉电阻
		P2NCS = GPIOx->Smit_Triggle_En == ENABLE ? P2NCS |GPIOx->Pin : P2NCS & ~GPIOx->Pin;		//端口施密特触发
		P2SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P2SR | GPIOx->Pin : P2SR & ~GPIOx->Pin;	//端口电电平切换速度
		P2DR = GPIOx->Strengthen_Current_En == ENABLE ? P2DR | GPIOx->Pin : P2DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P2IE = GPIOx->Digital_Input_En == ENABLE ? P2IE | GPIOx->Pin : P2IE & ~GPIOx->Pin;		//端口电平输入使能
		P2BP = GPIOx->User_Control_En == ENABLE ? P2BP | GPIOx->Pin : P2BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P3)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P3M1 &= ~GPIOx->Pin,    P3M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P3M1 |=  GPIOx->Pin,    P3M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P3M1 |=  GPIOx->Pin,    P3M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P3M1 &= ~GPIOx->Pin,    P3M0 |=  GPIOx->Pin;     //推挽输出
		
		P3PU = GPIOx->Pull_Up_En == ENABLE ? P3PU | GPIOx->Pin : P3PU & ~GPIOx->Pin;			//上拉电阻
		P3PD = GPIOx->Pull_Down_En == ENABLE ? P3PD | GPIOx->Pin : P3PD & ~GPIOx->Pin;			//下拉电阻
		P3NCS = GPIOx->Smit_Triggle_En == ENABLE ? P3NCS |GPIOx->Pin : P3NCS & ~GPIOx->Pin;		//端口施密特触发
		P3SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P3SR | GPIOx->Pin : P3SR & ~GPIOx->Pin;	//端口电电平切换速度
		P3DR = GPIOx->Strengthen_Current_En == ENABLE ? P3DR | GPIOx->Pin : P3DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P3IE = GPIOx->Digital_Input_En == ENABLE ? P3IE | GPIOx->Pin : P3IE & ~GPIOx->Pin;		//端口电平输入使能
		P3BP = GPIOx->User_Control_En == ENABLE ? P3BP | GPIOx->Pin : P3BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P4)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P4M1 &= ~GPIOx->Pin,    P4M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P4M1 |=  GPIOx->Pin,    P4M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P4M1 |=  GPIOx->Pin,    P4M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P4M1 &= ~GPIOx->Pin,    P4M0 |=  GPIOx->Pin;     //推挽输出
		
		P4PU = GPIOx->Pull_Up_En == ENABLE ? P4PU | GPIOx->Pin : P4PU & ~GPIOx->Pin;			//上拉电阻
		P4PD = GPIOx->Pull_Down_En == ENABLE ? P4PD | GPIOx->Pin : P4PD & ~GPIOx->Pin;			//下拉电阻
		P4NCS = GPIOx->Smit_Triggle_En == ENABLE ? P4NCS |GPIOx->Pin : P4NCS & ~GPIOx->Pin;		//端口施密特触发
		P4SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P4SR | GPIOx->Pin : P4SR & ~GPIOx->Pin;	//端口电电平切换速度
		P4DR = GPIOx->Strengthen_Current_En == ENABLE ? P4DR | GPIOx->Pin : P4DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P4IE = GPIOx->Digital_Input_En == ENABLE ? P4IE | GPIOx->Pin : P4IE & ~GPIOx->Pin;		//端口电平输入使能
		P4BP = GPIOx->User_Control_En == ENABLE ? P4BP | GPIOx->Pin : P4BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P5)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P5M1 &= ~GPIOx->Pin,    P5M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P5M1 |=  GPIOx->Pin,    P5M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P5M1 |=  GPIOx->Pin,    P5M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P5M1 &= ~GPIOx->Pin,    P5M0 |=  GPIOx->Pin;     //推挽输出
		
		P5PU = GPIOx->Pull_Up_En == ENABLE ? P5PU | GPIOx->Pin : P5PU & ~GPIOx->Pin;			//上拉电阻
		P5PD = GPIOx->Pull_Down_En == ENABLE ? P5PD | GPIOx->Pin : P5PD & ~GPIOx->Pin;			//下拉电阻
		P5NCS = GPIOx->Smit_Triggle_En == ENABLE ? P5NCS |GPIOx->Pin : P5NCS & ~GPIOx->Pin;		//端口施密特触发
		P5SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P5SR | GPIOx->Pin : P5SR & ~GPIOx->Pin;	//端口电电平切换速度
		P5DR = GPIOx->Strengthen_Current_En == ENABLE ? P5DR | GPIOx->Pin : P5DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P5IE = GPIOx->Digital_Input_En == ENABLE ? P5IE | GPIOx->Pin : P5IE & ~GPIOx->Pin;		//端口电平输入使能
		P5BP = GPIOx->User_Control_En == ENABLE ? P5BP | GPIOx->Pin : P5BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P6)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P6M1 &= ~GPIOx->Pin,    P6M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P6M1 |=  GPIOx->Pin,    P6M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P6M1 |=  GPIOx->Pin,    P6M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P6M1 &= ~GPIOx->Pin,    P6M0 |=  GPIOx->Pin;     //推挽输出
		
		P6PU = GPIOx->Pull_Up_En == ENABLE ? P6PU | GPIOx->Pin : P6PU & ~GPIOx->Pin;			//上拉电阻
		P6PD = GPIOx->Pull_Down_En == ENABLE ? P6PD | GPIOx->Pin : P6PD & ~GPIOx->Pin;			//下拉电阻
		P6NCS = GPIOx->Smit_Triggle_En == ENABLE ? P6NCS |GPIOx->Pin : P6NCS & ~GPIOx->Pin;		//端口施密特触发
		P6SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P6SR | GPIOx->Pin : P6SR & ~GPIOx->Pin;	//端口电电平切换速度
		P6DR = GPIOx->Strengthen_Current_En == ENABLE ? P6DR | GPIOx->Pin : P6DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P6IE = GPIOx->Digital_Input_En == ENABLE ? P6IE | GPIOx->Pin : P6IE & ~GPIOx->Pin;		//端口电平输入使能
		P6BP = GPIOx->User_Control_En == ENABLE ? P6BP | GPIOx->Pin : P6BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    if(GPIOx->Port == GPIO_P7)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P7M1 &= ~GPIOx->Pin,    P7M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P7M1 |=  GPIOx->Pin,    P7M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P7M1 |=  GPIOx->Pin,    P7M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P7M1 &= ~GPIOx->Pin,    P7M0 |=  GPIOx->Pin;     //推挽输出
		
		P7PU = GPIOx->Pull_Up_En == ENABLE ? P7PU | GPIOx->Pin : P7PU & ~GPIOx->Pin;			//上拉电阻
		P7PD = GPIOx->Pull_Down_En == ENABLE ? P7PD | GPIOx->Pin : P7PD & ~GPIOx->Pin;			//下拉电阻
		P7NCS = GPIOx->Smit_Triggle_En == ENABLE ? P7NCS |GPIOx->Pin : P7NCS & ~GPIOx->Pin;		//端口施密特触发
		P7SR = GPIOx->Fast_Switch_Speed_En == ENABLE ? P7SR | GPIOx->Pin : P7SR & ~GPIOx->Pin;	//端口电电平切换速度
		P7DR = GPIOx->Strengthen_Current_En == ENABLE ? P7DR | GPIOx->Pin : P7DR & ~GPIOx->Pin;	//端口电流驱动能力增强
		P7IE = GPIOx->Digital_Input_En == ENABLE ? P7IE | GPIOx->Pin : P7IE & ~GPIOx->Pin;		//端口电平输入使能
		P7BP = GPIOx->User_Control_En == ENABLE ? P7BP | GPIOx->Pin : P7BP & ~GPIOx->Pin;		//端口模式用户设置
    }
    return SUCCESS;    //成功
}

/*---------------------------------------------------------
@ function  :   void ADC_Init()
@ describe  :   ADC Initialization
@ parameter :   None
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   void ADC_Configuration(ADC_InitTypeDef* ADC_InitStructure)
@ describe  :   ADC Configuration
@ parameter :   ADC_InitTypeDef* ADC_InitStructure: ADC初始化结构体指针，包含ADC配置参数
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   u8 ADC_Pin_Channel_Select(ADC_InitTypeDef* ADC_InitStructure)
@ describe  :   ADC引脚通道选择
@ parameter :   ADC_InitTypeDef* ADC_InitStructure: ADC初始化结构体指针，包含ADC配置参数
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   void PWM_Init(void)
@ describe  :   PWM Initialization
@ parameter :   None
@ date      :   2026-6-11
----------------------------------------------------------*/
void PWM_Init(void)
{
	PWMx_InitDefine PWMx_InitStructure;
	/* 输出比较模式*/
	//1.1 设置PWMn成员属性
	PWMx_InitStructure.PWM_In_Out_Mode = CCAS_OUTPUT;	//设置PWM信号方向(输出/输入方向)(PWM_CCMR1[1:0])
	PWMx_InitStructure.PWM_Output_Channel_Select = ENO1P;	//PWM1P输出使能(PWMx_ENO[7:0])
	strcpy(PWMx_InitStructure.PWM_IO_Pin_Select, "P2.4");	//选择引脚(PWMx_PS[7:0])
	PWMx_InitStructure.PWM_OC_Mode = CCMRn_PWM_MODE2;		//计数器大于比较值，输出高电平(PWMx_CCMR1[6:4])
	PWMx_InitStructure.PWM_Duty = PWMx_InitStructure.PWM_Pscr / 1000 * 0;	//占空比(PWMx_PSCRH、PWMx_PSCRL) 0~Period 对应 0~100%
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

/*---------------------------------------------------------
@ function  :   void PWM_Frequency_ARR_PSCR_Calculator(u32 set_freq, PWMx_InitDefine* PWMx_InitStructure)
@ describe  :   根据设定的PWM频率自动计算ARR和PSCR的值，并配置到PWM初始化结构体中
@ parameter :   set_freq: 设定的PWM频率
                PWMx_InitStructure: PWM初始化结构体指针，包含PWM配置参数
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   u8 PWM_Pin_Configuration(u8 PWMx, PWMx_InitDefine *PWMx_InitStructure)
@ describe  :   根据选择的PWM通道和引脚，配置对应的寄存器以使能PWM输出
@ parameter :   PWMx: PWM通道号，取值范围1~8
                PWMx_InitStructure: PWM初始化结构体指针，包含PWM配置参数
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   u8 PWM_Configuration_2(u8 PWMx, PWMx_InitDefine *PWMx_InitStructure)
@ describe  :   根据设定的PWM配置参数，配置对应的PWM通道
@ parameter :   PWMx: PWM通道号，取值范围1~8    PWM组别，PWMA，PWMB
                PWMx_InitStructure: PWM初始化结构体指针，包含PWM配置参数
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   char* strstr(const char* str1, const char* str2) 
@ describe  :   在字符串str1中查找子串str2的位置
@ parameter :   str1: 主字符串    str2: 要查找的子串
@ date      :   2026-6-11
----------------------------------------------------------*/
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

/*---------------------------------------------------------
@ function  :   void SPI_Init(void) 
@ describe  :   SPI Initialization
@ parameter :   None
@ date      :   2026-6-11
----------------------------------------------------------*/
void SPI_Init(void)
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
	
	SPI_Configuration_2(&SPI_InitStructure);
}

/*---------------------------------------------------------
@ function  :   void DMA_ADC_Init(void) 
@ describe  :   DMA_ADC Initialization
@ parameter :   None
@ date      :   2026-6-12
----------------------------------------------------------*/
u16 xdata ADC_Result_Buffer[4];		//ADC转换结果

void DMA_ADC_Init(void)
{
	DMA_ADC_InitTypeDef xdata DMA_ADC_InitStructure;
	DMA_ADC_InitStructure.DMA_Enable = ENABLE;
	DMA_ADC_InitStructure.DMA_Channel = ADC_DMA_CHANNEL_0 | ADC_DMA_CHANNEL_1 | ADC_DMA_CHANNEL_2 | ADC_DMA_CHANNEL_2;//ADC0、ADC1、ADC2、ADC3
	DMA_ADC_InitStructure.DMA_Buffer_Address = (u16)&ADC_Result_Buffer[0]; 	//设定DMA存放ADC数据的地址
	DMA_ADC_InitStructure.DMA_Times = ADC_4_Times;	//每个通道转换次数(自动采样多次并取均值)
	DMA_ADC_InitStructure.DMA_Scan_Times = 0xFFFF;	//无限循环采样
	DMA_ADC_Inilize(&DMA_ADC_InitStructure);		//调用初始化函数
}

/*---------------------------------------------------------
@ function  :   void DMA_SPI_Init(void) 
@ describe  :   DMA_SPI Initialization
@ parameter :   None
@ date      :   2026-6-12
----------------------------------------------------------*/
void DMA_SPI_Init(void)
{
	DMA_SPI_InitTypeDef xdata DMA_SPI_InitStructure;
	DMA_SPI_InitStructure.SPI_DMA_Interrupt_En = ENABLE;		//使能SPI_DMA中断
	DMA_SPI_InitStructure.SPI_DMA_TX_En = ENABLE;				//使能SPI_DMA发送数据
	DMA_SPI_InitStructure.SPI_DMA_RX_En = ENABLE;				//使能SPI_DMA接收数据
	DMA_SPI_InitStructure.SPI_DMA_Interrupt_Priority = 0x01;	//设置SPI_DMA中断优先级
	DMA_SPI_InitStructure.SPI_DMA_BusData_Priority = 0x01;		//设置SPI_DMA总线数据优先级
	
	DMA_SPI_InitStructure.SPI_DMA_Enable = ENABLE;				//使能DMA_SPI功能
	DMA_SPI_InitStructure.SPI_DMA_Master_Mode_Triggle = ENABLE;	//使能主机模式触发控制
	DMA_SPI_InitStructure.SPI_DMA_Slave_Mode_Triggle = DISABLE;	//失能从机模式触发控制
	DMA_SPI_InitStructure.SPI_DMA_FIFO_Clear_En = ENABLE;		//使能SPI_DMA操作前先清空FIFO
	
	DMA_SPI_InitStructure.SPI_DMA_Length = 0xFFFF;						//设置SPI_DMA传输总字节数(DMA_SPI_AMT为FFFFH时无限循环)
	DMA_SPI_InitStructure.SPI_DMA_Tx_Buffer = (u8)&SPI_TX_Buffer[1];	//SPI发送地址寄存器低8位
	DMA_SPI_InitStructure.SPI_DMA_Tx_Buffer |= (u8)&SPI_TX_Buffer[0];	//SPI发送地址寄存器高8位
	DMA_SPI_InitStructure.SPI_DMA_Rx_Buffer = (u8)&SPI_RX_Buffer[1];	//SPI发送地址寄存器低8位
	DMA_SPI_InitStructure.SPI_DMA_Rx_Buffer |= (u8)&SPI_RX_Buffer[0];	//SPI发送地址寄存器高8位
	
	DMA_SPI_InitStructure.SPI_DMA_AUTO_SS = ENABLE;				//SPI_DMA传输过程中自动控制SS引脚
	DMA_SPI_InitStructure.SPI_DMA_SS_Sel = 0x00;				//设置SS片选引脚
	DMA_SPI_InitStructure.SPI_DMA_Transport_Time_Gap = 0;		//设置SPI_DMA传输间隔时间(默认为3, 0 最快)
	
	DMA_SPI_Inilize(&DMA_SPI_InitStructure);
}

/*---------------------------------------------------------
@ function  :   void Timer_Init(void)
@ describe  :   Timer Initialization
@ parameter :   None
@ date      :   2026-6-12
----------------------------------------------------------*/
void Timer_Init(void)
{
    /*
    u8    TIM_Mode;       //工作模式,      TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    u8    TIM_ClkMode;    //模式选择       TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    u8    TIM_ClkSource;  //时钟源         TIM_SOURCE_SYSCLK,TIM_SOURCE_HIRC,TIM_SOURCE_X32K,TIM_SOURCE_LIRC
    u8    TIM_ClkOut;     //定时器时钟输出, ENABLE,DISABLE
    u16   TIM_Value;      //装载初值
    u8    TIM_PS;         //8位预分频器
    u8    TIM_Run;        //是否运行        ENABLE,DISABLE
    */
    //配置一个1ms定时
    TIM_InitTypeDef xdata TIM_Structure;
    TIM_Structure.TIM_Mode = TIM_16BitAutoReload;   //工作模式
    TIM_Structure.TIM_ClkMode = TIM_CLOCK_1T;       //定时器时钟源分频
    TIM_Structure.TIM_ClkSource = TIM_SOURCE_HIRC;  //定时器时钟源
    TIM_Structure.TIM_PS = 40;                      //预分频
    TIM_Structure.TIM_Value = 65535 - 1000;         //预装载初值   40MHz / 1 / 40 / 1000 = 1000 Hz
    TIM_Structure.TIM_ClkOut = ENABLE;
    TIM_Structure.TIM_Run = ENABLE;
    Timer_Inilize(Timer11, &TIM_Structure);
    
    
    //配置一个1us定时
    TIM_Structure.TIM_ClkMode = TIM_CLOCK_1T;       //定时器时钟源分频
    TIM_Structure.TIM_PS = 1;                       //预分频
    TIM_Structure.TIM_Value = 65535 - 40;           //预装载初值   40MHz / 1 / 1 / 40 = 1000000 Hz
    TIM_Structure.TIM_ClkOut = ENABLE;
    TIM_Structure.TIM_Run = DISABLE;
    Timer_Inilize(Timer3, &TIM_Structure);
    
    //配置一个1s定时
    TIM_Structure.TIM_ClkMode = TIM_CLOCK_12T;       //定时器时钟源分频
    TIM_Structure.TIM_PS = 100;                      //预分频
    TIM_Structure.TIM_Value = 65535 - 33333;         //预装载初值   40MHz / 12 / 100 / 33333 = 1 Hz
    TIM_Structure.TIM_ClkOut = ENABLE;
    TIM_Structure.TIM_Run = DISABLE;
    Timer_Inilize(Timer4, &TIM_Structure);
}

/*---------------------------------------------------------
@ function  :   void UART_Init(u8 UARTx)
@ describe  :   UART Initialization
@ parameter :   UARTx: UART1,UART2,UART3,UART4
@ date      :   2026-6-11
----------------------------------------------------------*/
void UART_Init(u8 UARTx)
{
    COMx_InitDefine COMx_InitStruct;
    COMx_InitStruct.UART_Mode = UART_8bit_BRTx;
    COMx_InitStruct.UART_BRT_Use = BRT_Timer2;
    COMx_InitStruct.UART_BaudRate = 115200;
    COMx_InitStruct.UART_RxEnable = ENABLE;
    COMx_InitStruct.BaudRateDouble = DISABLE;
    COMx_InitStruct.ParityMode = PARITY_NONE;
    COMx_InitStruct.TimeOutEnable = DISABLE;
    COMx_InitStruct.TimeOutINTEnable = DISABLE;
    COMx_InitStruct.TimeOutScale = TO_SCALE_SYSCLK;
    COMx_InitStruct.TimeOutTimer = 1000; //单位为ms
    
    UART_Configuration(UARTx, &COMx_InitStruct);
}

