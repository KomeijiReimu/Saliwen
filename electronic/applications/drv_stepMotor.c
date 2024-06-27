/*********************************************************************************************
* 文件：drv_stepMotor.c
* 作者：Zhouchj 2020.11.05
* 描述：步进电机驱动源文件
* 修改：
* 注释：
*********************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <rtdevice.h>
#include <board.h>

#include "mqttclient.h"

#include "cJSON.h"

#include <rtdbg.h>
#include "drv_stepMotor.h"

#ifdef RT_USING_PWM
static struct rt_device_pwm *stepMotorDrv = RT_NULL; 

/*********************************************************************************************
* 名称：drv_stepMotor_ctrl()
* 功能：步进电机控制
* 参数：dir -> 0-停止 1-正转 2-反转
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void drv_stepMotor_ctrl(int dir)
{
  if(dir == 0)
  {
    rt_kprintf("STOP\n");
    rt_pwm_disable(stepMotorDrv, STEPMOTOR_DRV_CHANNEL);        // 失能PWM
    rt_pin_write(STEPMOTOR_EN_PIN_NUM, 1);                      // 步进电机EN设置为高电平，失能
  }
  else
  {
    if(dir == 1) {
      rt_pin_write(STEPMOTOR_DIR_PIN_NUM, 0);                   // 步进电机DIR设置为低电平
      rt_kprintf("GO1\n");
    }
    else {
      rt_pin_write(STEPMOTOR_DIR_PIN_NUM, 1);                   // 步进电机DIR设置为高电平
      rt_kprintf("GO2\n");
    }
    rt_pin_write(STEPMOTOR_EN_PIN_NUM, 0);                      // 步进电机EN设置为低电平，使能
    rt_pwm_enable(stepMotorDrv, STEPMOTOR_DRV_CHANNEL);         // 使能PWM
  }
}

/*********************************************************************************************
* 名称：drv_stepMotor_init()
* 功能：步进电机初始化
* 参数：无
* 返回：步进电机初始化是否成功
* 修改：
* 注释：
*********************************************************************************************/
int drv_stepMotor_init(void)
{
  stepMotorDrv = (struct rt_device_pwm *)rt_device_find(STEPMOTOR_DRV_NAME);// 查找设备
  if(stepMotorDrv == RT_NULL)
  {
    rt_kprintf("failed to find %s device!\n", STEPMOTOR_DRV_NAME);
    return -1;
  }
  rt_pin_mode(STEPMOTOR_EN_PIN_NUM, PIN_MODE_OUTPUT);           // 初始化步进电机EN引脚为输出
  rt_pin_mode(STEPMOTOR_DIR_PIN_NUM, PIN_MODE_OUTPUT);          // 初始化步进电机DIR引脚为输出
  rt_pin_write(STEPMOTOR_EN_PIN_NUM, 1);                        // 设置步进电机EN引脚高电平
  rt_pwm_set(stepMotorDrv, STEPMOTOR_DRV_CHANNEL, 1000000, 500000);// 设置步进电机STEP引脚PWM输出：频率1kHz，占空比50%
  return 0;
}
#endif
