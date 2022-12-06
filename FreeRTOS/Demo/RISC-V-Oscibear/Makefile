CROSS   = riscv64-unknown-elf-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
ARCH    = $(CROSS)ar

BUILD_DIR       = build
RTOS_SOURCE_DIR = $(abspath ../../Source)
DEMO_SOURCE_DIR = $(abspath ../Common/Minimal)
HAL_DIR  		= $(abspath ./HAL/firmware/bsp/oscibear)
HAL_SRC_DIR  	= $(HAL_DIR)/src

CPPFLAGS = \
	-D__riscv_float_abi_hard \
	-DportasmHANDLE_INTERRUPT=handle_trap \
	-I . -I ../Common/include \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions \
	-I $(HAL_DIR)/inc
CFLAGS  = -march=rv32imafc -mabi=ilp32f -mcmodel=medany \
	-Wall \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fno-builtin-printf \
	-I $(HAL_DIR)/inc

ASFLAGS = -march=rv32imafc -mabi=ilp32f -mcmodel=medany
LDFLAGS = -march=rv32imafc -mabi=ilp32f -nostartfiles -Tee290c_oscibear.ld \
	-Xlinker --gc-sections \
	-Xlinker --defsym=__stack_size=300 \
	-flto


ifeq ($(DEBUG), 1)
    CFLAGS += -Og -ggdb3
else
    CFLAGS += -Os -flto
endif

SRCS = main.c main_blinky.c printf-stdarg.c \
	platform/ee290c_oscibear.c \
	$(HAL_SRC_DIR)/oscibear_hal.c \
	$(HAL_SRC_DIR)/oscibear_hal_uart.c \
	$(HAL_SRC_DIR)/oscibear_hal_plic.c \
	$(DEMO_SOURCE_DIR)/EventGroupsDemo.c \
	$(DEMO_SOURCE_DIR)/TaskNotify.c \
	$(DEMO_SOURCE_DIR)/TimerDemo.c \
	$(DEMO_SOURCE_DIR)/blocktim.c \
	$(DEMO_SOURCE_DIR)/dynamic.c \
	$(DEMO_SOURCE_DIR)/recmutex.c \
	$(RTOS_SOURCE_DIR)/event_groups.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c \
	$(RTOS_SOURCE_DIR)/portable/MemMang/heap_4.c \
	$(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/port.c

ASMS = start.S \
	$(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/portASM.S

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(ASMS:%.S=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%.d) $(ASMS:%.S=$(BUILD_DIR)/%.d)

$(BUILD_DIR)/RTOSDemo.img: $(BUILD_DIR)/RTOSDemo.axf
	$(OBJCOPY) -O binary $(BUILD_DIR)/RTOSDemo.axf $(BUILD_DIR)/RTOSDemo.img

$(BUILD_DIR)/RTOSDemo.axf: $(OBJS) ee290c_oscibear.ld Makefile
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(ASFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
