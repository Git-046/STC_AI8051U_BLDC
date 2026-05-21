#ifndef __PROTECT_H
#define __PROTECT_H

/*========================================保护功能======================================*/
#define CURRENT_LIMIT_EN                        1           //电流限流功能使能位
#define CURRENT_LIMIT_VALUE                     2.0         //(A) 电流限流值

#define MOTOR_STALL_PROTECT_EN                  1           //堵转功能使能位
#define MOTOR_STALL_DETECT_TIME                 2           //(ms) 堵转判定时间阈值
#define MOTOR_STALL_DETECT_SPEED                40          //(RPM) 堵转判定速度阈值
#define MOTOR_STALL_RESTART_EN                  1           //堵转重启功能使能位
#define MOTOR_STALL_RECOVER_TIME                3           //(s)  堵转后的重启间隔时间

#define OVER_CURRENT_PROTECT_EN                 1           //过流保护使能位
#define OVER_CURRENT_DETECT_TIME                10          //(ms) 过流检测时间阈值
#define OVER_CURRENT_PROTECT_VALUE              5           //(A) 过流保护阈值

#define OVER_VOLTAGE_PROTECT_EN                 1           //过压保护使能位
#define OVER_VOLTAGE_PROTECT_RECOVER_EN         1           //过压保护恢复使能位
#define LOWER_VOLTAGE_PROTECT_EN                1           //低压保护使能位
#define LOWER_VOLTAGE_PROTECT_RECOVER_EN        1           //低压保护恢复使能位
#define OVER_VOLTAGE_PROTECT_VALUE              28          //(V) 过压保护阈值
#define OVER_VOLTAGE_PROTECT_RECOVER_VALUE      27          //(V) 过压保护恢复阈值
#define LOWER_VOLTAGE_PROTECT_VALUE             20          //(V) 低压保护阈值
#define LOWER_VOLTAGE_PROTECT_RECOVER_VALUE     21          //(V) 低压保护恢复阈值
#define OVER_VOLTAGE_DETECT_TIME                100         //(ms) 过压检测时间阈值
#define OVER_VOLTAGE_RECOVER_DETECT_TIME        100         //(ms) 过压恢复检测时间阈值
#define LOWER_VOLTAGE_DETECT_TIME               100         //(ms) 低压检测时间阈值
#define LOWER_VOLTAGE_RECOVER_DETECT_TIME       100         //(ms) 低压恢复检测时间阈值

#define MOTOR_RESTART_EN        0           //重启功能(0 关闭  1 开启)
#define MOTOR_RESTART_DELAY     3000        //(ms) 故障重启延时时间
#define MOTOR_RESTART_TIMES     2           //重启次数(0 无限重启 1~65535 有限次数重启)

void Current_Limit(float xdata *Q_axis_current);
void Over_Current_Protect(void);
void Motor_Stall_Protect(void);

#endif