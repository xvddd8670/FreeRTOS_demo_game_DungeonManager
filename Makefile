# ---------- libs path ----------
CUBE     := $(HOME)/.local/code_libraries/stm32/STM32CubeF1
FREERTOS := $(HOME)/.local/code_libraries/stm32/FreeRTOS-Kernel
RTT      := $(HOME)/.local/code_libraries/stm32/RTT

HAL      := $(CUBE)/Drivers/STM32F1xx_HAL_Driver
DEV      := $(CUBE)/Drivers/CMSIS/Device/ST/STM32F1xx
RTOS_PORT := $(FREERTOS)/portable/GCC/ARM_CM3
RTOS_HEAP := $(FREERTOS)/portable/MemMang

# ---------- flags ----------
CPU    := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CPU) -Og -g -Wall -ffunction-sections -fdata-sections -MMD \
  -DSTM32F103xB -DUSE_HAL_DRIVER \
  -Iinc -I$(HAL)/Inc -I$(DEV)/Include -I$(CUBE)/Drivers/CMSIS/Include \
  -I$(FREERTOS)/include -I$(RTOS_PORT) \
  -I$(RTT)/RTT -I$(RTT)/Config

# для відладки по uart замінити -specs=rdimon... на -specs=nosys.specs
LDFLAGS := $(CPU) -T stm32f103c8.ld -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections

# ---------- sources ----------
PROJ_SRCS := $(shell find src -name '*.c')

LIB_SRCS := $(DEV)/Source/Templates/system_stm32f1xx.c \
            $(filter-out %_template.c,$(wildcard $(HAL)/Src/stm32f1xx_hal*.c)) \
            $(FREERTOS)/tasks.c $(FREERTOS)/queue.c $(FREERTOS)/list.c \
            $(RTOS_PORT)/port.c $(RTOS_HEAP)/heap_4.c \
            $(RTT)/RTT/SEGGER_RTT.c

OBJS := $(PROJ_SRCS:%.c=build/%.o) \
        $(addprefix build/lib/,$(notdir $(LIB_SRCS:.c=.o))) \
        build/startup.o

vpath %.c $(HAL)/Src $(DEV)/Source/Templates $(FREERTOS) $(RTOS_PORT) $(RTOS_HEAP) $(RTT)/RTT

# ---------- rules ----------
all: build/firmware.elf

build/firmware.elf: $(OBJS)
	arm-none-eabi-gcc $(LDFLAGS) $^ -o $@
	arm-none-eabi-size $@

build/src/%.o: src/%.c
	@mkdir -p $(@D)
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

build/lib/%.o: %.c
	@mkdir -p $(@D)
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

build/startup.o: $(DEV)/Source/Templates/gcc/startup_stm32f103xb.s
	@mkdir -p $(@D)
	arm-none-eabi-gcc $(CPU) -c $< -o $@

flash: build/firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"

flash-live: build/firmware.elf
	echo "rtt stop; program $< verify reset; rtt start" | nc -q1 localhost 4444

rtt:
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
	-c "init" -c "rtt setup 0x20000000 0x5000 \"SEGGER RTT\"" \
	-c "rtt start" -c "rtt server start 9090 0"

clean:
	rm -rf build

-include $(OBJS:.o=.d)
.PHONY: all flash rtt clean flash-live
