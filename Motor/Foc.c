#include "Context.h"
#include "AI8051U.h"
#include "Foc.h"
#include "Motor.h"
#include "math.h"
#include "Protect.h"
#include "def.h"


void Current_Loop(void)
{
    //1.
    float ModulationRatio;

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

    //4. Park transform
    g_ctx->dq_current.current_d = g_ctx->alpha_beta_current.current_alpha * cos(g_ctx->motor_data.electrical_angle) + g_ctx->alpha_beta_current.current_beta * sin(g_ctx->motor_data.electrical_angle);
    g_ctx->dq_current.current_q = (-g_ctx->alpha_beta_current.current_alpha) * sin(g_ctx->motor_data.electrical_angle) + g_ctx->alpha_beta_current.current_beta * cos(g_ctx->motor_data.electrical_angle);

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
    g_ctx->v_bus = ADC_Result_Buffer[3] * ADC_V_REF * V_BUS_VOLTAGE_DIVISION_RATIO / 4095 ;
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

    //8. inverse Park
    g_ctx->alpha_beta_voltage_ctl.voltage_alpha = g_ctx->dq_voltage_ctl_limit.voltage_d * cos(g_ctx->motor_data.electrical_angle) - g_ctx->dq_voltage_ctl_limit.voltage_q * sin(g_ctx->motor_data.electrical_angle);
    g_ctx->alpha_beta_voltage_ctl.voltage_beta  = g_ctx->dq_voltage_ctl_limit.voltage_d * sin(g_ctx->motor_data.electrical_angle) + g_ctx->dq_voltage_ctl_limit.voltage_q * cos(g_ctx->motor_data.electrical_angle);

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
