/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-22     wq0453       the first version
 */
#include <rtdevice.h>
#include "led_control.h"


#include <stdio.h>
#include <stdint.h>
#include <rtdevice.h>
#include <board.h>
#include "mqttclient.h"
#include "drv_stepMotor.h"
#include "cJSON.h"

#include <rtdbg.h>

rt_base_t LED1_PIN_NUM;
rt_base_t LED2_PIN_NUM;
rt_base_t LED3_PIN_NUM;
rt_base_t LED4_PIN_NUM;


void init_io_port(void)
{

    //初始化为输出模式
    LED1_PIN_NUM = rt_pin_get("PE.0");
    LED2_PIN_NUM = rt_pin_get("PE.1");
    LED3_PIN_NUM = rt_pin_get("PE.2");
    LED4_PIN_NUM = rt_pin_get("PE.3");
    rt_pin_mode(LED1_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LED4_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_write(LED1_PIN_NUM, PIN_HIGH);
    rt_pin_write(LED2_PIN_NUM, PIN_HIGH);
    rt_pin_write(LED3_PIN_NUM, PIN_HIGH);
    rt_pin_write(LED4_PIN_NUM, PIN_HIGH);
}

void control_led(unsigned char cmd)
{
    //根据cmd变量的低四位，控制led输出
    //bit:0 为1， led1 点亮
    if (cmd & 0x01)
        rt_pin_write(LED1_PIN_NUM, PIN_LOW);
    else
        rt_pin_write(LED1_PIN_NUM, PIN_HIGH);

    if (cmd & (0x01<<1))
        rt_pin_write(LED2_PIN_NUM, PIN_LOW);
    else
        rt_pin_write(LED2_PIN_NUM, PIN_HIGH);

    if (cmd & (0x01<<2))
        rt_pin_write(LED3_PIN_NUM, PIN_LOW);
    else
        rt_pin_write(LED3_PIN_NUM, PIN_HIGH);

    if (cmd & (0x01<<3))
        rt_pin_write(LED4_PIN_NUM, PIN_LOW);
    else
        rt_pin_write(LED4_PIN_NUM, PIN_HIGH);

}

void turn(int a) {
    if(a == 1) {
        rt_pin_write(LED1_PIN_NUM, PIN_LOW);
        rt_pin_write(LED2_PIN_NUM, PIN_LOW);
        rt_pin_write(LED3_PIN_NUM, PIN_LOW);
        rt_pin_write(LED4_PIN_NUM, PIN_LOW);
        drv_stepMotor_ctrl(1);

    }
    else {
        rt_pin_write(LED1_PIN_NUM, PIN_HIGH);
        rt_pin_write(LED2_PIN_NUM, PIN_HIGH);
        rt_pin_write(LED3_PIN_NUM, PIN_HIGH);
        rt_pin_write(LED4_PIN_NUM, PIN_HIGH);
        drv_stepMotor_ctrl(0);
    }
}
void loop_led(void)
{
    static unsigned char var = 0x01;

    control_led(var);

    var = (var<<1);
    if(var == 0x10)
        var = 0x01;

    rt_thread_delay(500);
}
