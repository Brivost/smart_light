###############################################################################
#
# Makefile for SparkFun Artemis ATP Firmware
#
###############################################################################

# Project name
PROJECT = firmware

# Source directories
SRCDIR = src
BUILDDIR = build

# SDK path
SDK_ROOT = AmbiqSuiteSDK

# Target chip
PART = apollo3
CPU = cortex-m4
FPU = fpv4-sp-d16

# Toolchain
TOOLCHAIN = arm-none-eabi
CC = $(TOOLCHAIN)-gcc
LD = $(TOOLCHAIN)-ld
AR = $(TOOLCHAIN)-ar
AS = $(TOOLCHAIN)-as
OBJCOPY = $(TOOLCHAIN)-objcopy
OBJDUMP = $(TOOLCHAIN)-objdump
SIZE = $(TOOLCHAIN)-size

# Include paths
INCLUDES = -I$(SRCDIR)
INCLUDES += -I$(SDK_ROOT)/mcu/$(PART)
INCLUDES += -I$(SDK_ROOT)/mcu/$(PART)/hal
INCLUDES += -I$(SDK_ROOT)/CMSIS/AmbiqMicro/Include
INCLUDES += -I$(SDK_ROOT)/CMSIS/ARM/Include
INCLUDES += -I$(SDK_ROOT)/devices
INCLUDES += -I$(SDK_ROOT)/utils

# Defines
DEFINES = -DAM_PART_APOLLO3
DEFINES += -DAM_PACKAGE_BGA
DEFINES += -Dgcc

# Compiler flags
CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=hard
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wextra -Wno-unused-parameter
CFLAGS += -std=c99 -O0 -g
CFLAGS += $(INCLUDES) $(DEFINES)

# Linker flags
LDFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=hard
LDFLAGS += -nostartfiles -static
LDFLAGS += -Wl,--gc-sections,--entry,Reset_Handler
LDFLAGS += -Wl,-Map=$(BUILDDIR)/$(PROJECT).map
LDFLAGS += -T linker/artemis_atp.ld

# Source files
CSRCS = $(wildcard $(SRCDIR)/*.c)

# HAL sources from SDK
HAL_DIR = $(SDK_ROOT)/mcu/$(PART)/hal
HAL_SRCS = $(HAL_DIR)/am_hal_adc.c
HAL_SRCS += $(HAL_DIR)/am_hal_ble.c
HAL_SRCS += $(HAL_DIR)/am_hal_ble_patch.c
HAL_SRCS += $(HAL_DIR)/am_hal_burst.c
HAL_SRCS += $(HAL_DIR)/am_hal_cachectrl.c
HAL_SRCS += $(HAL_DIR)/am_hal_clkgen.c
HAL_SRCS += $(HAL_DIR)/am_hal_cmdq.c
HAL_SRCS += $(HAL_DIR)/am_hal_ctimer.c
HAL_SRCS += $(HAL_DIR)/am_hal_debug.c
HAL_SRCS += $(HAL_DIR)/am_hal_flash.c
HAL_SRCS += $(HAL_DIR)/am_hal_global.c
HAL_SRCS += $(HAL_DIR)/am_hal_gpio.c
HAL_SRCS += $(HAL_DIR)/am_hal_interrupt.c
HAL_SRCS += $(HAL_DIR)/am_hal_iom.c
HAL_SRCS += $(HAL_DIR)/am_hal_ios.c
HAL_SRCS += $(HAL_DIR)/am_hal_itm.c
HAL_SRCS += $(HAL_DIR)/am_hal_mcuctrl.c
HAL_SRCS += $(HAL_DIR)/am_hal_mspi.c
HAL_SRCS += $(HAL_DIR)/am_hal_pdm.c
HAL_SRCS += $(HAL_DIR)/am_hal_pwrctrl.c
HAL_SRCS += $(HAL_DIR)/am_hal_queue.c
HAL_SRCS += $(HAL_DIR)/am_hal_reset.c
HAL_SRCS += $(HAL_DIR)/am_hal_rtc.c
HAL_SRCS += $(HAL_DIR)/am_hal_scard.c
HAL_SRCS += $(HAL_DIR)/am_hal_secure_ota.c
HAL_SRCS += $(HAL_DIR)/am_hal_stimer.c
HAL_SRCS += $(HAL_DIR)/am_hal_sysctrl.c
HAL_SRCS += $(HAL_DIR)/am_hal_systick.c
HAL_SRCS += $(HAL_DIR)/am_hal_tpiu.c
HAL_SRCS += $(HAL_DIR)/am_hal_uart.c
HAL_SRCS += $(HAL_DIR)/am_hal_wdt.c

# Utils from SDK
UTILS_DIR = $(SDK_ROOT)/utils
UTILS_SRCS = $(UTILS_DIR)/am_util_delay.c
UTILS_SRCS += $(UTILS_DIR)/am_util_stdio.c

# Startup code
STARTUP_SRC = $(SDK_ROOT)/mcu/$(PART)/hal/gcc/startup_gcc.c

# All sources
ALL_SRCS = $(CSRCS) $(HAL_SRCS) $(UTILS_SRCS) $(STARTUP_SRC)

# Object files
OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(CSRCS)))
OBJS += $(patsubst %.c,$(BUILDDIR)/hal_%.o,$(notdir $(HAL_SRCS)))
OBJS += $(patsubst %.c,$(BUILDDIR)/util_%.o,$(notdir $(UTILS_SRCS)))
OBJS += $(BUILDDIR)/startup_gcc.o

# Default target
all: $(BUILDDIR)/$(PROJECT).bin

# Create build directory
$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile HAL files
$(BUILDDIR)/hal_%.o: $(HAL_DIR)/%.c | $(BUILDDIR)
	@echo "Compiling HAL: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile Utils files
$(BUILDDIR)/util_%.o: $(UTILS_DIR)/%.c | $(BUILDDIR)
	@echo "Compiling Utils: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile startup file
$(BUILDDIR)/startup_gcc.o: $(STARTUP_SRC) | $(BUILDDIR)
	@echo "Compiling startup code"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BUILDDIR)/$(PROJECT).axf: $(OBJS)
	@echo "Linking..."
	@$(CC) $(LDFLAGS) -o $@ $(OBJS)
	@$(SIZE) $@

# Create binary
$(BUILDDIR)/$(PROJECT).bin: $(BUILDDIR)/$(PROJECT).axf
	@echo "Creating binary..."
	@$(OBJCOPY) -O binary $< $@
	@echo "Build complete!"

# Flash target
flash: $(BUILDDIR)/$(PROJECT).bin
	@echo "Flashing firmware..."
	@uv run python tools/artemis_svl.py -f $(BUILDDIR)/$(PROJECT).bin -b 921600

# Clean
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILDDIR)

.PHONY: all clean flash

