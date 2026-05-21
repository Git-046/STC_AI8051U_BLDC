#ifndef _INIT_H_
#define _INIT_H_
#include "AI8051U.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"
#include "stdarg.h"		//NULL
#include "math.h"
#include "Clock.h"
#include "GPIO.h"
#include "PWM.h"
#include "ADC.h"
#include "DMA.h"
#include "SPI.h"
#include "config.h"
#include "Motor.h"
#include "Foc.h"
#include "State_Machine.h"
#include "Protect.h"

void System_Init(void);
void Parameter_Init(void);
#endif
