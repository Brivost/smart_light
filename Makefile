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

# CMSIS-NN Path
CMSISNN_PATH := external/CMSISNN

# Include Paths
INCLUDES := -Isrc
INCLUDES += -I$(BOARDPATH)/bsp
INCLUDES += -I$(SDKPATH)
INCLUDES += -I$(SDKPATH)/utils
INCLUDES += -I$(SDKPATH)/devices
INCLUDES += -I$(SDKPATH)/mcu/apollo3
INCLUDES += -I$(SDKPATH)/CMSIS/AmbiqMicro/Include
INCLUDES += -I$(SDKPATH)/CMSIS/ARM/Include
INCLUDES += -I$(CMSISNN_PATH)/Include

# Source Files
SRC := main.c
SRC += startup_gcc.c
SRC += am_util_delay.c
SRC += am_util_faultisr.c
SRC += am_util_stdio.c
SRC += am_devices_led.c
SRC += am_util_id.c
SRC += uart.cpp
SRC += layers.cpp
# for debugging hal
# SRC += am_hal_adc.c
# SRC += am_hal_ble.c
# SRC += am_hal_ble_patch.c
# SRC += am_hal_ble_patch_b0.c
# SRC += am_hal_burst.c
# SRC += am_hal_cachectrl.c
# SRC += am_hal_clkgen.c
# SRC += am_hal_cmdq.c
# SRC += am_hal_ctimer.c
# SRC += am_hal_debug.c
# SRC += am_hal_flash.c
# SRC += am_hal_global.c
# SRC += am_hal_gpio.c
# SRC += am_hal_interrupt.c
# SRC += am_hal_iom.c
# SRC += am_hal_ios.c
# SRC += am_hal_itm.c
# SRC += am_hal_mcuctrl.c
# SRC += am_hal_mspi.c
# SRC += am_hal_pdm.c
# SRC += am_hal_pwrctrl.c
# SRC += am_hal_queue.c
# SRC += am_hal_reset.c
# SRC += am_hal_rtc.c
# SRC += am_hal_scard.c
# SRC += am_hal_secure_ota.c
# SRC += am_hal_security.c
# SRC += am_hal_stimer.c
# SRC += am_hal_sysctrl.c
# SRC += am_hal_systick.c
# SRC += am_hal_tpiu.c
# SRC += am_hal_uart.c
# SRC += am_hal_wdt.c

# CMSIS-NN Source Files (auto-discover all .c files from enabled modules)
CMSISNN_SRC := $(notdir $(wildcard $(CMSISNN_PATH)/Source/NNSupportFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/ActivationFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/BasicMathFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/ConvolutionFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/FullyConnectedFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/PoolingFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/SoftmaxFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/ConcatenationFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/ReshapeFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/SVDFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/LSTMFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/PadFunctions/*.c))
CMSISNN_SRC += $(notdir $(wildcard $(CMSISNN_PATH)/Source/TransposeFunctions/*.c))
SRC += $(CMSISNN_SRC)

# Source Paths (VPATH tells make where to find source files)
VPATH := src
VPATH += $(SDKPATH)/utils
VPATH += $(SDKPATH)/devices
VPATH += $(COMMONPATH)/tools_sfe/templates
# for debugging hal
# VPATH += $(SDKPATH)/mcu/apollo3/hal

# CMSIS-NN Source Paths (add/remove modules as needed)
VPATH += $(CMSISNN_PATH)/Source/NNSupportFunctions
VPATH += $(CMSISNN_PATH)/Source/ActivationFunctions
VPATH += $(CMSISNN_PATH)/Source/BasicMathFunctions
VPATH += $(CMSISNN_PATH)/Source/ConvolutionFunctions
VPATH += $(CMSISNN_PATH)/Source/FullyConnectedFunctions
VPATH += $(CMSISNN_PATH)/Source/PoolingFunctions
VPATH += $(CMSISNN_PATH)/Source/SoftmaxFunctions
VPATH += $(CMSISNN_PATH)/Source/ConcatenationFunctions
VPATH += $(CMSISNN_PATH)/Source/ReshapeFunctions
VPATH += $(CMSISNN_PATH)/Source/SVDFunctions
VPATH += $(CMSISNN_PATH)/Source/LSTMFunctions
VPATH += $(CMSISNN_PATH)/Source/PadFunctions
VPATH += $(CMSISNN_PATH)/Source/TransposeFunctions

# Precompiled Libraries
LIBS := $(BOARDPATH)/bsp/gcc/bin/libam_bsp.a
# comment out if compiling hal from source for debugging
LIBS += $(SDKPATH)/mcu/apollo3/hal/gcc/bin/libam_hal.a

#******************************************************************************
# Compiler and Linker Configuration
#******************************************************************************

# Executables
CC := $(TOOLCHAIN)-gcc
CXX := $(TOOLCHAIN)-g++
CP := $(TOOLCHAIN)-objcopy
SIZE := $(TOOLCHAIN)-size
RM := rm -f

# Common Flags
COMMON_FLAGS := -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += -MMD -MP -Wall
COMMON_FLAGS += -g -O0
COMMON_FLAGS += $(DEFINES)
COMMON_FLAGS += $(INCLUDES)

# C Compiler Flags
CFLAGS := $(COMMON_FLAGS) -std=c99

# C++ Compiler Flags
CXXFLAGS := $(COMMON_FLAGS) -std=c++17 -fno-exceptions -fno-rtti

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
CXXSRC := $(filter %.cpp,$(SRC))
OBJS := $(CSRC:%.c=$(CONFIG)/%.o) $(CXXSRC:%.cpp=$(CONFIG)/%.o)
DEPS := $(CSRC:%.c=$(CONFIG)/%.d) $(CXXSRC:%.cpp=$(CONFIG)/%.d)

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

# Compile C++ files
$(CONFIG)/%.o: %.cpp
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) -o $@ $<

# Link (use g++ when mixing C/C++)
$(CONFIG)/$(TARGET).axf: $(OBJS)
	@echo "Linking..."
	@$(CXX) -T $(LINKER_SCRIPT) -o $@ $(OBJS) $(LFLAGS)

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
