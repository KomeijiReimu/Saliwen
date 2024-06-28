################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include rt-thread/src/subdir.mk
-include rt-thread/libcpu/arm/cortex-m4/subdir.mk
-include rt-thread/libcpu/arm/common/subdir.mk
-include rt-thread/components/net/sal/src/subdir.mk
-include rt-thread/components/net/sal/socket/subdir.mk
-include rt-thread/components/net/sal/impl/subdir.mk
-include rt-thread/components/net/netdev/src/subdir.mk
-include rt-thread/components/net/lwip/port/subdir.mk
-include rt-thread/components/net/lwip/lwip-2.0.3/src/netif/subdir.mk
-include rt-thread/components/net/lwip/lwip-2.0.3/src/core/ipv4/subdir.mk
-include rt-thread/components/net/lwip/lwip-2.0.3/src/core/subdir.mk
-include rt-thread/components/net/lwip/lwip-2.0.3/src/apps/ping/subdir.mk
-include rt-thread/components/net/lwip/lwip-2.0.3/src/api/subdir.mk
-include rt-thread/components/net/at/src/subdir.mk
-include rt-thread/components/net/at/at_socket/subdir.mk
-include rt-thread/components/libc/compilers/newlib/subdir.mk
-include rt-thread/components/libc/compilers/common/subdir.mk
-include rt-thread/components/finsh/subdir.mk
-include rt-thread/components/drivers/serial/subdir.mk
-include rt-thread/components/drivers/sensors/subdir.mk
-include rt-thread/components/drivers/misc/subdir.mk
-include rt-thread/components/drivers/ipc/subdir.mk
-include rt-thread/components/drivers/i2c/subdir.mk
-include rt-thread/components/drivers/hwtimer/subdir.mk
-include packages/rt_vsnprintf_full-latest/subdir.mk
-include packages/kawaii-mqtt-latest/platform/RT-Thread/subdir.mk
-include packages/kawaii-mqtt-latest/network/subdir.mk
-include packages/kawaii-mqtt-latest/mqttclient/subdir.mk
-include packages/kawaii-mqtt-latest/mqtt/subdir.mk
-include packages/kawaii-mqtt-latest/common/log/arch/rt-thread/subdir.mk
-include packages/kawaii-mqtt-latest/common/log/subdir.mk
-include packages/kawaii-mqtt-latest/common/subdir.mk
-include packages/cJSON-latest/subdir.mk
-include packages/bh1750-latest/subdir.mk
-include packages/at_device-latest/src/subdir.mk
-include packages/at_device-latest/samples/subdir.mk
-include packages/at_device-latest/class/esp8266/subdir.mk
-include libraries/STM32F4xx_HAL_Driver/Src/Legacy/subdir.mk
-include libraries/STM32F4xx_HAL_Driver/Src/subdir.mk
-include libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/subdir.mk
-include libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/subdir.mk
-include drivers/subdir.mk
-include applications/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
rtthread.bin \

SECONDARY_SIZE += \
rtthread.siz \


# 所有目标
all: rtthread.elf secondary-outputs

# 工具调用
rtthread.elf: $(OBJS) $(USER_OBJS)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -T "D:\RT-ThreadStudio\workspace\electronic\linkscripts\STM32F407VE\link.lds" -Xlinker --gc-sections -Wl,-Map,"rtthread.map" -Xlinker --cref -o "rtthread.elf" $(OBJS) $(USER_OBJS) $(LIBS)

rtthread.bin: rtthread.elf
	arm-none-eabi-objcopy -O binary "rtthread.elf"  "rtthread.bin"

rtthread.siz: rtthread.elf
	arm-none-eabi-size --format=berkeley "rtthread.elf"

# 其他目标
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) rtthread.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
