################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../packages/kawaii-mqtt-latest/mqtt/MQTTConnectClient.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTConnectServer.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTDeserializePublish.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTFormat.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTPacket.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTSerializePublish.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTSubscribeClient.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTSubscribeServer.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTUnsubscribeClient.c \
../packages/kawaii-mqtt-latest/mqtt/MQTTUnsubscribeServer.c 

OBJS += \
./packages/kawaii-mqtt-latest/mqtt/MQTTConnectClient.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTConnectServer.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTDeserializePublish.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTFormat.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTPacket.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTSerializePublish.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTSubscribeClient.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTSubscribeServer.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTUnsubscribeClient.o \
./packages/kawaii-mqtt-latest/mqtt/MQTTUnsubscribeServer.o 

C_DEPS += \
./packages/kawaii-mqtt-latest/mqtt/MQTTConnectClient.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTConnectServer.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTDeserializePublish.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTFormat.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTPacket.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTSerializePublish.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTSubscribeClient.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTSubscribeServer.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTUnsubscribeClient.d \
./packages/kawaii-mqtt-latest/mqtt/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
packages/kawaii-mqtt-latest/mqtt/%.o: ../packages/kawaii-mqtt-latest/mqtt/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F4 -DUSE_HAL_DRIVER -DSTM32F407xx -I"D:\RT-ThreadStudio\workspace\electronic\drivers" -I"D:\RT-ThreadStudio\workspace\electronic\drivers\include" -I"D:\RT-ThreadStudio\workspace\electronic\drivers\include\config" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\CMSIS\RTOS\Template" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\electronic\libraries\STM32F4xx_HAL_Driver\Inc\Legacy" -I"D:\RT-ThreadStudio\workspace\electronic" -I"D:\RT-ThreadStudio\workspace\electronic\applications" -I"D:\RT-ThreadStudio\workspace\electronic" -I"D:\RT-ThreadStudio\workspace\electronic\packages\at_device-latest\class\esp8266" -I"D:\RT-ThreadStudio\workspace\electronic\packages\at_device-latest\inc" -I"D:\RT-ThreadStudio\workspace\electronic\packages\bh1750-latest" -I"D:\RT-ThreadStudio\workspace\electronic\packages\cJSON-latest" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\common\log" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\common" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\mqttclient" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\mqtt" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\network" -I"D:\RT-ThreadStudio\workspace\electronic\packages\kawaii-mqtt-latest\platform\RT-Thread" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\drivers\sensors" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\compilers\common\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\compilers\newlib" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\posix\io\poll" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\posix\io\stdio" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\libc\posix\ipc" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\at\at_socket" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\at\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\lwip\lwip-2.0.3\src\include\netif" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\lwip\lwip-2.0.3\src\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\lwip\port" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\impl" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\include\socket\sys_socket" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\include\socket" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\components\net\sal\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\electronic\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\electronic\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

