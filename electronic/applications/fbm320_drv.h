/*********************************************************************************************
* 文件: fbm320_sample.h
* 作者：Zhouchj 2020.09.29
* 说明：FBM320设备例程头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef __FBM320_SAMPLE_H_
#define __FBM320_SAMPLE_H_
#include <rtthread.h>
#include "drivers/i2c.h"
#ifdef RT_USING_I2C
#define FBM320_I2C_BUS_NAME           "i2c1"                    // I2C设备名称

#define FBM320_ADDR                   0x6C 
#define FBM320_ID_ADDR                0x6B
#define FBM320_ID                     0x42
#define FBM320_CONFIG                 0XF4
#define FBM320_RESET                  0XE0
#define FBM320_DATAM                  0xF6 
#define FBM320_DATAC                  0xF7 
#define FBM320_DATAL                  0xF8 
#define OSR1024                       0x34
#define OSR2048                       0x74
#define OSR4096                       0xB4
#define OSR8192                       0xF4
#define TEMPERATURE                   0x2E
#define RESET_DATA                    0xB6

int fbm320_init(void);
unsigned char  fbm320_read_data(float *temperature, float *pressure);
#endif //#ifdef RT_USING_I2C
#endif // __I2C_SAMPLE_H_
