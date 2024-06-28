/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2020-05-22     shuobatian     first version
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <at_device_n21.h>

#if !defined(AT_SW_VERSION_NUM) || AT_SW_VERSION_NUM < 0x10300
#error "This AT Client version is older, please check and update latest AT Client!"
#endif

#define LOG_TAG "at.dev"
#include <at_log.h>

#ifdef AT_DEVICE_USING_N21

#define N21_WAIT_CONNECT_TIME 10000
#define N21_THREAD_STACK_SIZE 2048
#define N21_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX / 2)
// #define N21_THREAD_PRIORITY 6

static void n21_power_on(struct at_device *device)
{

    struct at_device_n21 *n21 = RT_NULL;

    n21 = (struct at_device_n21 *)device->user_data;

    /* not nead to set pin configuration for n21 device power on */
#if (N21_SAMPLE_POWER_PIN != -1)
#if (N21_SAMPLE_STATUS_PIN != -1)
    if (n21->power_pin == -1 || n21->power_status_pin == -1)
    {
        return;
    }

    if (rt_pin_read(n21->power_status_pin) == PIN_HIGH)
    {
        return;
    }
    rt_pin_write(n21->power_pin, PIN_HIGH);
    while (rt_pin_read(n21->power_status_pin) == PIN_LOW)
    {
        rt_thread_mdelay(10);
    }
    rt_pin_write(n21->power_pin, PIN_LOW);
#else
    if (n21->power_pin == -1)
    {
        return;
    }
    rt_pin_write(n21->power_pin, PIN_HIGH);

#endif
#endif
}

static void n21_power_off(struct at_device *device)
{
    struct at_device_n21 *n21 = RT_NULL;

    n21 = (struct at_device_n21 *)device->user_data;

#if (N21_SAMPLE_POWER_PIN != -1)
#if (N21_SAMPLE_STATUS_PIN != -1)
    /* not nead to set pin configuration for m26 device power on */
    if (n21->power_pin == -1 || n21->power_status_pin == -1)
    {
        return;
    }
    if (rt_pin_read(n21->power_status_pin) == PIN_LOW)
    {
        return;
    }

    rt_pin_write(n21->power_pin, PIN_HIGH);
    while (rt_pin_read(n21->power_status_pin) == PIN_HIGH)
    {
        rt_thread_mdelay(10);
    }
    rt_pin_write(n21->power_pin, PIN_LOW);
#else
    if (n21->power_pin == -1)
    {
        return;
    }
    rt_pin_write(n21->power_pin, PIN_LOW);
#endif
#endif
}

/* =============================  neoway n21 network interface operations ============================= */

/* set n21 network interface device status and address information */
static int n21_netdev_set_info(struct netdev *netdev)
{
#define N21_SET_INFO_RESP_SIZE 128
#define N21_INFO_RESP_TIMO rt_tick_from_millisecond(300)

    int result = RT_EOK;
    //    int at_result = 0;
    ip_addr_t addr;
    at_response_t resp = RT_NULL;
    struct at_device *device = RT_NULL;

    if (netdev == RT_NULL)
    {
        LOG_E("input network interface device is NULL.");
        return -RT_ERROR;
    }
    LOG_D("netdev set info");
    device = at_device_get_by_name(AT_DEVICE_NAMETYPE_NETDEV, netdev->name);
    if (device == RT_NULL)
    {
        LOG_E("get n21 device by netdev name(%s) failed.", netdev->name);
        return -RT_ERROR;
    }

    /* set network interface device status */
    netdev_low_level_set_status(netdev, RT_TRUE);
    netdev_low_level_set_link_status(netdev, RT_TRUE);
    netdev_low_level_set_dhcp_status(netdev, RT_TRUE);

    resp = at_create_resp(N21_SET_INFO_RESP_SIZE, 0, N21_INFO_RESP_TIMO);
    if (resp == RT_NULL)
    {
        LOG_E("n21 device(%s) set IP address failed, no memory for response object.", device->name);
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* set network interface device hardware address(IEMI) */
    {
#define N21_NETDEV_HWADDR_LEN 8
#define N21_IEMI_LEN 15

        char iemi[N21_IEMI_LEN] = {0};
        int i = 0, j = 0;

        /* send "AT+GSN" commond to get device IEMI */
        if (at_obj_exec_cmd(device->client, resp, "AT+CGSN") < 0)
        {
            result = -RT_ERROR;
            goto __exit;
        }

        if (at_resp_parse_line_args_by_kw(resp, "+CGSN:", "+CGSN: %s", iemi) <= 0)
        {
            LOG_E("n21 device(%s) IEMI get fail", device->name);
            result = -RT_ERROR;
            goto __exit;
        }

        netdev->hwaddr_len = N21_NETDEV_HWADDR_LEN;
        /* get hardware address by IEMI */
        for (i = 0, j = 0; i < N21_NETDEV_HWADDR_LEN && j < N21_IEMI_LEN; i++, j += 2)
        {
            if (j != N21_IEMI_LEN - 1)
            {
                netdev->hwaddr[i] = (iemi[j] - '0') * 10 + (iemi[j + 1] - '0');
            }
            else
            {
                netdev->hwaddr[i] = (iemi[j] - '0');
            }
        }

        LOG_D("hwaddr:%.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X,IEMI:%s", netdev->hwaddr[0],
              netdev->hwaddr[1], netdev->hwaddr[2], netdev->hwaddr[3], netdev->hwaddr[4],
              netdev->hwaddr[5], netdev->hwaddr[6], netdev->hwaddr[7], iemi);
    }
    LOG_D("get IP address");
    /* get network interface device IP address */
    {
#define IP_ADDR_SIZE_MAX 32
        char ipaddr[IP_ADDR_SIZE_MAX] = {0};

        /* send "AT+XIIC?" commond to get IP address */
        if (at_obj_exec_cmd(device->client, resp, "AT+XIIC?") < 0)
        {
            result = -RT_ERROR;
            goto __exit;
        }

        if (at_resp_parse_line_args_by_kw(resp, "+XIIC:    ", "+XIIC:%*[^,],%s", ipaddr) <= 0)
        {
            LOG_E("n21 device(%s) prase \"AT+XIIC?\" commands resposne data error!", device->name);
            result = -RT_ERROR;
            goto __exit;
        }

        LOG_I("n21 device(%s) IP address: %s", device->name, ipaddr);

        /* set network interface address information */
        inet_aton(ipaddr, &addr);
        netdev_low_level_set_ipaddr(netdev, &addr);
    }
    //n21 not support inquire dns

__exit:
    if (resp)
    {
        at_delete_resp(resp);
    }

    return result;
}

static void check_link_status_entry(void *parameter)
{
#define N21_LINK_STATUS_OK 1
#define N21_LINK_RESP_SIZE 64
#define N21_LINK_RESP_TIMO (3 * RT_TICK_PER_SECOND)
#define N21_LINK_DELAY_TIME (30 * RT_TICK_PER_SECOND)

    at_response_t resp = RT_NULL;
    int result_code, link_status;
    struct at_device *device = RT_NULL;
#if (N21_SAMPLE_STATUS_PIN != -1)
    struct at_device_n21 *n21 = RT_NULL;
#endif

    char parsed_data[10] = {0};
    struct netdev *netdev = (struct netdev *)parameter;

    LOG_D("statrt n21 device(%s) link status check \n");

    device = at_device_get_by_name(AT_DEVICE_NAMETYPE_NETDEV, netdev->name);
    if (device == RT_NULL)
    {
        LOG_E("get n21 device by netdev name(%s) failed.", netdev->name);
        return;
    }
#if (N21_SAMPLE_STATUS_PIN != -1)
    n21 = (struct at_device_n21 *)device->user_data;
#endif
    resp = at_create_resp(N21_LINK_RESP_SIZE, 0, N21_LINK_RESP_TIMO);
    if (resp == RT_NULL)
    {
        LOG_E("n21 device(%s) set check link status failed, no memory for response object.", device->name);
        return;
    }

    while (1)
    {
        /* send "AT+CEREG?" commond  to check netweork interface device link status */
        if (at_obj_exec_cmd(device->client, resp, "AT+CEREG?") < 0)
        {
            rt_thread_mdelay(N21_LINK_DELAY_TIME);
            LOG_E("n21 device(%s) send cgreg failed", device->name);
            continue;
        }

        link_status = -1;
        at_resp_parse_line_args_by_kw(resp, "+CEREG:", "+CEREG: %d,%d\r\n", &result_code, &link_status);

        /* check the network interface device link status  */
        if ((N21_LINK_STATUS_OK == link_status) != netdev_is_link_up(netdev))
        {

            netdev_low_level_set_link_status(netdev, (N21_LINK_STATUS_OK == link_status));
        }

#if (N21_SAMPLE_STATUS_PIN != -1)
        if (rt_pin_read(n21->power_status_pin) == PIN_HIGH) //check the module_status , if moduble_status is Low, user can do your logic here
        {
#endif
            if (at_obj_exec_cmd(device->client, resp, "AT+CSQ") == 0)
            {
                at_resp_parse_line_args_by_kw(resp, "+CSQ:", "+CSQ: %s", &parsed_data);
                if (strncmp(parsed_data, "99,99", sizeof(parsed_data)))
                {
                    LOG_W("n21 device(%s) signal strength: %s", device->name, parsed_data);
                }
            }
#if (N21_SAMPLE_STATUS_PIN != -1)
        }
        else
        {
            LOG_E("netdev name(%s) status pin is low", device->name);
            netdev_low_level_set_link_status(netdev, RT_FALSE);
            return;
        }
#endif
        rt_thread_mdelay(N21_LINK_DELAY_TIME);
    }
}

static int n21_netdev_check_link_status(struct netdev *netdev)
{

#define N21_LINK_THREAD_TICK 20
#define N21_LINK_THREAD_STACK_SIZE 1024
#define N21_LINK_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX - 22)

    rt_thread_t tid;
    char tname[RT_NAME_MAX] = {0};

    if (netdev == RT_NULL)
    {
        LOG_E("input network interface device is NULL.\n");
        return -RT_ERROR;
    }

    rt_snprintf(tname, RT_NAME_MAX, "%s_link", netdev->name);

    tid = rt_thread_create(tname, check_link_status_entry, (void *)netdev,
                           N21_LINK_THREAD_STACK_SIZE, N21_LINK_THREAD_PRIORITY, N21_LINK_THREAD_TICK);
    if (tid)
    {
        rt_thread_startup(tid);
    }

    return RT_EOK;
}

static int n21_net_init(struct at_device *device);

static int n21_netdev_set_up(struct netdev *netdev)
{
    struct at_device *device = RT_NULL;

    device = at_device_get_by_name(AT_DEVICE_NAMETYPE_NETDEV, netdev->name);
    if (device == RT_NULL)
    {
        LOG_E("get n21 device by netdev name(%s) failed.", netdev->name);
        return -RT_ERROR;
    }

    if (device->is_init == RT_FALSE)
    {
        n21_net_init(device);
        device->is_init = RT_TRUE;

        netdev_low_level_set_status(netdev, RT_TRUE);
        LOG_D("the network interface device(%s) set up status.", netdev->name);
    }

    return RT_EOK;
}

static int n21_netdev_set_down(struct netdev *netdev)
{
    struct at_device *device = RT_NULL;

    device = at_device_get_by_name(AT_DEVICE_NAMETYPE_NETDEV, netdev->name);
    if (device == RT_NULL)
    {
        LOG_E("get n21 device by netdev name(%s) failed.", netdev->name);
        return -RT_ERROR;
    }

    if (device->is_init == RT_TRUE)
    {
        n21_power_off(device);
        device->is_init = RT_FALSE;

        netdev_low_level_set_status(netdev, RT_FALSE);
        LOG_D("the network interface device(%s) set down status.", netdev->name);
    }

    return RT_EOK;
}

#ifdef NETDEV_USING_PING
static int n21_netdev_ping(struct netdev *netdev, const char *host,
            size_t data_len, uint32_t timeout, struct netdev_ping_resp *ping_resp
#if RT_VER_NUM >= 0x50100
            , rt_bool_t is_bind
#endif
            )
{
#define N21_PING_RESP_SIZE 512
#define N21_PING_IP_SIZE 16
#define N21_PING_TIMEO (5 * RT_TICK_PER_SECOND)

#define N21_PING_ERR_TIME 600
#define N21_PING_ERR_TTL 255

    int result = RT_EOK;
    int bytes, time, ttl, i;
    char ip_addr[N21_PING_IP_SIZE] = {0};
    at_response_t resp = RT_NULL;
    struct at_device *device = RT_NULL;

#if RT_VER_NUM >= 0x50100
    RT_UNUSED(is_bind);
#endif

    RT_ASSERT(netdev);
    RT_ASSERT(host);
    RT_ASSERT(ping_resp);

    device = at_device_get_by_name(AT_DEVICE_NAMETYPE_NETDEV, netdev->name);
    if (device == RT_NULL)
    {
        LOG_E("get n21 device by netdev name(%s) failed.", netdev->name);
        return -RT_ERROR;
    }

    for (i = 0; i < rt_strlen(host) && !isalpha(host[i]); i++)
        ;

    resp = at_create_resp(N21_PING_RESP_SIZE, 7, N21_PING_TIMEO);
    if (resp == RT_NULL)
    {
        LOG_E("n21 device(%s) set dns server failed, no memory for response object.", device->name);
        result = -RT_ERROR;
        goto __exit;
    }

    /* send "AT+PING=<ip>[,<timeout>,<size>,<num>]" commond to send ping request */
    /* n21 ping <size> ranges of ipv4(36-1500), ipv6(56-1500) */
    if (at_obj_exec_cmd(device->client, resp, "AT+PING=%s,%d,%d,1",
                        host, N21_PING_TIMEO / (RT_TICK_PER_SECOND / 10), data_len + 4) < 0)
    {
        result = -RT_ERROR;
        goto __exit;
    }
    //Reply from 39.156.69.79: bytes= 36 time = 148(ms), TTL = 255
    //Reply from %[^:]: bytes= %d time = %d(ms), TTL = %d
    if (at_resp_parse_line_args_by_kw(resp, "Reply from ", "Reply from %[^:]: bytes= %d time = %d(ms), TTL = %d",
                                      ip_addr, &bytes, &time, &ttl) <= 0)
    {
        result = -RT_ERROR;
        goto __exit;
    }

    /* the ping request timeout expires, the response time settting to 600 and ttl setting to 255 */
    if (time == N21_PING_ERR_TIME && ttl == N21_PING_ERR_TTL)
    {
        result = -RT_ETIMEOUT;
        goto __exit;
    }

    inet_aton(ip_addr, &(ping_resp->ip_addr));
    ping_resp->data_len = data_len;
    ping_resp->ticks = time;
    ping_resp->ttl = ttl;

__exit:
    if (resp)
    {
        at_delete_resp(resp);
    }

    return result;
}
#endif /* NETDEV_USING_PING */

#ifdef NETDEV_USING_NETSTAT
void n21_netdev_netstat(struct netdev *netdev)
{
    // TODO netstat support
}
#endif /* NETDEV_USING_NETSTAT */

const struct netdev_ops n21_netdev_ops =
    {
        n21_netdev_set_up,
        n21_netdev_set_down,

        RT_NULL, /* not support set ip, netmask, gatway address */
        RT_NULL, /* not support set dns server */
        RT_NULL, /* not support set DHCP status */

#ifdef NETDEV_USING_PING
        n21_netdev_ping,
#endif
#ifdef NETDEV_USING_NETSTAT
        n21_netdev_netstat,
#endif
};

static struct netdev *n21_netdev_add(const char *netdev_name)
{
#define n21_NETDEV_MTU 1500
    struct netdev *netdev = RT_NULL;

    RT_ASSERT(netdev_name);

    netdev = netdev_get_by_name(netdev_name);
    if (netdev != RT_NULL)
    {
        return (netdev);
    }

    netdev = (struct netdev *)rt_calloc(1, sizeof(struct netdev));
    if (netdev == RT_NULL)
    {
        LOG_E("no memory for n21 device(%s) netdev structure.", netdev_name);
        return RT_NULL;
    }

    netdev->mtu = n21_NETDEV_MTU;
    netdev->ops = &n21_netdev_ops;

#ifdef SAL_USING_AT
    extern int sal_at_netdev_set_pf_info(struct netdev * netdev);
    /* set the network interface socket/netdb operations */
    sal_at_netdev_set_pf_info(netdev);
#endif

    netdev_register(netdev, netdev_name, RT_NULL);

    return netdev;
}

/* =============================  n21 device operations ============================= */

#define AT_SEND_CMD(client, resp, resp_line, timeout, cmd)                                      \
    do                                                                                          \
    {                                                                                           \
        (resp) = at_resp_set_info((resp), 128, (resp_line), rt_tick_from_millisecond(timeout)); \
        if (at_obj_exec_cmd((client), (resp), (cmd)) < 0)                                       \
        {                                                                                       \
            result = -RT_ERROR;                                                                 \
            goto __exit;                                                                        \
        }                                                                                       \
    } while (0)

/* init for n21 */
static void n21_init_thread_entry(void *parameter)
{
#define INIT_RETRY 5
#define CPIN_RETRY 10
#define CSQ_RETRY 10
#define CREG_RETRY 10
#define CEREG_RETRY 30
#define CCID_SIZE 20

    int i, retry_num = INIT_RETRY;
    char ccid[CCID_SIZE] = {0};
    char parsed_data[10] = {0};
    rt_err_t result = RT_EOK;
    at_response_t resp = RT_NULL;
    struct at_device *device = (struct at_device *)parameter;
    struct at_client *client = device->client;

    resp = at_create_resp(128, 0, rt_tick_from_millisecond(300));
    if (resp == RT_NULL)
    {
        LOG_E("no memory for n21 device(%s) response structure.", device->name);
        return;
    }

    while (retry_num--)
    {
        rt_memset(parsed_data, 0, sizeof(parsed_data));

        n21_power_on(device);
        rt_thread_mdelay(5000); //check the n21 hardware manual, when we use the pow_key to start n21, it takes about 20s,so we put 25s here to ensure starting n21 normally.

        LOG_I("start initializing the n21 device(%s)", device->name);
        /* wait n21 startup finish */
        if (at_client_obj_wait_connect(client, N21_WAIT_CONNECT_TIME))
        {
            result = -RT_ETIMEOUT;
            goto __exit;
        }

        /* disable echo */
        AT_SEND_CMD(client, resp, 0, 300, "ATE0");
        /* get module version */
        AT_SEND_CMD(client, resp, 0, 300, "ATI");
        /* show module version */
        for (i = 0; i < (int)resp->line_counts - 1; i++)
        {
            LOG_I("%s", at_resp_get_line(resp, i + 1));
        }
        /* check SIM card */
        for (i = 0; i < CPIN_RETRY; i++)
        {
            at_resp_set_info(resp, 128, 2, 5 * RT_TICK_PER_SECOND);
            if (at_obj_exec_cmd(client, resp, "AT+CCID") < 0)
            {
                LOG_E("AT+CCID ERROR! retry:%d.", i);
                rt_thread_mdelay(1000);
                continue;
            }

            if (at_resp_parse_line_args_by_kw(resp, "+CCID:", "+CCID: %s", ccid))
            {
                LOG_I("n21 device(%s) SIM card detection success.", device->name);
                LOG_I("CCID: %s", ccid);
                break;
            }
            rt_thread_mdelay(1000);
        }
        if (i == CPIN_RETRY)
        {
            LOG_E("n21 device(%s) SIM card detection failed.", device->name);
            result = -RT_ERROR;
            goto __exit;
        }

        /* waiting for dirty data to be digested */
        rt_thread_mdelay(10);
        LOG_I("register network!");
        /* check the GSM network is registered */
        for (i = 0; i < CREG_RETRY; i++)
        {
            AT_SEND_CMD(client, resp, 0, 300, "AT+CREG?");
            at_resp_parse_line_args_by_kw(resp, "+CREG:", "+CREG: %s", &parsed_data);
            if (!strncmp(parsed_data, "0,1", sizeof(parsed_data)) ||
                !strncmp(parsed_data, "0,5", sizeof(parsed_data)))
            {
                LOG_I("n21 device(%s) network is registered(%s),", device->name, parsed_data);
                break;
            }
            rt_thread_mdelay(1000);
        }
        if (i == CREG_RETRY)
        {
            LOG_E("n21 device(%s) network is register failed(%s).", device->name, parsed_data);
            rt_thread_mdelay(10);
            result = -RT_ERROR;
            goto __exit;
        }
        /* check the GPRS network is registered */
        for (i = 0; i < CEREG_RETRY; i++)
        {
            AT_SEND_CMD(client, resp, 0, 300, "AT+CEREG?");
            at_resp_parse_line_args_by_kw(resp, "+CEREG:", "+CEREG: %s", &parsed_data);
            if (!strncmp(parsed_data, "0,1", sizeof(parsed_data)) ||
                !strncmp(parsed_data, "0,5", sizeof(parsed_data)))
            {
                LOG_I("n21 device(%s) GPRS network is registered(%s).", device->name, parsed_data);
                break;
            }
            rt_thread_mdelay(1000);
        }
        if (i == CEREG_RETRY)
        {
            LOG_E("n21 device(%s) GPRS network is register failed(%s).", device->name, parsed_data);
            result = -RT_ERROR;
            goto __exit;
        }

        /* check signal strength */
        for (i = 0; i < CSQ_RETRY; i++)
        {
            AT_SEND_CMD(client, resp, 0, 300, "AT+CSQ");
            at_resp_parse_line_args_by_kw(resp, "+CSQ:", "+CSQ: %s", &parsed_data);
            if (strncmp(parsed_data, "99,99", sizeof(parsed_data)))
            {
                LOG_I("n21 device(%s) signal strength: %s", device->name, parsed_data);
                break;
            }
            rt_thread_mdelay(1000);
        }
        if (i == CSQ_RETRY)
        {
            LOG_E("n21 device(%s) signal strength check failed (%s)", device->name, parsed_data);
            result = -RT_ERROR;
            goto __exit;
        }

        AT_SEND_CMD(client, resp, 0, 300, "AT+COPS=3,0");
        AT_SEND_CMD(client, resp, 0, 300, "AT+COPS?");
        at_resp_parse_line_args_by_kw(resp, "+COPS:", "+COPS: %*[^\"]\"%[^\"]", &parsed_data);

        if (rt_strcmp(parsed_data, "ChinaMobile") == 0)
        {
            /* "CMCC" */
            LOG_I("n21 device(%s) network operator: %s", device->name, parsed_data);
        }
        else if (rt_strcmp(parsed_data, "ChinaUnicom") == 0)
        {
            /* "UNICOM" */
            LOG_I("n21 device(%s) network operator: %s", device->name, parsed_data);
        }
        else if (rt_strcmp(parsed_data, "ChinaTelecom") == 0)
        {
            /* "CT" */
            LOG_I("n21 device(%s) network operator: %s", device->name, parsed_data);
        }
        else
        {
            LOG_E("n21 device(%s) Unknown carrier:%s", device->name, parsed_data);
        }
        /* the device default response timeout is 150 seconds, but it set to 20 seconds is convenient to use. */
        AT_SEND_CMD(client, resp, 0, 20 * 1000, "AT+XIIC=1");

        AT_SEND_CMD(client, resp, 0, 300, "AT+XIIC?");
        if (at_resp_get_line_by_kw(resp, "ERROR") != RT_NULL)
        {
            LOG_E("n21 device(%s) get the local address failed.", device->name);
            result = -RT_ERROR;
            goto __exit;
        }
        result = RT_EOK;

        AT_SEND_CMD(client, resp, 0, 300, "AT+RECVMODE=1"); //set direct receive

    __exit:
        if (result == RT_EOK)
        {
            break;
        }
        else
        {
            /* power off the n21 device */
            n21_power_off(device);
            rt_thread_mdelay(1000);

            LOG_I("n21 device(%s) initialize retry...", device->name);
        }
    }

    if (resp)
    {
        at_delete_resp(resp);
    }

    if (result == RT_EOK)
    {
        /* set network interface device status and address information */
        n21_netdev_set_info(device->netdev);
        /* check and create link staus sync thread  */
        if (rt_thread_find(device->netdev->name) == RT_NULL)
        {
            n21_netdev_check_link_status(device->netdev);
        }
        LOG_I("n21 device(%s) network initialize success!", device->name);
    }
    else
    {
        LOG_E("n21 device(%s) network initialize failed(%d)!", device->name, result);
    }
}

static int n21_net_init(struct at_device *device)
{
#ifdef AT_DEVICE_N21_INIT_ASYN
    rt_thread_t tid;
    tid = rt_thread_create("n21_net_init", n21_init_thread_entry, (void *)device,
                           N21_THREAD_STACK_SIZE, N21_THREAD_PRIORITY, 20);
    if (tid)
    {
        rt_thread_startup(tid);
    }
    else
    {
        LOG_E("create n21 device(%s) initialization thread failed.", device->name);
        return -RT_ERROR;
    }
#else
    n21_init_thread_entry(device);
#endif /* AT_DEVICE_N21_INIT_ASYN */

    return RT_EOK;
}

static void urc_func(struct at_client *client, const char *data, rt_size_t size)
{
    RT_ASSERT(data);

    LOG_I("URC data : %.*s", size, data);
}

/* n21 device URC table for the device control */
static const struct at_urc urc_table[] =
    {
        {"+PBREADY", "\r\n", urc_func},
        {"CLOSED", "\r\n", urc_func},
};

static int n21_init(struct at_device *device)
{
    struct at_device_n21 *n21 = (struct at_device_n21 *)device->user_data;

    /* initialize AT client */
#if RT_VER_NUM >= 0x50100
    at_client_init(n21->client_name, n21->recv_line_num, n21->recv_line_num);
#else
    at_client_init(n21->client_name, n21->recv_line_num);
#endif

    device->client = at_client_get(n21->client_name);
    if (device->client == RT_NULL)
    {
        LOG_E("n21 device(%s) initialize failed, get AT client(%s) failed.", n21->device_name, n21->client_name);
        return -RT_ERROR;
    }

    /* register URC data execution function  */
    at_obj_set_urc_table(device->client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]));

#ifdef AT_USING_SOCKET
    n21_socket_init(device);
#endif

    /* add n21 device to the netdev list */
    device->netdev = n21_netdev_add(n21->device_name);
    if (device->netdev == RT_NULL)
    {
        LOG_E("n21 device(%s) initialize failed, get network interface device failed.", n21->device_name);
        return -RT_ERROR;
    }

    /* initialize n21 pin configuration */
    if (n21->power_pin != -1)
    {
        rt_pin_mode(n21->power_pin, PIN_MODE_OUTPUT);
    }
    if (n21->power_status_pin != -1)
    {
        rt_pin_mode(n21->power_status_pin, PIN_MODE_INPUT);
    }

    /* initialize n21 device network */
    return n21_netdev_set_up(device->netdev);
}

static int n21_deinit(struct at_device *device)
{
    return n21_netdev_set_down(device->netdev);
}

static int n21_reset(struct at_device *device)
{
    int result = RT_EOK;
    struct at_client *client = device->client;

    /* n21 only poweroff cmd,not support reboot.*/
    /* use power pin reboot */
#if (N21_SAMPLE_POWER_PIN != -1)
    rt_pin_write(N21_SAMPLE_POWER_PIN, 0);
    rt_thread_mdelay(500);
    rt_pin_write(N21_SAMPLE_POWER_PIN, 1);

    rt_thread_mdelay(1000);

    /* waiting 10 seconds for n21 device reset */
    device->is_init = RT_FALSE;
    if (at_client_obj_wait_connect(client, N21_WAIT_CONNECT_TIME))
    {
        return -RT_ETIMEOUT;
    }

    /* initialize n21 device network */
    n21_net_init(device);

    device->is_init = RT_TRUE;
#else
    result = -RT_ERROR;
#endif

    return result;
}

static int n21_control(struct at_device *device, int cmd, void *arg)
{
    int result = -RT_ERROR;

    RT_ASSERT(device);

    switch (cmd)
    {
    case AT_DEVICE_CTRL_POWER_ON:
    case AT_DEVICE_CTRL_POWER_OFF:
    case AT_DEVICE_CTRL_LOW_POWER:
    case AT_DEVICE_CTRL_SLEEP:
    case AT_DEVICE_CTRL_WAKEUP:
    case AT_DEVICE_CTRL_NET_CONN:
    case AT_DEVICE_CTRL_NET_DISCONN:
    case AT_DEVICE_CTRL_SET_WIFI_INFO:
    case AT_DEVICE_CTRL_GET_SIGNAL:
    case AT_DEVICE_CTRL_GET_GPS:
    case AT_DEVICE_CTRL_GET_VER:
        LOG_W("n21 not support the control command(%d).", cmd);
        break;
    case AT_DEVICE_CTRL_RESET:
        result = n21_reset(device);
        break;
    default:
        LOG_E("input error control command(%d).", cmd);
        break;
    }

    return result;
}

const struct at_device_ops n21_device_ops =
    {
        n21_init,
        n21_deinit,
        n21_control,
};

static int n21_device_class_register(void)
{
    struct at_device_class *class = RT_NULL;

    class = (struct at_device_class *)rt_calloc(1, sizeof(struct at_device_class));
    if (class == RT_NULL)
    {
        LOG_E("no memory for n21 device class create.");
        return -RT_ENOMEM;
    }

    /* fill n21 device class object */
#ifdef AT_USING_SOCKET
    n21_socket_class_register(class);
#endif
    class->device_ops = &n21_device_ops;

    return at_device_class_register(class, AT_DEVICE_CLASS_N21);
}
INIT_DEVICE_EXPORT(n21_device_class_register);

#endif /* AT_DEVICE_USING_N21 */