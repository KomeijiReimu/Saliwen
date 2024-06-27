################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/def.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/dns.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/inet_chksum.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/init.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/ip.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/memp.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/netif.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/pbuf.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/raw.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/stats.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/sys.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp_in.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp_out.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/timeouts.c \
../rt-thread/components/net/lwip/lwip-2.0.3/src/core/udp.c 

OBJS += \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/def.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/dns.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/inet_chksum.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/init.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/ip.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/memp.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/netif.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/pbuf.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/raw.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/stats.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/sys.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp_in.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp_out.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/timeouts.o \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/udp.o 

C_DEPS += \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/def.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/dns.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/inet_chksum.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/init.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/ip.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/memp.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/netif.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/pbuf.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/raw.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/stats.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/sys.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp_in.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/tcp_out.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/timeouts.d \
./rt-thread/components/net/lwip/lwip-2.0.3/src/core/udp.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/net/lwip/lwip-2.0.3/src/core/%.o: ../rt-thread/components/net/lwip/lwip-2.0.3/src/core/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F4 -DUSE_HAL_DRIVER -DSTM32F407xx -I"D:\RT-ThreadStudio\workspace\electronic\drivers" -I"D:\RT-ThreadStudio\workspace\electronic\drivers\include" -I"D:\RT-ThreadStudio\workspace\electronic\drivers\include\config" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\CMSIS\RTOS\Template" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\STM32F4xx_HAL_Driver\Inc\Legacy" -I"D:\RT-ThreadStudio\workspace\electronic" -I"D:\RT-ThreadStudio\workspace\electronic\applications" -I"D:\RT-ThreadStudio\workspace\electronic" -I"D:\RT-ThreadStudio\workspace\electronic\packages\at_device-latest\class\esp8266" -I"D:\RT-ThreadStudio\workspace\electronic\packages\at_device-latest\inc" -I"D:\RT-ThreadStudio\workspace\electronic\packages\bh1750-latest" -I"D:\RT-ThreadStudio\workspace\electronic\packages\cJSON-latest" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\common\log" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\common" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\mqttclient" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\mqtt" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\network" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\platform\RT-Thread" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\drivers\sensors" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\compilers\common\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\compilers\newlib" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\posix\io\poll" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\posix\io\stdio" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\posix\ipc" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\at\at_socket" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\at\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\lwip\lwip-2.0.3\src\include\netif" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\lwip\lwip-2.0.3\src\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\lwip\port" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\impl" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\include\socket\sys_socket" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\include\socket" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\electronic\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

