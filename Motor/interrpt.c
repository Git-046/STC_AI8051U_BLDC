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
@ describe  :   ��ʱ�� 11 �жϴ�������
@ time      :   1ms
@ parameter :   None
@ date      :   2026-2-24
-----------------------------------------*/
void TIM11_ISR() interrupt TMR11_VECTOR
{
	if(TCON & 0x20)
    {
        TCON &= ~0x20;  //�������жϱ�־λ
    }
    
    Speed_Update(&g_ctx->motor_speed);     //����ת�١�ռ�ձȵ�
    Motor_Speed_Ramp(&g_ctx->motor_speed); //ͨ���ٶ�б�����ߣ������ٶȿ����ź�    
    /* �������� */  
    
    
    /* ״̬�� */
    State_Machine();
    Motor_Run_Stop_Control();
}

/*---------------------------------------
@ function  :   void TIM4_ISR() interrupt TMR4_VECTOR
@ describe  :   ��ʱ�� 4 �жϴ�������
@ time      :   1us
@ parameter :   None
@ date      :   2026-3-4
-----------------------------------------*/
void TIM4_ISR() interrupt TMR4_VECTOR
		{
    if(AUXINTIF & 0x04)     //�жϷ�������У�Ӳ���Զ�����
    {
        s_delay_ticks.count++;
    }
    
}

/*---------------------------------------
@ function  :   void TIM3_ISR() interrupt TMR4_VECTOR
@ describe  :   ��ʱ�� 3 �жϴ�������
@ time      :   1s
@ parameter :   None
@ date      :   2026-3-4
-----------------------------------------*/
void TIM3_ISR() interrupt TMR3_VECTOR
{
    
}

/*---------------------------------------
@ function  :   void ADC_ISR() interrupt ADC_VECTOR
@ describe  :   ADC�жϴ�������
@ parameter :   None
@ date      :   2026-2-27
-----------------------------------------*/
void ADC_ISR() interrupt ADC_VECTOR
{
	/*�ж�����*/
	//1.��ȡ��������
	/*�Ѿ���DMA���Զ�����ADC���������*/
	
	ADC_CONTR &= ~0x10;	//ADCת��������־λADC_FLAG����
    
    if(g_ctx->motor_state == motor_run || g_ctx->motor_state == motor_start)
    {
        /* ������ */
        Current_Loop();
    }
}

/*---------------------------------------
@ function  :   void DMA_SPI_isr() interrupt DMA_SPI_VECTOR
@ describe  :   DMA�жϴ�������
@ parameter :   None
@ date      :   2026-2-27
-----------------------------------------*/
void DMA_SPI_isr() interrupt DMA_SPI_VECTOR
{
	static u8 i = 4;

	//�����任TLE5012B�Ļ�ȡ��Ϣָ��
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
		
		DMA_SPI_STA = DMA_SPI_STA & ~0x01;							//�жϱ�־λ����
        if(i > 7)	//�ظ�TLE5012B��ָ�������±�
        {
            i = 4;
        }
	}
	
		if(DMA_SPI_STA & 0x02)	DMA_SPI_STA = DMA_SPI_STA & ~0x02;  //�������Զ���������λ����
		if(DMA_SPI_STA & 0x04)  DMA_SPI_STA = DMA_SPI_STA & ~0x04;	//���ݸ�д���´���ʧ�ܵı�־λ����
	
	
}


/*------------------------------------------------------------
@ function  :   void PWMB_Capture() interrupt PWMB_VECTOR
@ describe  :   PWMB����/�Ƚ��жϴ�������
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
    
    sr1 = ReadPWMB(PWMB_SR1);
    
    //1.1 ���������жϣ������
    if(sr1 & 0x01)
    {
        count++;
        if(count > 65535)
        {
            count = 65535;
        }
    }
    
    //1.2 �������������ж�
    if(sr1 & 0x04)      //PWM6�����������أ�
    {
        cc1 = ReadPWMB((u8)(&PWMB_CCR6H));    //�첽��ȡ�Ĵ�������
        cc1 = (cc1 << 8) | ReadPWMB((u8)&PWMB_CCR6L);
        if(cc1_last != 0)
        {
            if(cc1 >= cc1_last)     //û�������ʹ�õ�ǰ��count
            {
                total_ticks = (u32)count * 65536 + (cc1 - cc1_last);
            }
            else                    //�����
            {
                if(count > 0)       //һ����������
                {
                    total_ticks = (u32)(count - 1) * 65536 + (65536 - cc1_last + cc1);
                }
                else if(count == 0) //���˲�䲶������
                {
                    total_ticks = (u32)(65536 - cc1_last + cc1);
                }
            }
        }
        
        period_ticks = total_ticks;
        count = 0;      //�����������λ
    }
    cc1_last = cc1;     //������һ�ε�ֵ
    cc1_rise_last = cc1;
    
    //2.1 �������������ж�
    if(sr1 & 0x08)     //PWM7�������½��أ�
    {
        cc2 = ReadPWMB((u8)(&PWMB_CCR7H));    //�첽��ȡ�Ĵ�������
        cc2 = (cc2 << 8) | ReadPWMB((u8)(&PWMB_CCR7L));
        
        //����ߵ�ƽʱ��
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
            
            // ������������ݣ�������������
        }
    }
    
    /* ���㲶��PWM�źŵ�Ƶ����ռ�ձ� */
    g_ctx->motor_speed.pwm_freq = PWMB_SOURCE_FREQUENCY / period_ticks;
    if(high_ticks <= period_ticks)
    {
        g_ctx->motor_speed.pwm_duty = high_ticks * PWM_DUTY_BASE / period_ticks;
    }
    else
    {
        g_ctx->motor_speed.pwm_duty = 0;   //�������
    }
    
    
    // ����жϱ�־
    WritePWMB((u8)(&PWMB_SR1), 0x00);
    
}