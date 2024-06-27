
#include <rtthread.h>
#include <rtdevice.h>
#include "cjson.h"
#include <stdio.h>
#include <stdint.h>
#include "led_control.h"
#include "htu21d_drv.h"
#include "drv_stepMotor.h"
#include "mqttclient.h"





#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

#include <rtdbg.h>

#define SENSOR_DEVICE_TEMPUTRA_NAME    "li_bh1750"    /* 传感器设备名称 */
#define SENSOR_DEVICE_WET_NAME    "HTU21D"    /* 传感器设备名称 */

#ifndef KAWAII_MQTT_SUBTOPIC
#define KAWAII_MQTT_SUBTOPIC           "Marisa"
#endif
#ifndef KAWAII_MQTT_PUBTOPIC
#define KAWAII_MQTT_PUBTOPIC           "pub_crychic"
#endif
#define KAWAII_MQTT_CLIENTID           "crychic"
/* 邮箱控制块 */
static rt_mailbox_t test_mb = RT_NULL;
static int flag = 0;

static void sub_topic_handle1(void *client, message_data_t *msg) {
    (void) client;
    KAWAII_MQTT_LOG_I("-----------------------------------------------------------------------------------");
    KAWAII_MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name,
                      (char *) msg->message->payload);
    KAWAII_MQTT_LOG_I("-----------------------------------------------------------------------------------");

    /* 解析接收到的JSON消息 */
    cJSON *json = cJSON_Parse(msg->message->payload);
    if (json == NULL) {
        KAWAII_MQTT_LOG_E("Failed to parse JSON message");
        return;
    }

    /* 检查是否包含 "msg": "start" */
    cJSON *msg_item = cJSON_GetObjectItem(json, "msg");
    if (cJSON_IsString(msg_item) && (strcmp(msg_item->valuestring, "start") == 0)) {
        rt_kprintf("yes\n");
        if(flag) {
            flag = 0;
            turn(0);
        }
        else {
            flag = 1;
            turn(1);
        }
    }

    /* 释放JSON对象 */
    cJSON_Delete(json);
}

static int mqtt_publish_handle2(mqtt_client_t *client, const char *message) {
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

    msg.qos = QOS0;
    msg.payload = (void *) message;

    return mqtt_publish(client, KAWAII_MQTT_PUBTOPIC, &msg);
}

static void mqtt_date_pub_json(mqtt_client_t *client, rt_uint16_t light_intensity, float temperature, float humidity) {
    char *str = NULL;
    /* 创建一个JSON数据对象(链表头结点) */
    cJSON *cupload = cJSON_CreateObject();
    /* 创建int类型数据 */
    cJSON *cdate1 = cJSON_CreateNumber(light_intensity);
    cJSON *cdate2 = cJSON_CreateNumber(temperature);
    cJSON *cdate3 = cJSON_CreateNumber(humidity);
    /* 创建一个JSON数据对象(链表结点) */
    cJSON *csensor = cJSON_CreateObject();
    /* 创建 "date": cdate键值对，加入到 cparams对象中 */
    cJSON_AddItemToObject(csensor, "light_intensity", cdate1);
    cJSON_AddItemToObject(csensor, "temperature", cdate2);
    cJSON_AddItemToObject(csensor, "humidity", cdate3);
    /* 创建字符串类型数据 */
    cJSON *cgroup_name = cJSON_CreateString("pub_crychic_Marisa");
    /* 加入到JSON数据对象cupload */
    cJSON_AddItemToObject(cupload, "group_name", cgroup_name);
    cJSON_AddItemToObject(cupload, "sensor", csensor);
    /* 转换为标准JSON数据格式 */
    str = cJSON_Print(cupload);

    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.qos = QOS0;
    msg.payload = (void *)str;

    rt_kprintf("cjson:%s\n", str);

    /* 发送JSON消息 */
    mqtt_publish(client, KAWAII_MQTT_PUBTOPIC, &msg);

    /* 释放str内存 */
    cJSON_free(str);
    /* 删除json对象，释放内存 */
    cJSON_Delete(cupload);
}

static void kawaii_mqtt_demo(void *parameter) {
    int rc;
    mqtt_client_t *client = NULL;
    rt_uint32_t received_data;
    float temperature, humidity;

    rt_thread_delay(5000);

    mqtt_log_init();

    client = mqtt_lease();

    mqtt_set_host(client, "broker.emqx.io");
    mqtt_set_port(client, "1883");
    mqtt_set_user_name(client, "group11");
    mqtt_set_password(client, "group11");
    mqtt_set_client_id(client, KAWAII_MQTT_CLIENTID);
    mqtt_set_clean_session(client, 1);

    KAWAII_MQTT_LOG_I("The ID of the Kawaii client is: %s ", KAWAII_MQTT_CLIENTID);

    rc = mqtt_connect(client);
    KAWAII_MQTT_LOG_I("mqtt_connect return : %d ", rc);

    mqtt_subscribe(client, KAWAII_MQTT_SUBTOPIC, QOS0, sub_topic_handle1);

    //发布一条消息
    mqtt_publish_handle2(client, "Initial test message...");

    while (1) {
        if (rt_mb_recv(test_mb, &received_data, RT_WAITING_FOREVER) == RT_EOK) {
            rt_kprintf("thread mqtt: received data: 0x%x\n\n", received_data);
            // 获取温度和湿度
            temperature = read_temp();
            humidity = read_humi();
            // 发送JSON消息
            mqtt_date_pub_json(client, (rt_uint16_t)received_data, temperature, humidity);
        }
        mqtt_sleep_ms(500);
    }
}

int ka_mqtt(void) {
    rt_thread_t tid_mqtt;

    tid_mqtt = rt_thread_create("kawaii_demo", kawaii_mqtt_demo, RT_NULL, 2048, 17, 10);

    if (tid_mqtt == RT_NULL) {
        return -RT_ERROR;
    }

    rt_thread_startup(tid_mqtt);

    return RT_EOK;
}

MSH_CMD_EXPORT(ka_mqtt, Kawaii MQTT client test program);

int main(void) {
    init_io_port();
    drv_stepMotor_init();

    int count = 1;
    htu21d_init();
    rt_err_t uwRet = RT_EOK;
    struct rt_sensor_data sensor_data1;
    rt_device_t dev1;

    dev1 = rt_device_find(SENSOR_DEVICE_TEMPUTRA_NAME);
    if (dev1 == RT_NULL) {
        rt_kprintf("Failed to find device: %s\n", SENSOR_DEVICE_TEMPUTRA_NAME);
        return -RT_ERROR;
    }

    /* 以只读及轮询模式打开传感器设备 */
    rt_device_open(dev1, RT_DEVICE_FLAG_RDONLY);

    ka_mqtt();

    // 创建邮箱
    test_mb = rt_mb_create("mb_test", 1, RT_IPC_FLAG_PRIO);

    if (test_mb != RT_NULL) {
        rt_kprintf("MAIL_SUCCEED!\n");
    }

    while (1) {
        rt_device_read(dev1, 0, &sensor_data1, 1);
        rt_uint32_t light = sensor_data1.data.light;

        // 发送亮度数据邮件
        uwRet = rt_mb_send(test_mb, light);
        if (uwRet == RT_EOK) {
            rt_kprintf("MAIL_SEND_SUCCESS\n");
        } else {
            rt_kprintf("MAIL_SEND_FAILURE\n");
        }

        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
