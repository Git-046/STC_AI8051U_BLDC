/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "Timer.h"

//========================================================================
// 函数: u8    Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 Timer_Inilize(u8 TIM, TIM_InitTypeDef xdata *TIMx)
{
    if(TIM == Timer0)
    {
        Timer0_Stop();        //停止计数
        if(TIMx->TIM_Mode > TIM_16BitAutoReloadNoMask)    return FAIL;    //错误
        TMOD = (TMOD & ~0x03) | TIMx->TIM_Mode;     //工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 不可屏蔽16位自动重装
        if(TIMx->TIM_ClkMode > TIM_CLOCK_Ext)    return FAIL;
        Timer0_Mode_Select(TIMx->TIM_ClkMode);      //对外计数或分频, 定时12T/1T
        Timer0_CLK_Output(TIMx->TIM_ClkOut);        //输出时钟使能
        T0_Load(TIMx->TIM_Value);
        TM0PS = TIMx->TIM_PS;
        Timer0_Run(TIMx->TIM_Run);
        return    SUCCESS;        //成功
    }

    if(TIM == Timer1)
    {
        Timer1_Stop();        //停止计数
        if(TIMx->TIM_Mode > TIM_16BitAutoReloadNoMask)    return FAIL;    //错误
        TMOD = (TMOD & ~0x30) | (TIMx->TIM_Mode << 4);    //工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 停止工作
        if(TIMx->TIM_ClkMode > TIM_CLOCK_Ext)    return FAIL;
        Timer1_Mode_Select(TIMx->TIM_ClkMode);      //对外计数或分频, 定时12T/1T
        Timer1_CLK_Output(TIMx->TIM_ClkOut);        //输出时钟使能
        T1_Load(TIMx->TIM_Value);
        TM1PS = TIMx->TIM_PS;
        Timer1_Run(TIMx->TIM_Run);
        return    SUCCESS;        //成功
    }

    if(TIM == Timer2)        //Timer2,固定为16位自动重装, 中断无优先级
    {
        Timer2_Stop();    //停止计数
        Timer2_Mode_Select(TIMx->TIM_ClkMode);      //对外计数或分频, 定时12T/1T
        T2_Load(TIMx->TIM_Value);
        Timer2_CLK_Output(TIMx->TIM_ClkOut);        //输出时钟使能

        TM2PS = TIMx->TIM_PS;
        Timer2_Run(TIMx->TIM_Run);
        return    SUCCESS;        //成功
    }

    if(TIM == Timer3)        //Timer3,固定为16位自动重装, 中断无优先级
    {
        Timer3_Stop();    //停止计数
        if(TIMx->TIM_ClkMode > TIM_CLOCK_Ext)    return FAIL;
        Timer3_Mode_Select(TIMx->TIM_ClkMode);      //对外计数或分频, 定时12T/1T
        Timer3_CLK_Output(TIMx->TIM_ClkOut);        //输出时钟使能

        T3_Load(TIMx->TIM_Value);
        TM3PS = TIMx->TIM_PS;
        Timer3_Run(TIMx->TIM_Run);
        return    SUCCESS;        //成功
    }

    if(TIM == Timer4)        //Timer4,固定为16位自动重装, 中断无优先级
    {
        Timer4_Stop();    //停止计数
        if(TIMx->TIM_ClkMode > TIM_CLOCK_Ext)    return FAIL;
        Timer4_Mode_Select(TIMx->TIM_ClkMode);      //对外计数或分频, 定时12T/1T
        Timer4_CLK_Output(TIMx->TIM_ClkOut);        //输出时钟使能

        T4_Load(TIMx->TIM_Value);
        TM4PS = TIMx->TIM_PS;
        Timer4_Run(TIMx->TIM_Run);
        return    SUCCESS;        //成功
    }

    if(TIM == Timer11)        //Timer11,固定为16位自动重装, 中断无优先级
    {
        Timer11_Stop();    //停止计数
        if(TIMx->TIM_ClkMode > TIM_CLOCK_Ext)    return FAIL;
        Timer11_Mode_Select(TIMx->TIM_ClkMode);     //对外计数或分频, 定时12T/1T
        Timer11_CLK_Output(TIMx->TIM_ClkOut);       //输出时钟使能

        T11_Load(TIMx->TIM_Value);
        T11PS = TIMx->TIM_PS;
        
        if(TIMx->TIM_ClkSource > TIM_SOURCE_LIRC)
        {
            return FAIL;
        }
        else
        {
            if(TIMx->TIM_ClkSource == TIM_SOURCE_X32K)
            {
                X32KCR = 0x80 + 0x40;   //启动外部32K晶振, 低增益+0x00, 高增益+0x40.
                while (!(X32KCR & 1));  //等待时钟稳定
            }
            Timer11_CLK_Select(TIMx->TIM_ClkSource);
        }

        Timer11_Run(TIMx->TIM_Run);
        return    SUCCESS;        //成功
    }
    return FAIL;    //错误
}

void Timer_Init()
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
    TIM_Structure.TIM_ClkSource = TIM_SOURCE_LIRC;  //定时器时钟源
    TIM_Structure.TIM_PS = 111;                     //预分频
    TIM_Structure.TIM_Value = 65535 - 30030;        //预装载初值   40MHz / 12 / 111 = 30030
    TIM_Structure.TIM_ClkOut = ENABLE;
    TIM_Structure.TIM_Run = ENABLE;
    
    Timer_Inilize(Timer11, &TIM_Structure);
}

