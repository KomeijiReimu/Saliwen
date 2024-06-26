
#include <rtthread.h>

#include <stdio.h>
#include <stdint.h>
#include <rtdevice.h>
#include <board.h>
#include "mqttclient.h"

#include "led_control.h"
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
 * rtt发送json数据包的格式
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

 /*
  * rtt接收json数据包的格式
 {
      "group_name":   "group_name",
      "CMD":   {
          "led":     cmd1,
          "beep":     "cmd2"
      }
 }
 */
static void sub_topic_handle1(void* client, message_data_t* msg)
{
    (void) client;
    cJSON *cjson_data, *ccmd, *led;

    cjson_data = cJSON_Parse((char*)msg->message->payload);//将json字符串转化成CJSON结构体类型的数据

    if(!cjson_data)
     {
         //转换错误则退出解析
         rt_kprintf("json phase error%s\r\n",cjson_data);
         cJSON_Delete(cjson_data);
         return;
     }

     /*将"CMD"成员 放入ccmd*/
     ccmd = cJSON_GetObjectItem(cjson_data,"CMD");

     led = cJSON_GetObjectItem(ccmd, "led");
     if (led != NULL)
     {
         control_led(led->valueint);
         rt_kprintf("led: %d\n", led->valueint);
     }


         /* 删除json对象，释放内存 */

     cJSON_Delete(cjson_data);


//
//    KAWAII_MQTT_LOG_I("-----------------------------------------------------------------------------------");
//    KAWAII_MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
//    KAWAII_MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

static int mqtt_publish_handle1(mqtt_client_t *client)
{
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

    msg.qos = QOS0;
    msg.payload = (void *)"this is a kawaii mqtt test ...";

    return mqtt_publish(client, KAWAII_MQTT_PUBTOPIC, &msg);
}

static int mqtt_publish_json_handle1(mqtt_client_t *client)
{
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

    char *str=NULL;
    /* 创建一个JSON数据对象(链表头结点) */
    cJSON *cupload = cJSON_CreateObject();
    /* 创建int类型数据 */
    cJSON *cdate1 = cJSON_CreateNumber(12);
    cJSON *cdate2 = cJSON_CreateNumber(34);
    cJSON *cdate3 = cJSON_CreateNumber(56);
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


    msg.qos = QOS0;
    msg.payload = (void *)str;

    rt_kprintf("cjson:%s\n",str);

    /* 释放str内存 */
    cJSON_free(str);
    /* 删除json对象，释放内存 */
    cJSON_Delete(cupload);

    return mqtt_publish(client, KAWAII_MQTT_PUBTOPIC, &msg);
}

static void kawaii_mqtt_demo(void *parameter)
{
    int rc;
    mqtt_client_t *client = NULL;

    rt_thread_delay(8000);
    rt_kprintf("start mqtt\n");
    mqtt_log_init();

    client = mqtt_lease();

    mqtt_set_host(client, "broker.emqx.io");
    mqtt_set_port(client, "1883");
    mqtt_set_user_name(client, "group11");
    mqtt_set_password(client, "group11");
    mqtt_set_client_id(client, "rtthread001");
    mqtt_set_clean_session(client, 1);

    KAWAII_MQTT_LOG_I("The ID of the Kawaii client is: %s ", "rtthread001");

    rc = mqtt_connect(client);
    KAWAII_MQTT_LOG_I("mqtt_connect return : %d ", rc);


    mqtt_subscribe(client, KAWAII_MQTT_SUBTOPIC, QOS0, sub_topic_handle1);

    while (1) {
//        mqtt_publish_handle1(client);
        mqtt_publish_json_handle1(client);

        mqtt_sleep_ms(5 * 1000);
    }
}

int ka_mqtt(void)
{
    rt_thread_t tid_mqtt;

    tid_mqtt = rt_thread_create("kawaii_demo", kawaii_mqtt_demo, RT_NULL, 2048, 17, 10);
    if (tid_mqtt == RT_NULL) {
        return -RT_ERROR;
    }

    rt_thread_startup(tid_mqtt);

    return RT_EOK;
}

int main(void)
{
    int count = 1;
    init_io_port();
    ka_mqtt();
    while (count++)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
