#include "Context.h"
#include "AI8051U.h"
#include "interrupt.h"
#include "State_Machine.h"
#include "Motor.h"
#include "Timer.h"
#include "PWM.h"
#include "SPI.h"
#include "Foc.h"
#include "def.h"

/*---------------------------------------
@ function  :   void TIM11_ISR() interrupt TMR11_VECTOR
@ describe  :   定时器 11 中断处理函数
@ time      :   1ms
@ parameter :   None
@ date      :   2026-2-24
-----------------------------------------*/
void TIM11_ISR() interrupt TMR11_VECTOR
{
	if(TCON & 0x20)
    {
        TCON &= ~0x20;  //清除溢出中断标志位
    }
    
    Speed_Update(&g_ctx->motor_speed);     //更新转速、占空比等
    Motor_Speed_Ramp(&g_ctx->motor_speed); //通过速度斜坡曲线，更新速度控制信号    
    /* 保护功能 */  
    
    
    /* 状态机 */
    State_Machine();
    Motor_Run_Stop_Control();
}

/*---------------------------------------
@ function  :   void TIM4_ISR() interrupt TMR4_VECTOR
@ describe  :   定时器 4 中断处理函数
@ time      :   1us
@ parameter :   None
@ date      :   2026-3-4
-----------------------------------------*/
void TIM4_ISR() interrupt TMR4_VECTOR
		{
    if(AUXINTIF & 0x04)     //中断服务程序中，硬件自动清零
    {
        s_delay_ticks.count++;
    }
    
}

/*---------------------------------------
@ function  :   void TIM3_ISR() interrupt TMR4_VECTOR
@ describe  :   定时器 3 中断处理函数
@ time      :   1s
@ parameter :   None
@ date      :   2026-3-4
-----------------------------------------*/
void TIM3_ISR() interrupt TMR3_VECTOR
{
    
}

/*---------------------------------------
@ function  :   void ADC_ISR() interrupt ADC_VECTOR
@ describe  :   ADC中断处理函数
@ parameter :   None
@ date      :   2026-2-27
-----------------------------------------*/
void ADC_ISR() interrupt ADC_VECTOR
{
	/*中断任务*/
	//1.读取采样电流
	/*已经在DMA中自动搬运ADC采样结果了*/
	
	ADC_CONTR &= ~0x10;	//ADC转换结束标志位ADC_FLAG清零
    
    if(g_ctx->motor_state == motor_run || g_ctx->motor_state == motor_start)
    {
        /* 电流环 */
        Current_Loop();
    }
}

/*---------------------------------------
@ function  :   void DMA_SPI_isr() interrupt DMA_SPI_VECTOR
@ describe  :   DMA中断处理函数
@ parameter :   None
@ date      :   2026-2-27
-----------------------------------------*/
void DMA_SPI_isr() interrupt DMA_SPI_VECTOR
{
	static u8 i = 4;

	//轮流变换TLE5012B的获取信息指令
	SPI_TX_Buffer[i] = g_ctx->tx_cmd_arr[i];
	SPI_TX_Buffer[i+1] = g_ctx->tx_cmd_arr[i+1];
	i += 2;

	if(DMA_SPI_STA & 0x01)
	{
		switch(i)
		{
			case 2:
				g_ctx->motor_data.machine_angle = (s16)((SPI_RX_Buffer[0] << 8) | SPI_RX_Buffer[1]) * PI_DQ14;
				while(g_ctx->motor_data.machine_angle > PI)
				    g_ctx->motor_data.machine_angle -= 2 * PI;
				while(g_ctx->motor_data.machine_angle < -PI)
				    g_ctx->motor_data.machine_angle += 2 * PI;
				g_ctx->motor_data.electrical_angle = g_ctx->motor_data.machine_angle * MOTOR_POLE_PAIRS;
				break;
			
			case 4:
				g_ctx->motor_data.speed = (s16)((SPI_RX_Buffer[0] << 8) | SPI_RX_Buffer[1]) * 60;
				g_ctx->motor_data.loop  = (SPI_RX_Buffer[2] << 8) | SPI_RX_Buffer[3];
				break;
			
			default:
				break;
		}
		
		DMA_SPI_STA = DMA_SPI_STA & ~0x01;							//中断标志位清零
        if(i > 7)	//重复TLE5012B的指令数组下标
        {
            i = 4;
        }
	}
	
		if(DMA_SPI_STA & 0x02)	DMA_SPI_STA = DMA_SPI_STA & ~0x02;  //满数据自动丢弃数据位清零
		if(DMA_SPI_STA & 0x04)  DMA_SPI_STA = DMA_SPI_STA & ~0x04;	//数据覆写导致传输失败的标志位清零
	
	
}


/*------------------------------------------------------------
@ function  :   void PWMB_Capture() interrupt PWMB_VECTOR
@ describe  :   PWMB捕获/比较中断处理函数
@ parameter :   None
@ date      :   2026-3-2
-------------------------------------------------------------*/
u16 xdata cc1 = 0, cc2 = 0;     //通道1和通道2
u16 xdata cc1_last = 0;         //通道1上一次的捕获值
u16 xdata cc1_rise_last = 0;    //上升沿的上一次捕获值
u16 xdata count = 0;            //计数器溢出次数

volatile u32 xdata period_ticks = 0;        //周期计数值
volatile u32 xdata high_ticks = 0;


void PWMB_Capture() interrupt PWMB_VECTOR
{
    u8 sr1 = 0;
    u32 total_ticks = 0;
    u32 high_ticks_temp = 0;
    
    sr1 = ReadPWMB(PWMB_SR1);
    
    //1.1 处理更新中断（溢出）
    if(sr1 & 0x01)
    {
        count++;
        if(count > 65535)
        {
            count = 65535;
        }
    }
    
    //1.2 处理捕获数据中断
    if(sr1 & 0x04)      //PWM6（捕获上升沿）
    {
        cc1 = ReadPWMB((u8)(&PWMB_CCR6H));    //异步读取寄存器数据
        cc1 = (cc1 << 8) | ReadPWMB((u8)&PWMB_CCR6L);
        if(cc1_last != 0)
        {
            if(cc1 >= cc1_last)     //没有溢出，使用当前的count
            {
                total_ticks = (u32)count * 65536 + (cc1 - cc1_last);
            }
            else                    //有溢出
            {
                if(count > 0)       //一般溢出的情况
                {
                    total_ticks = (u32)(count - 1) * 65536 + (65536 - cc1_last + cc1);
                }
                else if(count == 0) //溢出瞬间捕获的情况
                {
                    total_ticks = (u32)(65536 - cc1_last + cc1);
                }
            }
        }
        
        period_ticks = total_ticks;
        count = 0;      //溢出计数器复位
    }
    cc1_last = cc1;     //保留上一次的值
    cc1_rise_last = cc1;
    
    //2.1 处理捕获数据中断
    if(sr1 & 0x08)     //PWM7（捕获下降沿）
    {
        cc2 = ReadPWMB((u8)(&PWMB_CCR7H));    //异步读取寄存器数据
        cc2 = (cc2 << 8) | ReadPWMB((u8)(&PWMB_CCR7L));
        
        //计算高电平时间
        if(cc1_rise_last != 0)
        {
            if(cc2 >= cc1_rise_last)
            {
                high_ticks_temp = count * 65536ul + (cc2 - cc1_rise_last);
            }
            else
            {
                if(count > 0)
                {
                    high_ticks_temp = (u32)(count - 1) * 65536 + (65536 - cc1_rise_last + cc2);
                }
                else
                {
                    high_ticks_temp = (u32)(65536 + (65536 - cc1_rise_last + cc2));
                }
            }
            high_ticks = high_ticks_temp;
            
            // 如果有周期数据，可以立即计算
        }
    }
    
    /* 计算捕获PWM信号的频率与占空比 */
    g_ctx->motor_speed.pwm_freq = PWMB_SOURCE_FREQUENCY / period_ticks;
    if(high_ticks <= period_ticks)
    {
        g_ctx->motor_speed.pwm_duty = high_ticks * PWM_DUTY_BASE / period_ticks;
    }
    else
    {
        g_ctx->motor_speed.pwm_duty = 0;   //捕获错误
    }
    
    
    // 清除中断标志
    WritePWMB((u8)(&PWMB_SR1), 0x00);
    
}