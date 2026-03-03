/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/
#include "stdarg.h"
#include "string.h"
#include "Clock.h"
#include "Delay.h"

/*---------------------------------------------------------------------------------
@ function  :   void Clock_Init(void)
@ describe  :   时钟初始化函数
@ parameter :   None
@ date      :   2026-2-25
-----------------------------------------------------------------------------------*/
void Clock_Init(void)
{
    Clock_Struct xdata clock_struct;
    memset((void*)&clock_struct, 0, sizeof(Clock_Struct));
    
    clock_struct.Main_clock_source = MCKSEL_HIRC;   //主时钟源选择      MCKSEL_HIRC(40MHz)
    clock_struct.IRC_en = ENABLE;                   //使能对应的时钟源
    clock_struct.Main_clock_div = 1;                //主时钟源分频
    clock_struct.PLL_select = PLL_SEL;              //PLL频率选择
    clock_struct.HSIO_select = HSCK_PLL;            //高速IO时钟源选择             144MHz
    clock_struct.HSIO_div = 1;                      //高速IO时钟分频               144MHz
    clock_struct.TFPU_div = 2;                      //TFPU对时钟源(HSIO)分频       72MHz
    clock_struct.PLL_en = ENABLE;                   //PLL倍频功能使能 
    clock_struct.PLL_source_div = 4;                //PLL的时钟源需要为12MHz(8~16MHz)
    clock_struct.PWMA_div = 2;                      //PWMA时钟源分频               72MHz
    clock_struct.PWMB_div = 1;                      //PWMB时钟源分频               144MHz
}

void Clock_Configuration(Clock_Struct xdata *clock_struct)
{
    /*1. 主时钟选择*/
    CLKSEL &= ~0x0F | clock_struct->Main_clock_source;
    /*2. 使能时钟源 */
    HIRCCR &= ~0x80 | clock_struct->IRC_en;
    /*3. 对主时钟分频得到系统时钟 */
    CLKDIV = clock_struct->Main_clock_div;
    
    /*4. 其他时钟设置 */
    CLKSEL &= ~0x80 | clock_struct->PLL_select;         //PLL频率选择
    CLKSEL &= ~0x40 | clock_struct->HSIO_select;        //高速IO时钟源选择
    HSCLKDIV = clock_struct->HSIO_div;                  //高速IO时钟源分频
    TFPU_CLKDIV = 1;                                    //TFPU的时钟源分频
    USBCLK &= ~0x80 | clock_struct->PLL_en;             //PLL倍频功能使能
    USBCLK &= ~0x60 | clock_struct->PLL_source_div;     //PLL时钟源分频
    PWMA_CLKDIV = clock_struct->PWMA_div;               //PWMA时钟源分频
    PWMB_CLKDIV = clock_struct->PWMB_div;               //PWMB时钟源分频
}


//========================================================================  
// 函数: void HIRCClkConfig(u8 div)
// 描述: 高速IRC时钟初始化程序.
// 参数: div: 时钟分频系数.
// 返回: none.
// 版本: V1.0, 2022-04-03
//========================================================================
void HIRCClkConfig(u8 div)
{
    HIRCCR = 0x80;      //启动内部 IRC
    while (!(HIRCCR & 1)); //等待时钟稳定
    CLKDIV = div;
    CLKSEL = 0x00;      //选择内部 IRC ( 默认 )
}

//========================================================================
// 函数: void XOSCClkConfig(u8 div)
// 描述: 外部晶振时钟初始化程序.
// 参数: div: 时钟分频系数.
// 返回: none.
// 版本: V1.0, 2022-04-03
//========================================================================
void XOSCClkConfig(u8 div)
{
    XOSCCR = 0xc0;      //启动外部晶振
    while (!(XOSCCR & 1));  //等待时钟稳定
    CLKDIV = div;       //时钟分频
    CLKSEL = 0x01;      //选择外部晶振
}

//========================================================================
// 函数: void IRC32KClkConfig(u8 div)
// 描述: 低速32K IRC时钟初始化程序.
// 参数: div: 时钟分频系数.
// 返回: none.
// 版本: V1.0, 2022-04-03
//========================================================================
void IRC32KClkConfig(u8 div)
{
    IRC32KCR = 0x80;    //启动内部 32K IRC
    while (!(IRC32KCR & 1)); //等待时钟稳定
    CLKDIV = div;       //时钟分频
    CLKSEL = 0x03;      //选择内部 32K
}

//========================================================================
// 函数: void HSPllClkConfig(u8 clksrc, u8 pcki, u8 pllsel, u8 div)
// 描述: 高速IO时钟初始化程序.
// 参数: clksrc: 系统时钟选择, MCLKSEL_HIRC/MCLKSEL_XIRC/MCLKSEL_X32K/MCLKSEL_I32K/MCLKSEL_PLL/MCLKSEL_PLL2/MCLKSEL_I48M.
// 参数: pllsel: PLL时钟选择, PLL_96M/PLL_144M.
// 参数: div: 高速IO时钟分频系数.
// 返回: none.
// 版本: V1.0, 2022-04-03
//========================================================================
void HSPllClkConfig(u8 clksrc, u8 pllsel, u8 div)
{
	//系统时钟选择, MCLKSEL_HIRC/MCLKSEL_XIRC/MCLKSEL_X32K/MCLKSEL_I32K/MCLKSEL_PLL/MCLKSEL_PLL2/MCLKSEL_I48M
    MainClockSel(clksrc);

    //系统时钟 n 分频作为PLL时钟源,确保分频后为12M
    USBCLK &= ~PCKI_MSK;
#if (MAIN_Fosc == 12000000UL)
    USBCLK |= PCKI_D1;          //PLL输入时钟1分频
#elif (MAIN_Fosc == 24000000UL)
    USBCLK |= PCKI_D2;          //PLL输入时钟2分频
#elif (MAIN_Fosc == 48000000UL)
    USBCLK |= PCKI_D4;          //PLL输入时钟4分频
#elif (MAIN_Fosc == 96000000UL)
    USBCLK |= PCKI_D8;          //PLL输入时钟8分频
#else
    USBCLK |= PCKI_D4;          //默认PLL输入时钟4分频
#endif

    PLLClockSel(pllsel);        //PLL时钟选择, PLL_96M/PLL_144M
    PLLEnable(ENABLE);          //PLL倍频使能, ENABLE/DISABLE
    delay_ms(1);                //等待PLL锁频
    HSIOClockSel(HSCK_PLL);     //高速IO时钟选择, HSCK_MCLK/HSCK_PLL
    HSClockDiv(div);            //高速IO时钟分频系数
	
	//2025.10.21
//	DMAIR = 0x3E;	//选择 系统时钟 作为TFPU时钟源
	DMAIR = 0x3F;	//选择 PLL时钟  作为TFPU时钟源
	TFPU_CLKDIV = 2;	//TFPU工作频率要求低于108MHz
}
