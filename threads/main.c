#include <rtthread.h>
#include <rtdevice.h>
#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

#include <rtdbg.h>
static rt_thread_t tid1,tid2 = RT_NULL;
rt_base_t LED1_PIN_NUM;
rt_base_t LED2_PIN_NUM;
rt_base_t LED3_PIN_NUM;
rt_base_t LED4_PIN_NUM;
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;
    while(1){
        //rt_kprintf("thread1 count: %d\n", count ++);

        //rt_thread_mdelay(600);
        rt_pin_write(LED1_PIN_NUM, PIN_LOW);
        rt_pin_write(LED2_PIN_NUM, PIN_LOW);

        //rt_thread_mdelay(600);
        rt_pin_write(LED1_PIN_NUM, PIN_HIGH);
        rt_pin_write(LED2_PIN_NUM, PIN_HIGH);
    }
}

static void thread2_entry(void *parameter)
{
    rt_uint32_t count = 0;
    while(1){
        rt_kprintf("thread2 count: %d\n", count ++);

        rt_thread_mdelay(300);

        rt_pin_write(LED3_PIN_NUM, PIN_LOW);
        rt_pin_write(LED4_PIN_NUM, PIN_LOW);

        rt_thread_mdelay(300);
        rt_pin_write(LED3_PIN_NUM, PIN_HIGH);
        rt_pin_write(LED4_PIN_NUM, PIN_HIGH);
    }
}

void init_io_port(){
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

int main()
{

    init_io_port();

    tid1 = rt_thread_create("thread1",
            thread1_entry,RT_NULL,
            THREAD_STACK_SIZE,
            20,THREAD_TIMESLICE);

    if(tid1 != RT_NULL)
        rt_thread_startup(tid1);

    tid2 = rt_thread_create("thread2",
            thread2_entry,RT_NULL,
            THREAD_STACK_SIZE,
            25,THREAD_TIMESLICE);

    if(tid2 != RT_NULL)
        rt_thread_startup(tid2);

    while(1){
        //rt_thread_mdelay(1000);
    }

    return RT_EOK;

}
