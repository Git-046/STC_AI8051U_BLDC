#include "AI8051U.h"
#include "Init.h"
#include "Context.h"

System_Context xdata *g_ctx;

void main(void)
{
	System_Context xdata ctx = {0};
	g_ctx = &ctx;

	System_Init();
	g_ctx->motor_state = motor_init;

	while(1)
	{

	}
}
