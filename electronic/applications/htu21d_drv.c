/*********************************************************************************************
* 文件: i2c_sample.c
* 作者：Zhouchj 2020.09.29
* 说明：I2C设备例程源文件
* 修改：
* 注释：
*********************************************************************************************/
#pragma once

#include "drivers/i2c.h"
#ifdef RT_USING_I2C
#include "htu21d_drv.h"


static struct rt_i2c_bus_device *i2c_bus = RT_NULL; 
float tempVal = 0.0f, humiVal = 0.0f;

/*********************************************************************************************
* 名称：write_reg()
* 功能：向I2C从设备寄存器中写数据
* 参数： *bus -> I2C设备对象句柄
*         reg -> 寄存器
*       *data -> 数据
* 返回：是否写入成功
* 修改：
* 注释：
*********************************************************************************************/
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
  rt_uint8_t buf[4] = {0};
  struct rt_i2c_msg msgs;
  buf[0] = reg;
  msgs.addr = HTU21D_ADDR;
  msgs.flags = RT_I2C_WR;
  msgs.buf = buf;
  msgs.len = 1;
  if(rt_i2c_transfer(bus, &msgs, 1) == 1)
    return RT_EOK;
  else
    return -RT_ERROR;
}

/*********************************************************************************************
* 名称：read_regs()
* 功能：读取I2C从设备寄存器数据
* 参数： *bus -> I2C设备对象句柄
*         len -> 读取数据长度
*        *buf -> 读取数据
* 返回：是否读取成功
* 修改：
* 注释：
*********************************************************************************************/
static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
  struct rt_i2c_msg msgs;
  msgs.addr = HTU21D_ADDR;
  msgs.flags = RT_I2C_RD;
  msgs.buf = buf;
  msgs.len = len;
  if(rt_i2c_transfer(bus, &msgs, 1) == 1)
    return RT_EOK;
  else
    return -RT_ERROR;
}

/*********************************************************************************************
* 名称：read_temp()
* 功能：读取温度值
* 参数：无
* 返回：温度值
* 修改：
* 注释：
*********************************************************************************************/
float read_temp(void)
{
  rt_uint8_t temp[2] = {0};
  write_reg(i2c_bus, HTU21D_READ_TEMP, 0);
  rt_thread_mdelay(50);
  read_regs(i2c_bus, 2, temp);
  if((temp[1] & 0x02) == 0)
  {
    tempVal = -46.85f + 175.72f * ((temp[0]<<8 | temp[1])&0xfffc) / (1<<16);
    return tempVal;
  }
  return -1;
}

/*********************************************************************************************
* 名称：read_humi()
* 功能：读取湿度值
* 参数：无
* 返回：湿度值
* 修改：
* 注释：
*********************************************************************************************/
float read_humi(void)
{
  rt_uint8_t temp[2] = {0};
  write_reg(i2c_bus, HTU21D_READ_HUMI, 0);
  rt_thread_mdelay(50);
  read_regs(i2c_bus, 2, temp);
  if ((temp[1]&0x02) == 0x02) 
  {
    humiVal = -6 + 125 * ((temp[0]<<8 | temp[1])&0xfffc) / (1<<16);
    return humiVal;
  }
  return -1;
}

/*********************************************************************************************
* 名称：htu21d_init()
* 功能：HTU21D初始化
* 参数：无
* 返回：初始化是否成功
* 修改：
* 注释：
*********************************************************************************************/
int htu21d_init(void)
{
  i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(HTU21D_I2C_BUS_NAME);
  if(i2c_bus == RT_NULL)
  {
    rt_kprintf("failed to find %s device!\n", HTU21D_I2C_BUS_NAME);
    return -1;
  }
  write_reg(i2c_bus, HTU21D_RESET, 0);
  rt_thread_mdelay(50);
  return 0;
}

#endif
