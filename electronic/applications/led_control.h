/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-22     wq0453       the first version
 */
#include "drv_stepMotor.h"
#ifndef APPLICATIONS_LED_CONTROL_H_
#define APPLICATIONS_LED_CONTROL_H_


void loop_led(void);
void init_io_port(void);
void control_led(unsigned char cmd);
void loop_led(void);


#endif /* APPLICATIONS_LED_CONTROL_H_ */

