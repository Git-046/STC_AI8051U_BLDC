#include "AI8051U.h"
#include "Foc.h"
#include "Motor.h"		//电机参数
#include "math.h"
#include "Protect.h"
#include "def.h"

float xdata V_bus;
PID_Structure xdata pid_structure_d;
PID_Structure xdata pid_structure_q;
PID_Structure xdata pid_structure_speed;

Speed_Ramp xdata speed_ramp;

void Current_Loop(void)
{
    //1.
    float ModulationRatio;  //调制比
    
	//2. 运行步骤
	//2.1.更新采样电流
	phase_current.current_u = ADC_Result_Buffer[0];
	phase_current.current_v = ADC_Result_Buffer[1];
	phase_current.current_w = ADC_Result_Buffer[2];
	//2.2.滤波
	First_Order_LPF(phase_current.current_u, phase_current.current_u_filtered, phase_current.last_value, 0.1);
	First_Order_LPF(phase_current.current_v, phase_current.current_v_filtered, phase_current.last_value, 0.1);
	First_Order_LPF(phase_current.current_w, phase_current.current_w_filtered, phase_current.last_value, 0.1);
//	First_Order_LPF_Current(phase_current, 0.1);	//相电流更新、滤波
	
	//3.Clarke变换
	alpha_beta_current.current_alpha = phase_current.current_u;
	alpha_beta_current.current_beta = (phase_current.current_u + 2 * phase_current.current_w) / ONE_DIVIDE_THE_SQUARE_OF_THREE;
	
	//4.Park变换
	dq_current.current_d = alpha_beta_current.current_alpha * cos(motor_data.electrical_angle) + alpha_beta_current.current_beta * sin(motor_data.electrical_angle);
	dq_current.current_q = (-alpha_beta_current.current_alpha) * sin(motor_data.electrical_angle) + alpha_beta_current.current_beta * cos(motor_data.electrical_angle);
	
	//5.电流环PID
    pid_structure_d.error = dq_current_ref.current_d - dq_current.current_d;
	PID(&pid_structure_d);
    pid_structure_q.error = dq_current_ref.current_q - dq_current.current_q;
    PID(&pid_structure_q);
    
    //6.PID输出的控制量
    dq_voltage_ctl.voltage_d = pid_structure_d.output;
    dq_voltage_ctl.voltage_q = pid_structure_q.output;
    
    /* 电流限流 */
    if(CURRENT_LIMIT_EN == ENABLE)
    {
        Current_Limit(&dq_voltage_ctl.voltage_q);
    }
    
    //7.单位圆调制，电压限制
    V_bus = ADC_Result_Buffer[3] * ADC_V_REF * V_BUS_VOLTAGE_DIVISION_RATIO / 4095 ;
    ModulationRatio = ONE_DIVIDE_THE_SQUARE_OF_THREE_PLUS_ZERO_POINT_NINE * V_bus / sqrt(SQUARE(dq_voltage_ctl.voltage_d) + SQUARE(dq_voltage_ctl.voltage_q));
    if(ModulationRatio < 1.0)   //调制比小于1，电压过调制
    {
        /*积分锁定*/
        pid_structure_d.I_EN_FLAG = 0;
        pid_structure_q.I_EN_FLAG = 0;
        /*电压控制指令限幅*/
        dq_voltage_ctl_limit.voltage_d = dq_voltage_ctl.voltage_d * ModulationRatio;
        dq_voltage_ctl_limit.voltage_q = dq_voltage_ctl.voltage_q * ModulationRatio;
    }
    else
    {
        /*积分解锁*/
        pid_structure_d.I_EN_FLAG = 1;
        pid_structure_q.I_EN_FLAG = 1;
        
        dq_voltage_ctl_limit.voltage_d = dq_voltage_ctl.voltage_d;
        dq_voltage_ctl_limit.voltage_q = dq_voltage_ctl.voltage_q;
    }
    
    //8.反park变换
    alpha_beta_voltage_ctl.voltage_alpha = dq_voltage_ctl_limit.voltage_d * cos(motor_data.electrical_angle) - dq_voltage_ctl_limit.voltage_q * sin(motor_data.electrical_angle);
    alpha_beta_voltage_ctl.voltage_beta  = dq_voltage_ctl_limit.voltage_d * sin(motor_data.electrical_angle) + dq_voltage_ctl_limit.voltage_q * cos(motor_data.electrical_angle);
    
    //9.SVPWM输出占空比
    SVPWM(&alpha_beta_voltage_ctl, V_bus, 0.00005);
}

void SVPWM(Alpha_Beta_Axis_Voltage xdata *alpha_beta_voltage, float V_bus, float T_pwm)
{
    Phase_Voltage xdata voltage_abc;
    u8 xdata n;
    u8 xdata a, b, c;
    int16 xdata x, y, z;
    uint16 xdata timeA, timeB, timeC;
    
    //1.Clark逆变换
    voltage_abc.voltage_u = alpha_beta_voltage->voltage_beta;
    voltage_abc.voltage_v =SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha - 0.5 * alpha_beta_voltage->voltage_beta;
    voltage_abc.voltage_w = -SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha - 0.5 * alpha_beta_voltage->voltage_beta;
    
    //2.计算电压矢量所在的扇区
    a = (voltage_abc.voltage_u > 0) ? 1 : 0;
    b = (voltage_abc.voltage_v > 0) ? 1 : 0;
    c = (voltage_abc.voltage_w > 0) ? 1 : 0;
    n = c << 2 + b << 1 + c;
    
    //3.计算中间变量(各扇区的基本作用时间)
    x = (int16)(SQUARE_ROOT_3 * alpha_beta_voltage->voltage_beta / V_bus * (T_pwm));
    y = (int16)(SQUARE_ROOT_3 * (SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha + 0.5 * alpha_beta_voltage->voltage_beta) / V_bus * T_pwm);
    z = (int16)(SQUARE_ROOT_3 * (-SQUARE_ROOT_3_DIVIDE2 * alpha_beta_voltage->voltage_alpha + 0.5 * alpha_beta_voltage->voltage_beta) / V_bus * T_pwm);
    
    //
    switch(n)
    {
        case 3:
                timeA = (uint16)((T_pwm + z - x) / 4);
                timeB = (uint16)((T_pwm - z - x) / 4);
                timeC = (uint16)((T_pwm - z + x) / 4);
                break;
        case 1:
                timeA = (uint16)((T_pwm - z - y) / 4);
                timeB = (uint16)((T_pwm + z - y) / 4);
                timeC = (uint16)((T_pwm + z + y) / 4);
                break;
        case 5:
                timeA = (uint16)((T_pwm - x + y) / 4);
                timeB = (uint16)((T_pwm + x + y) / 4);
                timeC = (uint16)((T_pwm + x - y) / 4);
                break;
        case 4:
                timeA = (uint16)((T_pwm + x - z) / 4);
                timeB = (uint16)((T_pwm - x - z) / 4);
                timeC = (uint16)((T_pwm - x + z) / 4);
                break;
        case 6:
                timeA = (uint16)((T_pwm + y + z) / 4);
                timeB = (uint16)((T_pwm - y + z) / 4);
                timeC = (uint16)((T_pwm - y - z) / 4);
                break;
        case 2:
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
    
    PWMA_CCR1H = (u8)(timeA >> 8);
    PWMA_CCR1L = (u8)(timeA & 0xFF);
    PWMA_CCR2H = (u8)(timeB >> 8);
    PWMA_CCR2L = (u8)(timeB & 0xFF);
    PWMA_CCR3H = (u8)(timeC >> 8);
    PWMA_CCR3L = (u8)(timeC & 0xFF);
}

/*一阶低通滤波滤波函数*/
void First_Order_LPF(float new_value, float filtered_value, float last_value, float alpha)
{
	filtered_value = alpha * new_value + (1 - alpha) * last_value;
	last_value = filtered_value;
}

void PID(PID_Structure xdata *pid_structure)
{
	//比例环节
    pid_structure->P_out = pid_structure->error * pid_structure->P_gain;
    
    //积分环节
    if(pid_structure->I_EN_FLAG == 1)
    {
        pid_structure->I_sum += pid_structure->error * pid_structure->I_period;  //积分累加
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
    //微分环节
    pid_structure->D_out = (pid_structure->error - pid_structure->pre_error) * pid_structure->D_gain;
    pid_structure->pre_error = pid_structure->error;        //保存上一次误差
    
    //总输出
    pid_structure->output_ref = pid_structure->P_out + pid_structure->I_out +pid_structure->D_out;
    if(pid_structure->output_ref > pid_structure->output_limit)
    {
        pid_structure->output = pid_structure->output_limit;
    }
    else if(pid_structure->output < -pid_structure->output_limit)
    {
        pid_structure->output = -pid_structure->output_limit;
    }
    
    //积分抗饱和
    pid_structure->I_sum -= pid_structure->Kc_gain * (pid_structure->output_ref - pid_structure->output);
}

/*---------------------------------------------
    @ function  : 速度斜坡函数
    @ describe  : 处理输入速度指令
    @ param     : Motor_Speed 结构体指针
    @ date      : 2026-2-1
-----------------------------------------------*/
void Motor_Speed_Ramp(Motor_Speed xdata *speed)
{
    if(speed->motor_speed > 0)      //正转
    {
        speed->set_speed = speed->motor_speed;
        if(speed->target_speed > speed->motor_speed)   //加速
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
        else                                            //减速
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
    else    //反转
    {
        speed->set_speed = speed->motor_speed;
        if(speed->target_speed < speed->motor_speed)   //加速
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
        else                                            //减速
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

/*---------------------------------------------
    @ function  : 速度变量更新函数
    @ describe  : 处理输入速度指令
    @ param     : Motor_Speed 结构体指针
    @ date      : 2026-2-6
-----------------------------------------------*/
void Speed_Update(Motor_Speed xdata *speed)
{
    speed->motor_speed = motor_data.speed;
    speed->target_speed = SPEED_MAX * speed->pwm_duty / PWM_DUTY_BASE;      //目标转速 = 最大转速 * 占空比 
}

/*---------------------------------------------
    @ function   : Speed_Loop
    @ describe   : 速度环
    @ param      : None
    @ date       : 2026-2-24
----------------------------------------------*/
void Speed_Loop(void)
{
    pid_structure_speed.error = motor_speed.target_speed - motor_speed.motor_speed;
    PID(&pid_structure_speed);
    dq_voltage_ctl.voltage_q = pid_structure_speed.output;
}
