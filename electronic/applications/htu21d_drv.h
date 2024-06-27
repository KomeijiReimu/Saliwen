/*********************************************************************************************
* 文件: i2c_sample.h
* 作者：Zhouchj 2020.09.29
* 说明：I2C设备例程头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef __HTU21D_H_
#define __HTU21D_H_
#include <rtthread.h>
#ifdef RT_USING_I2C

#define HTU21D_I2C_BUS_NAME           "i2c1"                    // I2C设备名称

#define HTU21D_ADDR                   0x40                      // HTU21D地址
#define HTU21D_RESET                  0xFE                      // HTU21D复位寄存器地址
#define HTU21D_READ_TEMP              0xF3                      // HTU21D温度寄存器地址
#define HTU21D_READ_HUMI              0xF5                      // HTU21D湿度寄存器地址

int htu21d_init(void);
float read_temp(void);
float read_humi(void);
#endif //RT_USING_I2C
#endif // __I2C_SAMPLE_H_
