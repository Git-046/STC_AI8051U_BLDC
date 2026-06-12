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
@ describe  :   Timer 11 Interrupt Handler
@ time      :   1ms
@ parameter :   None
@ date      :   2026-2-24
-----------------------------------------*/
void TIM11_ISR() interrupt TMR11_VECTOR
{
	if(TCON & 0x20)
    {
        TCON &= ~0x20;  //Clear Timer 11 interrupt flag
    }
    
    Speed_Update(&g_ctx->motor_speed);     //Update motor speed and pwm duty
    Motor_Speed_Ramp(&g_ctx->motor_speed); //Ramp motor speed

    /* Protection */  
    
    
    /* State Machine */
    State_Machine();
    Motor_Run_Stop_Control();
}

/*---------------------------------------
@ function  :   void TIM4_ISR() interrupt TMR4_VECTOR
@ describe  :   Timer 4 Interrupt Handler
@ time      :   1us
@ parameter :   None
@ date      :   2026-3-4
-----------------------------------------*/
void TIM4_ISR() interrupt TMR4_VECTOR
	{
    if(AUXINTIF & 0x04)     //Clear Timer 4 interrupt flag
    {
        AUXINTIF &= ~0x04;
    }
    {
        s_delay_ticks.count++;
    }
    
}

/*---------------------------------------
@ function  :   void TIM3_ISR() interrupt TMR3_VECTOR
@ describe  :   Timer 3 Interrupt Handler
@ time      :   1s
@ parameter :   None
@ date      :   2026-3-4
-----------------------------------------*/
void TIM3_ISR() interrupt TMR3_VECTOR
{
    
}

/*---------------------------------------
@ function  :   void ADC_ISR() interrupt ADC_VECTOR
@ describe  :   ADC Interrupt Handler
@ parameter :   None
@ date      :   2026-2-27
-----------------------------------------*/
void ADC_ISR() interrupt ADC_VECTOR
{
	//1.Obtain current sampling results

	/*Auto sampling the current in dma */
	
	ADC_CONTR &= ~0x10;	//Clear ADC convert finish interrupt flag
    
    if(g_ctx->motor_state == motor_run || g_ctx->motor_state == motor_start)
    {
        /* Current Loop */
        Current_Loop();
    }
}

/*---------------------------------------
@ function  :   void DMA_SPI_isr() interrupt DMA_SPI_VECTOR
@ describe  :   DMA Interrupt Handler
@ parameter :   None
@ date      :   2026-2-27
-----------------------------------------*/
void DMA_SPI_isr() interrupt DMA_SPI_VECTOR
{
	static u8 i = 4;

	if(DMA_SPI_STA & 0x01)//SPI_DMA interrupt flag (SPIIF)
	{
        //Change the CMD of magenitic encoder for the next data acquisition
        SPI_TX_Buffer[i] = g_ctx->tx_cmd_arr[i];
	    SPI_TX_Buffer[i+1] = g_ctx->tx_cmd_arr[i+1];
	    i += 2;

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
		
		DMA_SPI_STA = DMA_SPI_STA & ~0x01;							//clear SPI DMA transfer complete flag

        if(i > 7)	//repete the SPI transmission for the next data acquisition after finishing the current data acquisition
        {
            i = 4;
        }
	}
	
		if(DMA_SPI_STA & 0x02)	DMA_SPI_STA = DMA_SPI_STA & ~0x02;  //clear SPI DMA transfer error flag
		if(DMA_SPI_STA & 0x04)  DMA_SPI_STA = DMA_SPI_STA & ~0x04;	//clear SPI DMA receive complete flag
	
}


/*------------------------------------------------------------
@ function  :   void PWMB_Capture() interrupt PWMB_VECTOR
@ describe  :   PWMB Capture Interrupt Handler
@ parameter :   None
@ date      :   2026-3-2
-------------------------------------------------------------*/
static u16 xdata cc1 = 0, cc2 = 0;
static u16 xdata cc1_last = 0;
static u16 xdata cc1_rise_last = 0;
static u16 xdata count = 0;

static volatile u32 xdata period_ticks = 0;
static volatile u32 xdata high_ticks = 0;

void PWMB_Capture() interrupt PWMB_VECTOR
{
    u8 sr1 = 0;
    u32 total_ticks = 0;
    u32 high_ticks_temp = 0;
    
    //read status register
    sr1 = ReadPWMB(PWMB_SR1);
    
    //1. Update interrupt
    if(sr1 & PWM_UIE)
    {
        count++;
        if(count > 65535)
        {
            count = 65535;
        }
        // WritePWMB(PWMB_SR1, 0x00);  //clear update interrupt flag
    }
    
    //2.Capture compare interrupt
    if(sr1 & PWM_CC6IE)      //Capture rising edge interrupt flag
    {
        cc1 = ReadPWMB((u8)(&PWMB_CCR6H));    //Read PWM capture register
        cc1 = (cc1 << 8) | ReadPWMB((u8)&PWMB_CCR6L);
        if(cc1_last != 0)
        {
            if(cc1 >= cc1_last)     //without overflow
            {
                total_ticks = (u32)count * 65536 + (cc1 - cc1_last);
            }
            else                    //with overflow
            {
                if(count > 0)       //one overflow occurred
                {
                    total_ticks = (u32)(count - 1) * 65536 + (65536 - cc1_last + cc1);
                }
                else if(count == 0) //no overflow occurred
                {
                    total_ticks = (u32)(65536 - cc1_last + cc1);
                }
            }
        }
        
        period_ticks = total_ticks;
        count = 0;      //reset overflow counter
    }
    cc1_last = cc1;     //update the last captured value
    cc1_rise_last = cc1;

    if(sr1 & PWM_CC7IE)     //capthure falling edge interrupt
    {
        cc2 = ReadPWMB((u8)(&PWMB_CCR7H));
        cc2 = (cc2 << 8) | ReadPWMB((u8)(&PWMB_CCR7L));
        
        //calculate high level time
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

        }
    }
    
    /* calculate PWM frequency and duty cycle */
    g_ctx->motor_speed.pwm_freq = PWMB_SOURCE_FREQUENCY / period_ticks;
    if(high_ticks <= period_ticks)
    {
        g_ctx->motor_speed.pwm_duty = high_ticks * PWM_DUTY_BASE / period_ticks;
    }
    else
    {
        g_ctx->motor_speed.pwm_duty = 0;   //case error, set duty to 0
    }
    
    
    // clear capture compare interrupt flags
    WritePWMB(PWMB_SR1, 0x00);
    
}