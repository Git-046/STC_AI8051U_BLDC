/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __CLOCK_H
#define __CLOCK_H

#include "config.h"

//========================================================================
//                              时钟设置
//========================================================================

#define MainClockSel(n)     CLKSEL = (CLKSEL & ~0x0f) | (n)         /* 系统时钟选择 */
#define PLLClockSel(n)      CLKSEL = (CLKSEL & ~0x80) | (n<<7)      /* PLL时钟选择 */
#define HSIOClockSel(n)     CLKSEL = (CLKSEL & ~0x40) | (n<<6)      /* 高速IO时钟选择 */
#define PLLClockIn(n)       USBCLK = (USBCLK & ~0x60) | (n<<4)      /* 系统时钟 n 分频作为PLL时钟源,确保分频后为12M */
#define PLLEnable(n)        USBCLK = (USBCLK & ~0x80) | (n<<7)      /* PLL倍频使能 */
#define HSClockDiv(n)       HSCLKDIV = (n)       	 				/* 高速IO时钟分频系数 */

//========================================================================
//                              定义声明
//========================================================================

/* 高速IO时钟选择参数 */
#define HSCK_MCLK       0
#define HSCK_PLL        1

/* PLL时钟选择参数 */
#define PLL_96M         0
#define PLL_144M        1
#define PLL_SEL         PLL_144M

/* 系统时钟选择参数 */
#define MCKSEL_HIRC         0x00    //内部高精度IRC
#define MCKSEL_XOSC         0x01    //外部高速晶振
#define MCKSEL_X32K         0x02    //外部32KHz晶振
#define MCKSEL_IRC32K       0x03    //内部32KHz晶振
#define MCKSEL_PLL          0x04    //内部PLL时钟
#define MCKSLE_PLL_DIV_2    0x08    //内部PLL时钟2分频
#define MCKSLE_IRC48        0x0C    //内部高速时钟48MHz

/* 系统时钟 n 分频作为PLL时钟源参数,确保分频后为12M */
#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

/* 外部振荡器时钟源类型 */
#define XOSC_TYPE_1         0       //外部时钟源是外部时钟信号（或有源晶振）
#define XOSC_TYPE_2         1       //外部时钟源是晶体振荡器
#define XOSC_BAND_48_24     0x00    //外部晶体振荡器频率在48~24MHz
#define XOSC_BAND_24_12     0x01    //外部晶体振荡器频率在24~12MHz
#define XOSC_BAND_12_LOW    0x10    //外部晶体振荡器频率在12MHz以下

/* PWMx时钟源 */
#define PWMA_SOURCE_FREQ    72000000    //(Hz) Go to definition ->  void Clock_Init(void)

typedef struct
{
    u8 Main_clock_source;
    u8 IRC_en;
    u8 XOSC_en;
    u8 XOSC_type;
    u8 XOSC_gain_en;
    u8 XOSC_band;
    u8 X32KOSC_en;
    u8 X32KOSC_gain_en;
    u8 IRC32K_en;
    u8 Main_clock_div;
    u8 PLL_select;
    u8 HSIO_select;
    u8 HSIO_div;
    u8 TFPU_div;
    u8 PLL_en;
    u8 PLL_source_div;
    u8 PWMA_div;
    u8 PWMB_div;
}Clock_Struct;

//========================================================================
//                              外部声明
//========================================================================

void HSPllClkConfig(u8 clksrc, u8 pllsel, u8 div);

#endif
