/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/
#include "AI8051U.h"
#include "Timer.h"
#include "def.h"

Delay_Ticks s_delay_ticks;
Delay_Ticks ms_delay_ticks;

/*------------------------------------------------------
@ function  :   void Timer_Delay_us(u16 us)
@ describe  :   定时器TIM3延时函数(查询法)
@ parameter :   u16 us 延时时间(1~1000us)
@ date      :   2026-3-4
-------------------------------------------------------*/
void Timer_Delay_us(u16 us)
{
    u32 start, current, elapsed;
    // 参数范围检查
    if (us == 0 || us > 1000) 
    {
        us = 1;  // 默认最小1us
    }
    
    start = (u16)T3H << 8 + T3L;  // 读取当前计数值
    
    // 查询方式等待
    while (1)
    {
        current = (u16)T3H << 8 + T3L;
        
        // 处理计数器溢出
        if (current >= start) 
        {
            elapsed = current - start;
        } 
        else 
        {
            elapsed = (0xFFFF - start) + current + 1;
        }
        
        if (elapsed >= us) 
        {
            break;
        }
    }
}

/*------------------------------------------------------
@ function  :   u8 Timer_Delay_s(u16 s)
@ describe  :   定时器TIM3延时函数(中断法)
@ parameter :   u16 s 延时时间(0~1000s)
@ date      :   2026-3-5
-------------------------------------------------------*/
u8 Timer_Delay_s(u16 s)
{
    //1.参数范围检查
    if (s == 0) return 0;
    else if(s > 1000)
    {
        s = 1000;  // 默认最大1000s
    }
    
    //2.启动定时器
    if(s_delay_ticks.start == 0)
    {
        s_delay_ticks.count = 0;    //计数清零
        s_delay_ticks.end = 0;      //结束标志位清零
        s_delay_ticks.start = 1;    //开始计数
        Timer3_Run(1); //启动定时器
    }
    
    if(s_delay_ticks.count >= s)
    {
        s_delay_ticks.end = 1;      //到达延时时间
        s_delay_ticks.start = 0;   //开始标志位清零
        Timer3_Stop();  //停止定时器
    }
    else
    {
        s_delay_ticks.end = 0;      //未到达延时时间
    }
    
    //3.返回定时器完成状态
    return s_delay_ticks.end;   //返回延时状态
}

/*------------------------------------------------------
@ function  :   u8 Timer_Delay_ms(u16 ms)
@ describe  :   定时器TIM4延时函数(中断法)
@ parameter :   u16 ms 延时时间(0~1000ms)
@ date      :   2026-3-5
-------------------------------------------------------*/
u8 Timer_Delay_ms(u16 ms)
{
    //1.参数范围检查
    if (ms == 0) return 0;
    else if(ms > 1000)
    {
        ms = 1000;  // 默认最大1000ms
    }
    
    //2.启动定时器
    if(ms_delay_ticks.start == 0)
    {
        ms_delay_ticks.count = 0;    //计数清零
        ms_delay_ticks.end = 0;      //结束标志位清零
        ms_delay_ticks.start = 1;    //开始计数
        Timer3_Run(1);  //启动定时器
    }
    
    if(ms_delay_ticks.count >= ms)
    {
        ms_delay_ticks.end = 1;     //延时结束标志位
        ms_delay_ticks.start = 0;   //开始标志位清零
        Timer3_Stop();  //停止定时器
    }
    else
    {
        ms_delay_ticks.end = 0;
    }
    
    //3.返回定时器完成状态
    return ms_delay_ticks.end;
}

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

