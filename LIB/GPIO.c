/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "GPIO.h"

void GPIO_Init()
{
	//1.设置PWM波输出对应的IO口
	GPIO_InitTypeDef GPIOx_InitStructure;
	
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

//========================================================================
// 函数: u8 GPIO_Inilize(u8 GPIO, GPIO_InitTypeDef *GPIOx)
// 描述: 初始化IO口.
// 参数: GPIOx: 结构参数,请参考头文件里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 GPIO_Inilize(u8 GPIO, GPIO_InitTypeDef *GPIOx)
{
    if(GPIO > GPIO_P7)               return FAIL;    //错误
    if(GPIOx->Mode > GPIO_OUT_PP)    return FAIL;    //错误
	
    if(GPIO == GPIO_P0)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P0M1 &= ~GPIOx->Pin,    P0M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P0M1 |=  GPIOx->Pin,    P0M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P0M1 |=  GPIOx->Pin,    P0M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P0M1 &= ~GPIOx->Pin,    P0M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P1)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P1M1 &= ~GPIOx->Pin,    P1M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P1M1 |=  GPIOx->Pin,    P1M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P1M1 |=  GPIOx->Pin,    P1M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P1M1 &= ~GPIOx->Pin,    P1M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P2)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P2M1 &= ~GPIOx->Pin,    P2M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P2M1 |=  GPIOx->Pin,    P2M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P2M1 |=  GPIOx->Pin,    P2M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P2M1 &= ~GPIOx->Pin,    P2M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P3)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P3M1 &= ~GPIOx->Pin,    P3M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P3M1 |=  GPIOx->Pin,    P3M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P3M1 |=  GPIOx->Pin,    P3M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P3M1 &= ~GPIOx->Pin,    P3M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P4)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P4M1 &= ~GPIOx->Pin,    P4M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P4M1 |=  GPIOx->Pin,    P4M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P4M1 |=  GPIOx->Pin,    P4M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P4M1 &= ~GPIOx->Pin,    P4M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P5)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P5M1 &= ~GPIOx->Pin,    P5M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P5M1 |=  GPIOx->Pin,    P5M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P5M1 |=  GPIOx->Pin,    P5M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P5M1 &= ~GPIOx->Pin,    P5M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P6)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P6M1 &= ~GPIOx->Pin,    P6M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P6M1 |=  GPIOx->Pin,    P6M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P6M1 |=  GPIOx->Pin,    P6M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P6M1 &= ~GPIOx->Pin,    P6M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P7)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P7M1 &= ~GPIOx->Pin,    P7M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P7M1 |=  GPIOx->Pin,    P7M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P7M1 |=  GPIOx->Pin,    P7M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P7M1 &= ~GPIOx->Pin,    P7M0 |=  GPIOx->Pin;     //推挽输出
    }
    return SUCCESS;    //成功
}

//========================================================================
// 函数: u8 GPIO_INT_Inilize(u8 GPIO, GPIO_Int_InitTypeDef *GPIOx)
// 描述: 初始化IO口中断.
// 参数: GPIOx: 结构参数,请参考头文件里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2025-02-11
//========================================================================
u8 GPIO_INT_Inilize(u8 GPIO, GPIO_Int_InitTypeDef *GPIOx)
{
    if(GPIO > GPIO_P7)                   return FAIL;    //错误
    if(GPIOx->Mode > GPIO_HIGH_LEVEL)    return FAIL;    //错误

    if(GPIO == GPIO_P0)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P0IM1 &= ~GPIOx->Pin,    P0IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P0IM1 &= ~GPIOx->Pin,    P0IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P0IM1 |=  GPIOx->Pin,    P0IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P0IM1 |=  GPIOx->Pin,    P0IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P0WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P0WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P0INTE |=  GPIOx->Pin;     //中断使能
        else                                    P0INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P1)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P1IM1 &= ~GPIOx->Pin,    P1IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P1IM1 &= ~GPIOx->Pin,    P1IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P1IM1 |=  GPIOx->Pin,    P1IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P1IM1 |=  GPIOx->Pin,    P1IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P1WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P1WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P1INTE |=  GPIOx->Pin;     //中断使能
        else                                    P1INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P2)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P2IM1 &= ~GPIOx->Pin,    P2IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P2IM1 &= ~GPIOx->Pin,    P2IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P2IM1 |=  GPIOx->Pin,    P2IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P2IM1 |=  GPIOx->Pin,    P2IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P2WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P2WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P2INTE |=  GPIOx->Pin;     //中断使能
        else                                    P2INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P3)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P3IM1 &= ~GPIOx->Pin,    P3IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P3IM1 &= ~GPIOx->Pin,    P3IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P3IM1 |=  GPIOx->Pin,    P3IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P3IM1 |=  GPIOx->Pin,    P3IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P3WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P3WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P3INTE |=  GPIOx->Pin;     //中断使能
        else                                    P3INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P4)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P4IM1 &= ~GPIOx->Pin,    P4IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P4IM1 &= ~GPIOx->Pin,    P4IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P4IM1 |=  GPIOx->Pin,    P4IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P4IM1 |=  GPIOx->Pin,    P4IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P4WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P4WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P4INTE |=  GPIOx->Pin;     //中断使能
        else                                    P4INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P5)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P5IM1 &= ~GPIOx->Pin,    P5IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P5IM1 &= ~GPIOx->Pin,    P5IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P5IM1 |=  GPIOx->Pin,    P5IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P5IM1 |=  GPIOx->Pin,    P5IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P5WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P5WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P5INTE |=  GPIOx->Pin;     //中断使能
        else                                    P5INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P6)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P6IM1 &= ~GPIOx->Pin,    P6IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P6IM1 &= ~GPIOx->Pin,    P6IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P6IM1 |=  GPIOx->Pin,    P6IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P6IM1 |=  GPIOx->Pin,    P6IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P6WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P6WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P6INTE |=  GPIOx->Pin;     //中断使能
        else                                    P6INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P7)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P7IM1 &= ~GPIOx->Pin,    P7IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P7IM1 &= ~GPIOx->Pin,    P7IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P7IM1 |=  GPIOx->Pin,    P7IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P7IM1 |=  GPIOx->Pin,    P7IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P7WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P7WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P7INTE |=  GPIOx->Pin;     //中断使能
        else                                    P7INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIOx->Priority == Priority_0)        PINIPH &= ~(1<<GPIO),    PINIPL &= ~(1<<GPIO);     //中断优先级为0
    if(GPIOx->Priority == Priority_1)        PINIPH &= ~(1<<GPIO),    PINIPL |=  (1<<GPIO);     //中断优先级为1
    if(GPIOx->Priority == Priority_2)        PINIPH |=  (1<<GPIO),    PINIPL &= ~(1<<GPIO);     //中断优先级为2
    if(GPIOx->Priority == Priority_3)        PINIPH |=  (1<<GPIO),    PINIPL |=  (1<<GPIO);     //中断优先级为3

    return SUCCESS;    //成功
}
