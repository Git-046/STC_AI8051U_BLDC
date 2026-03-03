#ifndef __PROTECT_H
#define __PROTECT_H

/*========================================保护功能======================================*/
#define CURRENT_LIMIT_EN                        1           //电流限流功能使能位
#define CURRENT_LIMIT_VALUE                     2.0         //(A) 电流限流值

#define MOTOR_STALL_PROTECT_EN                  1           //堵转功能使能位
#define MOTOR_STALL_DETECT_TIME                 2000        //(ms) 堵转判定时间阈值
#define MOTOR_STALL_DETECT_SPEED                40          //(RPM) 堵转判定速度阈值

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
#define LOVER_VOLTAGE_DETECT_TIME               100         //(ms) 低压检测时间阈值
#define LOVER_VOLTAGE_RECOVER_DETECT_TIME       100         //(ms) 低压恢复检测时间阈值

void Current_Limit(float xdata *Q_axis_current);
void Over_Current_Protect(void);
void Motor_Stall_Protect(void);

#endif