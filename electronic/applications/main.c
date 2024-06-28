#include <rtthread.h>
#include <rtdevice.h>
#include "cjson.h"
#include <stdio.h>
#include <stdint.h>
#include "led_control.h"
#include "htu21d_drv.h"
#include "drv_stepMotor.h"
#include "mqttclient.h"
#include "fbm320_drv.h"

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

enum LedStatus {
    LED_IDLE, LED_CLAP
};

volatile enum LedStatus curr_state = LED_IDLE;

int cnt = 0;
// struct rt_timer *debounce_timer, *clap_space_timer;


volatile unsigned int flag_sound_first_pulse = 1;

volatile unsigned int flag_led_status = 0x0;
volatile unsigned int flag_clap_timer = 0;
volatile unsigned int flag_clap_timer_timeout = 0;
volatile unsigned int flag_clap_send = 0;
static float tmp_arr = 30.0;
static int lt_arr = 6000;
// 初始化禁用阈值功能
static int flag_arr = 0;
rt_base_t KEY1;
rt_base_t FAN;
rt_uint32_t status;

rt_timer_t clap_space_timer = RT_NULL;
rt_timer_t debounce_timer = RT_NULL;
/* 邮箱控制块 */
static rt_mailbox_t test_mb = RT_NULL;
static int flag = 0;


void on_debounce_timer(void *parameter) {
    rt_kprintf(">>>Debouned cnt = %d\n", cnt);
    flag_sound_first_pulse = 1;
}

void on_clap_space_timer(void *parameter) {

    flag_clap_timer = 1;


    rt_kprintf(">>> 1s clap_timer_timeout\n");


    if (cnt == 2) {

        curr_state = LED_IDLE;
        if (flag_led_status) {
            flag_led_status = 0x0;
            control_led(0xf);
            rt_kprintf("shift\n");
        } else {
            flag_led_status = 0xf;
            control_led(0x0);
            rt_kprintf("shift\n");
        }
        rt_kprintf(">>>>>>>  CLAP -> IDLE\n");
        flag_clap_send = 1;
    } else {
        curr_state = LED_IDLE;
        rt_kprintf("CLAP -> IDLE\n");
    }
    cnt = 0;

}

void timer_init(void) {
    clap_space_timer = rt_timer_create("clap_space_timer", // 定时器名称
                                       on_clap_space_timer, // 定时器入口函数
                                       RT_NULL, // 入口函数传入参数
                                       1000, // 定时器超时时间
                                       RT_TIMER_FLAG_ONE_SHOT);

    debounce_timer = rt_timer_create("debounce_timer", // 定时器名称
                                     on_debounce_timer, // 定时器入口函数
                                     RT_NULL, // 入口函数传入参数
                                     250, // 定时器超时时间
                                     RT_TIMER_FLAG_ONE_SHOT); // 定时器

}

void on_soundlevel(void *para) {
    rt_timer_start(debounce_timer);
    if (flag_sound_first_pulse) {
        cnt++;
        flag_sound_first_pulse = 0;
        rt_kprintf("on_soundlevel\n");

        if (curr_state == LED_IDLE) {

            // rt_timer_start(debounce_timer);
            curr_state = LED_CLAP;
            rt_timer_start(clap_space_timer);
            rt_kprintf(">>>>>>>   IDLE -> CLAP\n");

        } else {
            // 如果定时器正在运行，先停止它
            rt_timer_stop(clap_space_timer);


            rt_timer_start(clap_space_timer);

        }


    }
}

void on_key1_clicked(void *args) {
    flag_clap_send = 1;
}

void init_int(void) {
    rt_pin_attach_irq(rt_pin_get("PC.0"), PIN_IRQ_MODE_FALLING, on_soundlevel, RT_NULL);
    rt_pin_irq_enable(rt_pin_get("PC.0"), PIN_IRQ_ENABLE);
    KEY1 = rt_pin_get("PB.12");
    FAN = rt_pin_get("PE.5");
    rt_pin_mode(KEY1, PIN_MODE_INPUT);

    rt_pin_mode(rt_pin_get("PB.12"), PIN_MODE_INPUT);
    rt_pin_attach_irq(rt_pin_get("PB.12"), PIN_IRQ_MODE_FALLING, on_key1_clicked, RT_NULL);
    rt_pin_irq_enable(rt_pin_get("PB.12"), PIN_IRQ_ENABLE);

    rt_pin_mode(rt_pin_get("PE.5"), PIN_MODE_OUTPUT);
    // rt_pin_attach_irq(rt_pin_get("PE.5"), PIN_IRQ_MODE_FALLING, on_key1_clicked, RT_NULL);
    // rt_pin_irq_enable(rt_pin_get("PE.5"), PIN_IRQ_ENABLE);

}

void fan(unsigned a) {
    if (a)
        rt_pin_write(FAN, PIN_HIGH);
    else
        rt_pin_write(FAN, PIN_LOW);
}

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

    /* 检查是否包含 "lt" 和 "tmp" */
    cJSON *lt_item = cJSON_GetObjectItem(json, "lt");
    cJSON *tmp_item = cJSON_GetObjectItem(json, "tmp");
    cJSON *start_off = cJSON_GetObjectItem(json, "msg");
    cJSON *step = cJSON_GetObjectItem(json, "step");

    if (cJSON_IsString(start_off) && (strcmp(start_off->valuestring, "start") == 0)) {
        flag_arr = 1;
    }
    if (cJSON_IsString(start_off) && (strcmp(start_off->valuestring, "stop") == 0)) {
        flag_arr = 0;
    }
    if (cJSON_IsString(step) && (strcmp(step->valuestring, "stop") == 0)) {
        rt_kprintf("stop!!\n");
        drv_stepMotor_ctrl(0);
    }
    if (cJSON_IsString(step) && (strcmp(step->valuestring, "start") == 0)) {
        rt_kprintf("start\n");
        drv_stepMotor_ctrl(1);
    }

    if (cJSON_IsNumber(lt_item) && cJSON_IsNumber(tmp_item)) {
        lt_arr = lt_item->valueint;
        tmp_arr = (float) tmp_item->valuedouble;
        rt_kprintf("Updated lt_arr to %d\n", lt_arr);
        rt_kprintf("Updated tmp_arr to %f\n", tmp_arr);
    } else {
        KAWAII_MQTT_LOG_E("Failed to find valid 'lt' or 'tmp' in the JSON message");
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

static void mqtt_date_pub_json(mqtt_client_t *client, rt_uint16_t light_intensity, float temperature, float humidity, float pressure) {
    char *str = NULL;
    /* 创建一个JSON数据对象(链表头结点) */
    cJSON *cupload = cJSON_CreateObject();
    /* 创建int类型数据 */
    cJSON *cdate1 = cJSON_CreateNumber(light_intensity);
    cJSON *cdate2 = cJSON_CreateNumber(temperature);
    cJSON *cdate3 = cJSON_CreateNumber(humidity);
    cJSON *cdate4 = cJSON_CreateNumber(pressure);
    /* 创建一个JSON数据对象(链表结点) */
    cJSON *csensor = cJSON_CreateObject();
    /* 创建 "date": cdate键值对，加入到 cparams对象中 */
    cJSON_AddItemToObject(csensor, "light_intensity", cdate1);
    cJSON_AddItemToObject(csensor, "temperature", cdate2);
    cJSON_AddItemToObject(csensor, "humidity", cdate3);
    cJSON_AddItemToObject(csensor, "pressure", cdate4);
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
    msg.payload = (void *) str;

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
    float temperature, humidity, pressure;

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
        if (rt_mb_recv(test_mb, &received_data, RT_WAITING_FOREVER) == RT_EOK && flag_clap_send == 1) {
            flag_clap_send = 0;
            rt_kprintf("thread mqtt: received data: 0x%x\n\n", received_data);
            // 获取温度和湿度
            temperature = read_temp();
            humidity = read_humi();
            // 获取大气压数据
            float tmp_fbm = 0.0;
            if (fbm320_read_data(&tmp_fbm, &pressure) != 0) {
                rt_kprintf("Failed to read data from FBM320!\n");
                pressure = 0.0;
            }
            // 发送JSON消息
            mqtt_date_pub_json(client, (rt_uint16_t) received_data, temperature, humidity, pressure);
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
    init_int();
    timer_init();
    int result = fbm320_init();
    if (result != 0) {
        rt_kprintf("FBM320 initialization failed!\n");
    } else {
        rt_kprintf("FBM320 initialization successful!\n");
    }

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

    float tmp_fbm = 0.0;
    float pressure = 0.0;
    if (fbm320_read_data(&tmp_fbm, &pressure) == 0) {
        rt_kprintf("Temperature: %.2f °C\n", tmp_fbm);
        rt_kprintf("Pressure: %.2f Pa\n", pressure);
    } else {
        rt_kprintf("Failed to read data from FBM320!\n");
    }

    ka_mqtt();

    drv_stepMotor_ctrl(1);

    // 创建邮箱
    test_mb = rt_mb_create("mb_test", 1, RT_IPC_FLAG_PRIO);

    if (test_mb != RT_NULL) {
        rt_kprintf("MAIL_SUCCEED!\n");
    }

    while (1) {

        rt_device_read(dev1, 0, &sensor_data1, 1);
        rt_uint32_t light = sensor_data1.data.light;
        float tmp = read_temp();
        int lt = sensor_data1.data.light;
        // 发送亮度数据邮件
        uwRet = rt_mb_send(test_mb, light);
        if (uwRet == RT_EOK) {
            rt_kprintf("MAIL_SEND_SUCCESS\n");
        } else {
            rt_kprintf("MAIL_SEND_FAILURE\n");
        }
        if (flag_arr == 1) {
            if (tmp >= tmp_arr) {
                fan(1);
            } else {
                fan(0);
            }

            if (lt >= lt_arr) {
                turn(1);
            } else {
                turn(0);
            }
        }

        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
