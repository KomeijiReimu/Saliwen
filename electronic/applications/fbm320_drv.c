/*********************************************************************************************
* 文件: fbm320_sample.c
* 作者：Zhouchj 2020.11.05
* 说明：FBM320设备例程源文件
* 修改：
* 注释：
*********************************************************************************************/
#include "fbm320_drv.h"
#ifdef RT_USING_I2C

static struct rt_i2c_bus_device *fbm320_bus = RT_NULL;

long UP_S=0, UT_S=0, RP_S=0, RT_S=0, OffP_S=0;
long UP_I=0, UT_I=0, RP_I=0, RT_I=0, OffP_I=0;
float H_S=0, H_I=0;
float Rpress;
unsigned int C0_S, C1_S, C2_S, C3_S, C6_S, C8_S, C9_S, C10_S, C11_S, C12_S; 
unsigned long C4_S, C5_S, C7_S;
unsigned int C0_I, C1_I, C2_I, C3_I, C6_I, C8_I, C9_I, C10_I, C11_I, C12_I; 
unsigned long C4_I, C5_I, C7_I;
unsigned char Formula_Select=1;

/*********************************************************************************************
* 名称：fbm320_write_reg()
* 功能：向fbm320设备寄存器中写数据
* 参数： *bus -> I2C设备对象句柄
*         reg -> 寄存器
*       *data -> 数据
*         len -> 数据长度
* 返回：是否写入成功
* 修改：
* 注释：
*********************************************************************************************/
static rt_err_t fbm320_write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data, rt_uint8_t len)
{
  rt_uint8_t buf[10] = {0};
  struct rt_i2c_msg msgs;
  buf[0] = reg;
  for(unsigned char i=0; i<len; i++)
    buf[i+1] = data[i];
  msgs.addr = FBM320_ADDR;
  msgs.flags = RT_I2C_WR;
  msgs.buf = buf;
  msgs.len = len + 1;
  if(rt_i2c_transfer(bus, &msgs, 1) == 1)
    return RT_EOK;
  else
    return -RT_ERROR;
}

/*********************************************************************************************
* 名称：fbm320_read_reg()
* 功能：读取fbm320设备寄存器数据
* 参数： *bus -> I2C设备对象句柄
*        addr -> 寄存器地址
*         len -> 读取数据长度
*        *buf -> 读取数据
* 返回：是否读取成功
* 修改：
* 注释：
*********************************************************************************************/
static rt_err_t fbm320_read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t addr, rt_uint8_t len, rt_uint8_t *buf)
{
  struct rt_i2c_msg msgs[2];
  msgs[0].addr = FBM320_ADDR;
  msgs[0].flags = RT_I2C_WR;
  msgs[0].buf = &addr;
  msgs[0].len = 1;
  
  msgs[1].addr = FBM320_ADDR;
  msgs[1].flags = RT_I2C_RD;
  msgs[1].buf = buf;
  msgs[1].len = len;
  if(rt_i2c_transfer(bus, msgs, 2) == 2)
    return RT_EOK;
  else
    return -RT_ERROR;
}


void Coefficient(void)                                                                                                        //Receive Calibrate Coefficient
{
  unsigned int R[10];
  unsigned int C0=0, C1=0, C2=0, C3=0, C6=0, C8=0, C9=0, C10=0, C11=0, C12=0; 
  unsigned long C4=0, C5=0, C7=0;
  
  unsigned char readData[25] = {0};
  fbm320_read_reg(fbm320_bus, 0xAA, 18, readData);
  for(unsigned char i=0; i<9; i++)
    R[i] = (unsigned int)((readData[i*2] << 8) | readData[i*2+1]);
  fbm320_read_reg(fbm320_bus, 0xA4, 1, readData);
  R[9] = (unsigned int)(readData[0]<<8);
  fbm320_read_reg(fbm320_bus, 0xF1, 1, readData);
  R[9] |= readData[0];
  if(((Formula_Select & 0xF0) == 0x10) || ((Formula_Select & 0x0F) == 0x01))
  {
    C0 = R[0] >> 4;
    C1 = ((R[1] & 0xFF00) >> 5) | (R[2] & 7);
    C2 = ((R[1] & 0xFF) << 1) | (R[4] & 1);
    C3 = R[2] >> 3;
    C4 = ((unsigned long)R[3] << 2) | (R[0] & 3);
    C5 = R[4] >> 1;
    C6 = R[5] >> 3;
    C7 = ((unsigned long)R[6] << 3) | (R[5] & 7);
    C8 = R[7] >> 3;
    C9 = R[8] >> 2;
    C10 = ((R[9] & 0xFF00) >> 6) | (R[8] & 3);
    C11 = R[9] & 0xFF;
    C12 = ((R[0] & 0x0C) << 1) | (R[7] & 7);
  }
  else
  {
    C0 = R[0] >> 4;
    C1 = ((R[1] & 0xFF00) >> 5) | (R[2] & 7);
    C2 = ((R[1] & 0xFF) << 1) | (R[4] & 1);
    C3 = R[2] >> 3;
    C4 = ((unsigned long)R[3] << 1) | (R[5] & 1);
    C5 = R[4] >> 1;
    C6 = R[5] >> 3;
    C7 = ((unsigned long)R[6] << 2) | ((R[0] >> 2) & 3);
    C8 = R[7] >> 3;
    C9 = R[8] >> 2;
    C10 = ((R[9] & 0xFF00) >> 6) | (R[8] & 3);
    C11 = R[9] & 0xFF;
    C12 = ((R[5] & 6) << 2) | (R[7] & 7);
  }
  C0_I = C0;
  C1_I = C1;
  C2_I = C2;
  C3_I = C3;
  C4_I = C4;
  C5_I = C5;
  C6_I = C6;
  C7_I = C7;
  C8_I = C8;
  C9_I = C9;
  C10_I = C10;
  C11_I = C11;
  C12_I = C12;        
}

/*********************************************************************************************
* 名称：Calculate()
* 功能：大气压力换算
* 参数：UP -> 气压数据 UT -> 温度数据
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void Calculate(long UP, long UT)                                //Calculate Real Pressure & Temperautre
{
  signed char C12=0;
  int C0=0, C2=0, C3=0, C6=0, C8=0, C9=0, C10=0, C11=0; 
  long C1=0, C4=0, C5=0, C7=0;                                  //long C0=0, C2=0, C3=0, C6=0, C8=0, C9=0, C10=0, C11=0;
  long RP=0, RT=0;
  long DT, DT2, X01, X02, X03, X11, X12, X13, X21, X22, X23, X24, X25, X26, X31, X32, CF, PP1, PP2, PP3, PP4;
  C0 = C0_I;
  C1 = C1_I;
  C2 = C2_I;
  C3 = C3_I;
  C4 = C4_I;
  C5 = C5_I;
  C6 = C6_I;
  C7 = C7_I;
  C8 = C8_I;
  C9 = C9_I;
  C10 = C10_I;
  C11 = C11_I;
  C12 = C12_I;
  //For FBM320-02
  if(((Formula_Select & 0xF0) == 0x10) || ((Formula_Select & 0x0F) == 0x01)) 
  {
    DT        =        ((UT - 8388608) >> 4) + (C0 << 4);
    X01        =        (C1 + 4459) * DT >> 1;
    X02        =        ((((C2 - 256) * DT) >> 14) * DT) >> 4;
    X03        =        (((((C3 * DT) >> 18) * DT) >> 18) * DT);
    RT        =        (((long)2500 << 15) - X01 - X02 - X03) >> 15;
    
    DT2        =        (X01 + X02 + X03) >> 12;
    
    X11        =        ((C5 - 4443) * DT2);
    X12        =        (((C6 * DT2) >> 16) * DT2) >> 2;
    X13        =        ((X11 + X12) >> 10) + ((C4 + 120586) << 4);
    
    X21        =        ((C8 + 7180) * DT2) >> 10;
    X22        =        (((C9 * DT2) >> 17) * DT2) >> 12;
    if(X22 >= X21)
      X23        =        X22 - X21;
    else
      X23        =        X21 - X22;
    X24        =        (X23 >> 11) * (C7 + 166426);
    X25        =        ((X23 & 0x7FF) * (C7 + 166426)) >> 11;
    if((X22 - X21) < 0)
      X26        =        ((0 - X24 - X25) >> 11) + C7 + 166426;
    else        
      X26        =        ((X24 + X25) >> 11) + C7 + 166426;
    
    PP1        =        ((UP - 8388608) - X13) >> 3;
    PP2        =        (X26 >> 11) * PP1;
    PP3        =        ((X26 & 0x7FF) * PP1) >> 11;
    PP4        =        (PP2 + PP3) >> 10;
    
    CF        =        (2097152 + C12 * DT2) >> 3;
    X31        =        (((CF * C10) >> 17) * PP4) >> 2;
    X32        =        (((((CF * C11) >> 15) * PP4) >> 18) * PP4);
    RP        =        ((X31 + X32) >> 15) + PP4 + 99880;
  }
  else                                                                                                                                                                                                                                                                                                                //For FBM320
  {
    DT        =        ((UT - 8388608) >> 4) + (C0 << 4);
    X01        =        (C1 + 4418) * DT >> 1;
    X02        =        ((((C2 - 256) * DT) >> 14) * DT) >> 4;
    X03        =        (((((C3 * DT) >> 18) * DT) >> 18) * DT);
    RT = (((long)2500 << 15) - X01 - X02 - X03) >> 15;
    
    DT2        =        (X01 + X02 + X03) >>12;
    
    X11        =        (C5 * DT2);
    X12        =        (((C6 * DT2) >> 16) * DT2) >> 2;
    X13        =        ((X11 + X12) >> 10) + ((C4 + 211288) << 4);
    
    X21        =        ((C8 + 7209) * DT2) >> 10;
    X22        =        (((C9 * DT2) >> 17) * DT2) >> 12;
    if(X22 >= X21)
      X23        =        X22 - X21;
    else
      X23        =        X21 - X22;
    X24        =        (X23 >> 11) * (C7 + 285594);
    X25        =        ((X23 & 0x7FF) * (C7 + 285594)) >> 11;
    if((X22 - X21) < 0) 
      X26        =        ((0 - X24 - X25) >> 11) + C7 + 285594;
    else
      X26        =        ((X24 + X25) >> 11) + C7 + 285594;
    PP1        =        ((UP - 8388608) - X13) >> 3;
    PP2        =        (X26 >> 11) * PP1;
    PP3        =        ((X26 & 0x7FF) * PP1) >> 11;
    PP4        =        (PP2 + PP3) >> 10;
    
    CF        =        (2097152 + C12 * DT2) >> 3;
    X31        =        (((CF * C10) >> 17) * PP4) >> 2;
    X32        =        (((((CF * C11) >> 15) * PP4) >> 18) * PP4);
    RP = ((X31 + X32) >> 15) + PP4 + 99880;
  }
  
  RP_I = RP;
  RT_I = RT;
}

/*********************************************************************************************
* 名称：fbm320_read_data()
* 功能：读取fbm320数据
* 参数：无
* 返回：加速度数据
* 修改：
* 注释：
*********************************************************************************************/
unsigned char fbm320_read_data(float *temperature, float *pressure)
{
  unsigned char readData[10]= {0};
  unsigned char cmd = TEMPERATURE;
  fbm320_write_reg(fbm320_bus, FBM320_CONFIG, &cmd, 1);         //发送识别信息
  rt_thread_mdelay(5);                                          //延时5ms
  fbm320_read_reg(fbm320_bus, FBM320_DATAM, 3, readData);
  UT_I = (((long)readData[0] << 16) | ((long)readData[1] << 8) | readData[2]);
  cmd = OSR8192;
  fbm320_write_reg(fbm320_bus, FBM320_CONFIG, &cmd, 1);         //发送识别信息
  rt_thread_mdelay(10);                                         //延时10ms
  fbm320_read_reg(fbm320_bus, FBM320_DATAM, 3, readData);
  UP_I = (((long)readData[0] << 16) | ((long)readData[1] << 8) | readData[2]);
  if (UT_I == -1 || UP_I == -1){
    return -1;
  }
  Calculate(UP_I, UT_I);                                        //传感器数值换算
  *temperature = RT_I * 0.01f;                                  //温度计算
  *pressure = RP_I;                                             //压力计算
  return 0;
}

/*********************************************************************************************
* 名称：fbm320_init()
* 功能：LIS3DH初始化
* 参数：无
* 返回：初始化是否成功
* 修改：
* 注释：
*********************************************************************************************/
int fbm320_init(void)
{
  fbm320_bus = (struct rt_i2c_bus_device *)rt_device_find(FBM320_I2C_BUS_NAME);
  if(fbm320_bus == RT_NULL)
  {
    rt_kprintf("failed to find %s device!\n", FBM320_I2C_BUS_NAME);
    return -1;
  }
  unsigned char id = 0;
  fbm320_read_reg(fbm320_bus, FBM320_ID_ADDR, 1, &id);          // 读取FBM320 ID
  if(FBM320_ID != id)                                           // 对比ID
    return 1;

  Coefficient();
  float t1=0, p1=0;
  fbm320_read_data(&t1, &p1);

  Formula_Select = 0;
  float t0=0, p0=0;
  fbm320_read_data(&t0, &p0);
  if (abs(((int)p1) - 100000) < abs(((int)p0)-100000)) {
    Formula_Select = 1;
  }
  return 0;
}

#endif
