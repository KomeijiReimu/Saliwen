#pragma once

#include <rtthread.h>

#include <stdio.h>
#include <stdint.h>
#include <rtdevice.h>
#include <board.h>
#include "mqttclient.h"

#include "cJSON.h"

#include <rtdbg.h>
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

#ifndef KAWAII_MQTT_SUBTOPIC
#define KAWAII_MQTT_SUBTOPIC           "rtt-sub"
#endif
#ifndef KAWAII_MQTT_PUBTOPIC
#define KAWAII_MQTT_PUBTOPIC           "rtt-pub"
#endif

/*
 * 创建一个以下格式的json数据包
{
     "group_name":   "group_name",
     "sensor":   {
         "light_intensity":     "data1",
         "temperature":         "data2",
         "humidity":            "data3"
     }
}
*/
 void  mqtt_date_pub_json(rt_uint16_t date1 ,rt_uint16_t date2 ,rt_uint16_t date3)
 {
        char *str=NULL;
        /* 创建一个JSON数据对象(链表头结点) */
        cJSON *cupload = cJSON_CreateObject();
        /* 创建int类型数据 */
        cJSON *cdate1 = cJSON_CreateNumber(date1);
        cJSON *cdate2 = cJSON_CreateNumber(date2);
        cJSON *cdate3 = cJSON_CreateNumber(date3);
        /* 创建一个JSON数据对象(链表结点) //创建JSON对象，｛｝扩起来的*/
        cJSON *csensor = cJSON_CreateObject();
        /* 创建 "date": cdate键值对，加入到 cparams对象中*/
        cJSON_AddItemToObject(csensor, "light_intensity", cdate1);
        cJSON_AddItemToObject(csensor, "temperature", cdate2);
        cJSON_AddItemToObject(csensor, "humidity", cdate3);
        /* 创建字符串类型数据 */
        cJSON *cgroup_name = cJSON_CreateString("embedded_master");
        /* 加入到JSON数据对象cupload */
        cJSON_AddItemToObject(cupload, "group_name", cgroup_name);
        cJSON_AddItemToObject(cupload, "sensor", csensor);
        /* 转换为标准JSON数据格式 */
        str=cJSON_Print(cupload);
        rt_kprintf("cjson:%s",str);
        /* 释放str内存 */
        cJSON_free(str);
        /* 删除json对象，释放内存 */
        cJSON_Delete(cupload);
 }


