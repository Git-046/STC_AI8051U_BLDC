#include "Context.h"
#include "AI8051U.h"
#include "Foc.h"
#include "Motor.h"
#include "math.h"
#include "Protect.h"
#include "def.h"

/*==================== Sin/Cos LUT (Q15) ====================*/
/* Auto-generated, 256 entries, Q15 format (32767 = 1.0)
 * Index = electrical_angle * 40.743665 (256 / (2*PI))
 */
const int16 SIN_TABLE_Q15[256] = {
         0,    804,   1608,   2410,   3212,   4011,   4808,   5602,
      6393,   7179,   7962,   8739,   9512,  10278,  11039,  11793,
     12539,  13279,  14010,  14732,  15446,  16151,  16846,  17530,
     18204,  18868,  19519,  20159,  20787,  21403,  22005,  22594,
     23170,  23731,  24279,  24811,  25329,  25832,  26319,  26790,
     27245,  27683,  28105,  28510,  28898,  29268,  29621,  29956,
     30273,  30571,  30852,  31113,  31356,  31580,  31785,  31971,
     32137,  32285,  32412,  32521,  32609,  32678,  32728,  32757,
     32767,  32757,  32728,  32678,  32609,  32521,  32412,  32285,
     32137,  31971,  31785,  31580,  31356,  31113,  30852,  30571,
     30273,  29956,  29621,  29268,  28898,  28510,  28105,  27683,
     27245,  26790,  26319,  25832,  25329,  24811,  24279,  23731,
     23170,  22594,  22005,  21403,  20787,  20159,  19519,  18868,
     18204,  17530,  16846,  16151,  15446,  14732,  14010,  13279,
     12539,  11793,  11039,  10278,   9512,   8739,   7962,   7179,
      6393,   5602,   4808,   4011,   3212,   2410,   1608,    804,
         0,   -803,  -1607,  -2409,  -3211,  -4010,  -4807,  -5601,
     -6392,  -7178,  -7961,  -8738,  -9511, -10277, -11038, -11792,
    -12538, -13278, -14009, -14731, -15445, -16150, -16845, -17529,
    -18203, -18867, -19518, -20158, -20786, -21402, -22004, -22593,
    -23169, -23730, -24278, -24810, -25328, -25831, -26318, -26789,
    -27244, -27682, -28104, -28509, -28897, -29267, -29620, -29955,
    -30272, -30570, -30851, -31112, -31355, -31579, -31784, -31970,
    -32136, -32284, -32411, -32520, -32608, -32677, -32727, -32756,
    -32766, -32756, -32727, -32677, -32608, -32520, -32411, -32284,
    -32136, -31970, -31784, -31579, -31355, -31112, -30851, -30570,
    -30272, -29955, -29620, -29267, -28897, -28509, -28104, -27682,
    -27244, -26789, -26318, -25831, -25328, -24810, -24278, -23730,
    -23169, -22593, -22004, -21402, -20786, -20158, -19518, -18867,
    -18203, -17529, -16845, -16150, -15445, -14731, -14009, -13278,
    -12538, -11792, -11038, -10277,  -9511,  -8738,  -7961,  -7178,
     -6392,  -5601,  -4807,  -4010,  -3211,  -2409,  -1607,   -803
};

#define TWO_PI           6.283185307f
#define INDEX_SCALE      40.743665f       /* 256 / (2*PI) */
#define Q15_TO_FLOAT     0.000030517578f  /* 1.0f / 32768.0f */

static int16 sin_q15(float angle_rad)
{
    uint16 idx;
    if (angle_rad < 0.0f)        angle_rad += TWO_PI;
    if (angle_rad >= TWO_PI)     angle_rad -= TWO_PI;
    idx = (uint16)(angle_rad * INDEX_SCALE);
    return SIN_TABLE_Q15[idx & 0xFF];
}

static int16 cos_q15(float angle_rad)
{
    return sin_q15(angle_rad + 1.570796327f);  /* cos(¦È) = sin(¦È + ¦Ð/2) */
}

#define SIN_F(angle)  ((float)sin_q15(angle) * Q15_TO_FLOAT)
#define COS_F(angle)  ((float)cos_q15(angle) * Q15_TO_FLOAT)


void Current_Loop(void)
{
    //1.define local variables
    float ModulationRatio;
    float angle, sin_f, cos_f;

    //2. current sampling
    //2.1. read ADC
    g_ctx->phase_current.current_u = ADC_Result_Buffer[0];
    g_ctx->phase_current.current_v = ADC_Result_Buffer[1];
    g_ctx->phase_current.current_w = ADC_Result_Buffer[2];
    //2.2. low-pass filter
    First_Order_LPF(&g_ctx->phase_current, 0.1);

    //3. Clarke transform (consistent with SVPWM inverse Clarke: Vu=Vbeta)
    g_ctx->alpha_beta_current.current_alpha = (g_ctx->phase_current.current_v - g_ctx->phase_current.current_w) * ONE_DIVIDE_THE_SQUARE_OF_THREE;
    g_ctx->alpha_beta_current.current_beta  = g_ctx->phase_current.current_u;

    //4. Park transform (sin/cos from LUT, single lookup)
    angle = g_ctx->motor_data.electrical_angle;
    sin_f = SIN_F(angle);
    cos_f = COS_F(angle);
    g_ctx->dq_current.current_d = g_ctx->alpha_beta_current.current_alpha * cos_f + g_ctx->alpha_beta_current.current_beta * sin_f;
    g_ctx->dq_current.current_q = (-g_ctx->alpha_beta_current.current_alpha) * sin_f + g_ctx->alpha_beta_current.current_beta * cos_f;

    //5. current PID
    g_ctx->pid_d.error = g_ctx->dq_current_ref.current_d - g_ctx->dq_current.current_d;
    PID(&g_ctx->pid_d);
    g_ctx->pid_q.error = g_ctx->dq_current_ref.current_q - g_ctx->dq_current.current_q;
    PID(&g_ctx->pid_q);

    //6. PID output -> target voltage
    g_ctx->dq_voltage_ctl.voltage_d = g_ctx->pid_d.output;
    g_ctx->dq_voltage_ctl.voltage_q = g_ctx->pid_q.output;

    /* current limit */
    if(CURRENT_LIMIT_EN == ENABLE)
    {
        Current_Limit(&g_ctx->dq_voltage_ctl.voltage_q);
    }

    //7. voltage limit circle
    //  g_ctx->v_bus = ADC_Result_Buffer[3] * ADC_V_REF * V_BUS_VOLTAGE_DIVISION_RATIO / 4095;
        g_ctx->v_bus = ADC_Result_Buffer[3] * ADC_V_REF * V_BUS_VOLTAGE_DIVISION_RATIO * 0.000244140625;
    ModulationRatio = ONE_DIVIDE_THE_SQUARE_OF_THREE_PLUS_ZERO_POINT_NINE * g_ctx->v_bus / sqrt(SQUARE(g_ctx->dq_voltage_ctl.voltage_d) + SQUARE(g_ctx->dq_voltage_ctl.voltage_q));
    if(ModulationRatio < 1.0)
    {
        /* integrator freeze */
        g_ctx->pid_d.I_EN_FLAG = 0;
        g_ctx->pid_q.I_EN_FLAG = 0;
        /* voltage limit */
        g_ctx->dq_voltage_ctl_limit.voltage_d = g_ctx->dq_voltage_ctl.voltage_d * ModulationRatio;
        g_ctx->dq_voltage_ctl_limit.voltage_q = g_ctx->dq_voltage_ctl.voltage_q * ModulationRatio;
    }
    else
    {
        /* integrator enable */
        g_ctx->pid_d.I_EN_FLAG = 1;
        g_ctx->pid_q.I_EN_FLAG = 1;

        g_ctx->dq_voltage_ctl_limit.voltage_d = g_ctx->dq_voltage_ctl.voltage_d;
        g_ctx->dq_voltage_ctl_limit.voltage_q = g_ctx->dq_voltage_ctl.voltage_q;
    }

    //8. inverse Park (reuse sin_f/cos_f from Park)
    g_ctx->alpha_beta_voltage_ctl.voltage_alpha = g_ctx->dq_voltage_ctl_limit.voltage_d * cos_f - g_ctx->dq_voltage_ctl_limit.voltage_q * sin_f;
    g_ctx->alpha_beta_voltage_ctl.voltage_beta  = g_ctx->dq_voltage_ctl_limit.voltage_d * sin_f + g_ctx->dq_voltage_ctl_limit.voltage_q * cos_f;

    //9. SVPWM -> PWM duty
    SVPWM(&g_ctx->alpha_beta_voltage_ctl, g_ctx->v_bus, 0.00005);
}

void SVPWM(Alpha_Beta_Axis_Voltage xdata *alpha_beta_voltage, float v_bus, float T_pwm)
{
    Phase_Voltage xdata voltage_abc;
    u8 xdata n;
    u8 xdata a, b, c;
    int16 xdata x, y, z;
    uint16 xdata timeA, timeB, timeC;

    //1. inverse Clarke (TI convention: Vu = Vbeta, gives uniform 60deg sectors)
    voltage_abc.voltage_u = alpha_beta_voltage->voltage_beta;
    voltage_abc.voltage_v = SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha - 0.5 * alpha_beta_voltage->voltage_beta;
    voltage_abc.voltage_w = -SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha - 0.5 * alpha_beta_voltage->voltage_beta;

    //2. sector determination
    a = (voltage_abc.voltage_u > 0) ? 1 : 0;
    b = (voltage_abc.voltage_v > 0) ? 1 : 0;
    c = (voltage_abc.voltage_w > 0) ? 1 : 0;
    n = (c << 2) | (b << 1) | a;

    //3. intermediate time variables
    x = (int16)(SQUARE_ROOT_3 * alpha_beta_voltage->voltage_beta / v_bus * T_pwm);
    y = (int16)(SQUARE_ROOT_3 * (SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha + 0.5 * alpha_beta_voltage->voltage_beta) / v_bus * T_pwm);
    z = (int16)(SQUARE_ROOT_3 * (-SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha + 0.5 * alpha_beta_voltage->voltage_beta) / v_bus * T_pwm);

    //4. sector time calculation (TI convention)
    switch(n)
    {
        case 3:  // Vu>0, Vv>0, Vw<0
                timeA = (uint16)((T_pwm + z - x) / 4);
                timeB = (uint16)((T_pwm - z - x) / 4);
                timeC = (uint16)((T_pwm - z + x) / 4);
                break;
        case 1:  // Vu>0, Vv<0, Vw<0
                timeA = (uint16)((T_pwm - z - y) / 4);
                timeB = (uint16)((T_pwm + z - y) / 4);
                timeC = (uint16)((T_pwm + z + y) / 4);
                break;
        case 5:  // Vu>0, Vv<0, Vw>0
                timeA = (uint16)((T_pwm - x + y) / 4);
                timeB = (uint16)((T_pwm + x + y) / 4);
                timeC = (uint16)((T_pwm + x - y) / 4);
                break;
        case 4:  // Vu<0, Vv<0, Vw>0
                timeA = (uint16)((T_pwm + x - z) / 4);
                timeB = (uint16)((T_pwm - x - z) / 4);
                timeC = (uint16)((T_pwm - x + z) / 4);
                break;
        case 6:  // Vu<0, Vv>0, Vw>0
                timeA = (uint16)((T_pwm + y + z) / 4);
                timeB = (uint16)((T_pwm - y + z) / 4);
                timeC = (uint16)((T_pwm - y - z) / 4);
                break;
        case 2:  // Vu<0, Vv>0, Vw<0
                timeA = (uint16)((T_pwm - y + x) / 4);
                timeB = (uint16)((T_pwm + y + x) / 4);
                timeC = (uint16)((T_pwm + y - x) / 4);
                break;
        default:
                timeA = (uint16)T_pwm >> 2;
                timeB = (uint16)T_pwm >> 2;
                timeC = (uint16)T_pwm >> 2;
                break;
    }

    //5. set PWM compare registers
    PWMA_CCR1H = (u8)(timeA >> 8);
    PWMA_CCR1L = (u8)(timeA & 0xFF);
    PWMA_CCR2H = (u8)(timeB >> 8);
    PWMA_CCR2L = (u8)(timeB & 0xFF);
    PWMA_CCR3H = (u8)(timeC >> 8);
    PWMA_CCR3L = (u8)(timeC & 0xFF);
}

void First_Order_LPF(Phase_Current xdata *phase_current, float alpha)
{
    phase_current->current_u_filtered = alpha * phase_current->current_u + (1 - alpha) * phase_current->current_u_filtered;
    phase_current->current_v_filtered = alpha * phase_current->current_v + (1 - alpha) * phase_current->current_v_filtered;
    phase_current->current_w_filtered = alpha * phase_current->current_w + (1 - alpha) * phase_current->current_w_filtered;
}

void PID(PID_Structure xdata *pid_structure)
{
    //proportional
    pid_structure->P_out = pid_structure->error * pid_structure->P_gain;

    //integral
    if(pid_structure->I_EN_FLAG == 1)
    {
        pid_structure->I_sum += pid_structure->error * pid_structure->I_period;
        if(pid_structure->I_sum > pid_structure->I_limit)
        {
            pid_structure->I_sum = pid_structure->I_limit;
        }
        else if(pid_structure->I_sum < -pid_structure->I_limit)
        {
            pid_structure->I_sum = -pid_structure->I_limit;
        }
        pid_structure->I_out = pid_structure->I_sum * pid_structure->I_gain;
    }
    //derivative
    pid_structure->D_out = (pid_structure->error - pid_structure->pre_error) * pid_structure->D_gain;
    pid_structure->pre_error = pid_structure->error;

    //output
    pid_structure->output_ref = pid_structure->P_out + pid_structure->I_out +pid_structure->D_out;
    if(pid_structure->output_ref > pid_structure->output_limit)
    {
        pid_structure->output = pid_structure->output_limit;
    }
    else if(pid_structure->output_ref < -pid_structure->output_limit)
    {
        pid_structure->output = -pid_structure->output_limit;
    }
    else
    {
        pid_structure->output = pid_structure->output_ref;
    }

    //anti-windup (back-calculation)
    pid_structure->I_sum -= pid_structure->Kc_gain * (pid_structure->output_ref - pid_structure->output);
}

void Motor_Speed_Ramp(Motor_Speed xdata *speed)
{
    if(speed->motor_speed > 0)
    {
        speed->set_speed = speed->motor_speed;
        if(speed->target_speed > speed->motor_speed)
        {
            if(speed->target_speed - speed->motor_speed > speed->acceleration_speed)
            {
                speed->set_speed += speed->acceleration_speed;
            }
            else
            {
                speed->set_speed = speed->target_speed;
            }
        }
        else
        {
            if(speed->motor_speed - speed->target_speed > speed->deceleration_speed)
            {
                speed->set_speed -= speed->deceleration_speed;
            }
            else
            {
                speed->set_speed = speed->target_speed;
            }
        }
    }
    else
    {
        speed->set_speed = speed->motor_speed;
        if(speed->target_speed < speed->motor_speed)
        {
            if(speed->target_speed - speed->motor_speed < -speed->acceleration_speed)
            {
                speed->set_speed -= speed->acceleration_speed;
            }
            else
            {
                speed->set_speed = speed->target_speed;
            }
        }
        else
        {
            if(speed->motor_speed - speed->target_speed < -speed->deceleration_speed)
            {
                speed->set_speed += speed->deceleration_speed;
            }
            else
            {
                speed->set_speed = speed->target_speed;
            }
        }
    }
}

void Speed_Update(Motor_Speed xdata *speed)
{
    speed->motor_speed = g_ctx->motor_data.speed;
    speed->target_speed = SPEED_MAX * speed->pwm_duty / PWM_DUTY_BASE;
}

void Speed_Loop(void)
{
    g_ctx->pid_speed.error = g_ctx->motor_speed.target_speed - g_ctx->motor_speed.motor_speed;
    PID(&g_ctx->pid_speed);
    g_ctx->dq_current_ref.current_q = g_ctx->pid_speed.output;
}
