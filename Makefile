#******************************************************************************
#
# Makefile - Build script for smart_light project
#
#******************************************************************************

#******************************************************************************
# User Configuration
#******************************************************************************

# Board Configuration
BOARD := redboard_artemis_atp

# Paths (using relative paths to SDK)
SDKPATH := external/AmbiqSuiteSDK
BOARDPATH := $(SDKPATH)/boards_sfe/$(BOARD)
COMMONPATH := $(SDKPATH)/boards_sfe/common

# Upload Configuration (override on command line: make bootload COM_PORT=/dev/cu.usbserial-XXX)
COM_PORT ?= /dev/cu.usbserial-110
SVL_UPLOAD_BAUD ?= 115200
PYTHON3 ?= python3

# Project Settings
TARGET := smart_light
PROJECT := smart_light_gcc
COMPILERNAME := gcc

#******************************************************************************
# Build Configuration
#******************************************************************************

# Output directory
CONFIG := build

# Toolchain
TOOLCHAIN := arm-none-eabi
PART := apollo3
CPU := cortex-m4
FPU := fpv4-sp-d16
FABI := hard

# Global Defines
DEFINES := -DPART_apollo3
DEFINES += -DAM_PACKAGE_BGA
DEFINES += -DAM_PART_APOLLO3

# Include Paths
INCLUDES := -Isrc
INCLUDES += -I$(BOARDPATH)/bsp
INCLUDES += -I$(SDKPATH)
INCLUDES += -I$(SDKPATH)/utils
INCLUDES += -I$(SDKPATH)/devices
INCLUDES += -I$(SDKPATH)/mcu/apollo3
INCLUDES += -I$(SDKPATH)/CMSIS/AmbiqMicro/Include
INCLUDES += -I$(SDKPATH)/CMSIS/ARM/Include

# Source Files
SRC := main.c
SRC += startup_gcc.c
SRC += am_util_delay.c
SRC += am_util_faultisr.c
SRC += am_util_stdio.c
SRC += am_devices_led.c

# Source Paths (VPATH tells make where to find source files)
VPATH := src
VPATH += $(SDKPATH)/utils
VPATH += $(SDKPATH)/devices
VPATH += $(COMMONPATH)/tools_sfe/templates

# Precompiled Libraries
LIBS := $(BOARDPATH)/bsp/gcc/bin/libam_bsp.a
LIBS += $(SDKPATH)/mcu/apollo3/hal/gcc/bin/libam_hal.a

#******************************************************************************
# Compiler and Linker Configuration
#******************************************************************************

# Executables
CC := $(TOOLCHAIN)-gcc
GCC := $(TOOLCHAIN)-gcc
CPP := $(TOOLCHAIN)-cpp
LD := $(TOOLCHAIN)-ld
CP := $(TOOLCHAIN)-objcopy
OD := $(TOOLCHAIN)-objdump
AR := $(TOOLCHAIN)-ar
SIZE := $(TOOLCHAIN)-size
RM := rm -f

# Compiler Flags
CFLAGS := -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -MMD -MP -std=c99 -Wall
CFLAGS += -g -O3
CFLAGS += $(DEFINES)
CFLAGS += $(INCLUDES)

# Linker Script
LINKER_SCRIPT := $(COMMONPATH)/tools_sfe/templates/asb_svl_linker.ld

# Linker Flags
LFLAGS := -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS += -nostartfiles -static
LFLAGS += -Wl,--gc-sections,--entry,Reset_Handler
LFLAGS += -Wl,--start-group -lm -lc -lgcc $(LIBS) -Wl,--end-group
LFLAGS += -Wl,-Map,$(CONFIG)/$(TARGET).map

#******************************************************************************
# Build Rules
#******************************************************************************

# Object files
CSRC := $(filter %.c,$(SRC))
OBJS := $(CSRC:%.c=$(CONFIG)/%.o)
DEPS := $(CSRC:%.c=$(CONFIG)/%.d)

# Default target
all: directories $(CONFIG)/$(TARGET).bin
	@echo "Build complete: $(CONFIG)/$(TARGET).bin"
	@$(SIZE) $(CONFIG)/$(TARGET).axf

# Create output directories
directories:
	@mkdir -p $(CONFIG)

# Compile C files
$(CONFIG)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) -c $(CFLAGS) -o $@ $<

# Link
$(CONFIG)/$(TARGET).axf: $(OBJS)
	@echo "Linking..."
	@$(CC) -T $(LINKER_SCRIPT) -o $@ $(OBJS) $(LFLAGS)

# Create binary
$(CONFIG)/$(TARGET).bin: $(CONFIG)/$(TARGET).axf
	@echo "Creating binary..."
	@$(CP) -O binary $< $@

# Clean
clean:
	@echo "Cleaning..."
	@$(RM) -rf $(CONFIG)

# Bootload using SparkFun Variable Loader
bootload: all
	@echo "Uploading to $(COM_PORT) at $(SVL_UPLOAD_BAUD) baud..."
	$(PYTHON3) $(COMMONPATH)/tools_sfe/svl/svl.py $(COM_PORT) -f $(CONFIG)/$(TARGET).bin -b $(SVL_UPLOAD_BAUD) -v

# Include dependencies
-include $(DEPS)

.PHONY: all clean bootload directories
