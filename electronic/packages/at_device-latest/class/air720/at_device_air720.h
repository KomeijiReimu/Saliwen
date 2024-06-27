/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-16     chenyong     first version
 */

#ifndef __AT_DEVICE_AIR720_H__
#define __AT_DEVICE_AIR720_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <at_device.h>

/* The maximum number of sockets supported by the air720 device */
#define AT_DEVICE_AIR720_SOCKETS_NUM      6

struct at_device_air720
{
    char *device_name;
    char *client_name;

    int power_pin;
    int power_status_pin;
    size_t recv_line_num;
    struct at_device device;

    void *user_data;
};

#ifdef AT_USING_SOCKET

/* air720 device socket initialize */
int air720_socket_init(struct at_device *device);

/* air720 device class socket register */
int air720_socket_class_register(struct at_device_class *class);

#endif /* AT_USING_SOCKET */

#ifdef __cplusplus
}
#endif

#endif /* __AT_DEVICE_AIR720_H__ */
