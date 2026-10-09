CUBE := $(HOME)/.local/code_libraries/stm32/STM32CubeF1
HAL  := $(CUBE)/Drivers/STM32F1xx_HAL_Driver
DEV  := $(CUBE)/Drivers/CMSIS/Device/ST/STM32F1xx
CPU  := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CPU) -Og -g -Wall -ffunction-sections -fdata-sections -MMD \
  -DSTM32F103xB -DUSE_HAL_DRIVER -Iinc -I$(HAL)/Inc -I$(DEV)/Include -I$(CUBE)/Drivers/CMSIS/Include

# для відладки по uart замінити -specs=rdimon... на -specs=nosys.specs
LDFLAGS := $(CPU) -T stm32f103c8.ld -specs=nano.specs -specs=rdimon.specs -Wl,--gc-sections

SRCS := $(wildcard src/*.c) $(DEV)/Source/Templates/system_stm32f1xx.c \
        $(filter-out %_template.c,$(wildcard $(HAL)/Src/stm32f1xx_hal*.c))
OBJS := $(addprefix build/,$(notdir $(SRCS:.c=.o))) build/startup.o
vpath %.c src $(HAL)/Src $(DEV)/Source/Templates

all: build/firmware.elf
build/firmware.elf: $(OBJS)
	arm-none-eabi-gcc $(LDFLAGS) $^ -o $@
	arm-none-eabi-size $@
build/%.o: %.c | build
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@
build/startup.o: $(DEV)/Source/Templates/gcc/startup_stm32f103xb.s | build
	arm-none-eabi-gcc $(CPU) -c $< -o $@
build:
	mkdir -p build
flash: build/firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"
clean:
	rm -rf build
-include build/*.d
.PHONY: all flash clean
