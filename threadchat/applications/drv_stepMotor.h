#ifndef __DRV_STEPMOTOR_H_
#define __DRV_STEPMOTOR_H_
#include "drivers/rt_drv_pwm.h"
#include <rtdevice.h>
#ifdef RT_USING_PWM
#define STEPMOTOR_EN_PIN_NUM                41                  // 步进电机EN
#define STEPMOTOR_DIR_PIN_NUM               68                  // 步进电机DIR
#define STEPMOTOR_DRV_NAME                  "pwm4"              // 步进电机设备名称
#define STEPMOTOR_DRV_CHANNEL               1                   // 步进电机设备通道

int drv_stepMotor_init(void);
void drv_stepMotor_ctrl(int dir);


#endif //#ifdef RT_USING_PWM
#endif // __DRV_STEPMOTOR_H_

