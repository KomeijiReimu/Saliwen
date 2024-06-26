#include <rtthread.h>
#include <rtdevice.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

#include <rtdbg.h>

enum LedStatus {
    LED_IDLE, LED_CLAP
};

volatile enum LedStatus curr_state = LED_IDLE;

rt_base_t LED1_PIN_NUM;
rt_base_t LED2_PIN_NUM;
rt_base_t LED3_PIN_NUM;
rt_base_t LED4_PIN_NUM;
int cnt = 0;
// struct rt_timer *debounce_timer, *clap_space_timer;

volatile unsigned int flag_sound_first_pulse = 1;

volatile unsigned int flag_led_status = 0x0;
volatile unsigned int flag_clap_timer = 0;
volatile unsigned int flag_clap_timer_timeout = 0;
rt_uint32_t status;

rt_timer_t clap_space_timer = RT_NULL;
rt_timer_t debounce_timer = RT_NULL;

void on_debounce_timer(void *parameter) {
    rt_kprintf(">>>Debouned cnt = %d\n",cnt);
    flag_sound_first_pulse = 1;
}

void on_clap_space_timer(void *parameter) {

    flag_clap_timer = 1;


    rt_kprintf(">>> 1s clap_timer_timeout\n");


    if(cnt == 2)
    {

        curr_state = LED_IDLE;
        if(flag_led_status) {
            flag_led_status = 0x0;
            control_led(0xf);
            rt_kprintf("shift\n");
        }else {
            flag_led_status = 0xf;
            control_led(0x0);
            rt_kprintf("shift\n");
        }
        rt_kprintf(">>>>>>>  CLAP -> IDLE\n");
    }
    else {
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
        cnt ++;
        flag_sound_first_pulse = 0;
        rt_kprintf("on_soundlevel\n");

        if (curr_state == LED_IDLE) {

            // rt_timer_start(debounce_timer);
            curr_state = LED_CLAP;
            rt_timer_start(clap_space_timer);
            rt_kprintf(">>>>>>>   IDLE -> CLAP\n");

        }
        else{
                // 如果定时器正在运行，先停止它
            rt_timer_stop(clap_space_timer);


            rt_timer_start(clap_space_timer);

        }


    }
}

void init_io_port(void) {

    //初始化为输出模式
    LED1_PIN_NUM = rt_pin_get("PE.0");
    LED2_PIN_NUM = rt_pin_get("PE.1");
    LED3_PIN_NUM = rt_pin_get("PE.2");
    LED4_PIN_NUM = rt_pin_get("PE.3");
    rt_pin_write(LED1_PIN_NUM, PIN_HIGH);
       rt_pin_write(LED2_PIN_NUM, PIN_HIGH);
       rt_pin_write(LED3_PIN_NUM, PIN_HIGH);
       rt_pin_write(LED4_PIN_NUM, PIN_HIGH);
    rt_pin_mode(LED1_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LED4_PIN_NUM, PIN_MODE_OUTPUT);
}

void control_led(volatile unsigned int x) {
    //根据cmd变量的低四位，控制led输出
    //bit:0 为1， led1 点亮
    if(x == 0xf)
    {
           rt_pin_write(LED1_PIN_NUM, PIN_LOW);
           rt_pin_write(LED2_PIN_NUM, PIN_LOW);
           rt_pin_write(LED3_PIN_NUM, PIN_LOW);
           rt_pin_write(LED4_PIN_NUM, PIN_LOW);
    }
    else {
             rt_pin_write(LED1_PIN_NUM, PIN_HIGH);
             rt_pin_write(LED2_PIN_NUM, PIN_HIGH);
             rt_pin_write(LED3_PIN_NUM, PIN_HIGH);
             rt_pin_write(LED4_PIN_NUM, PIN_HIGH);
    }


}

void init_int(void) {
    rt_pin_attach_irq(rt_pin_get("PC.0"), PIN_IRQ_MODE_FALLING, on_soundlevel, RT_NULL);
    rt_pin_irq_enable(rt_pin_get("PC.0"), PIN_IRQ_ENABLE);
}

int main(void) {



    init_io_port();
    init_int();
    timer_init();
    while (1) {

//
//        if (flag_led_status) {
//            control_led(0xf);
//        } else {
//            control_led(0x0);
//        }

        rt_thread_delay(10);

    }
    return RT_EOK;
}
