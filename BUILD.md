# Build Instructions

This project uses a single Makefile to compile and upload firmware to the SparkFun RedBoard Artemis ATP.

## Prerequisites

- ARM GCC Toolchain: `arm-none-eabi-gcc` (already installed)
- Python 3 (for uploading firmware)
- AmbiqSuiteSDK (included as git submodule at `external/AmbiqSuiteSDK`)

## Build Commands

### Compile firmware:
```bash
make
```

This will create `build/smart_fan.bin`

### Upload firmware:
```bash
make bootload COM_PORT=/dev/cu.usbserial-XXX
```

Replace `/dev/cu.usbserial-XXX` with your actual serial port. You can also set a default in the Makefile.

**Note:** Close any serial monitor programs before uploading!

### Clean build:
```bash
make clean
```

## Configuration

The Makefile uses these key settings (all in one place):

- **BOARD**: `redboard_artemis_atp` (change if using a different board)
- **COM_PORT**: Default serial port for uploading
- **SVL_UPLOAD_BAUD**: Upload baud rate (default 115200)
- **SDKPATH**: Path to AmbiqSuiteSDK (default: `external/AmbiqSuiteSDK`)

## How It Works

The Makefile references the SDK directly without copying files:
- **Source files**: Your code in `src/`
- **BSP files**: Referenced from `$(SDKPATH)/boards_sfe/$(BOARD)/bsp/`
- **HAL libraries**: Pre-compiled libraries in the SDK
- **Linker script**: Uses SDK's SparkFun Variable Loader compatible script
- **Startup code**: Uses SDK's `startup_gcc.c`

This is the minimal approach - only one Makefile needed!

## Adding More Source Files

To add your own `.c` files:

1. Place them in `src/`
2. Add them to the `SRC` variable in the Makefile:
```makefile
SRC := main.c
SRC += my_new_file.c    # Add this line
SRC += another_file.c   # And this
```

3. Rebuild with `make`

## Troubleshooting

**Linker errors about missing functions?**
- Check that the required SDK utility files are in the `SRC` list
- Common ones: `am_util_delay.c`, `am_util_stdio.c`, `am_devices_led.c`

**Upload fails with "port in use"?**
- Close any serial monitor or terminal programs
- Make sure no other process is using the COM port

**Can't find headers?**
- The Makefile includes paths to all SDK headers automatically
- For board-specific BSP: `am_bsp.h`, `am_bsp_pins.h`
- For HAL: `am_mcu_apollo.h`, `am_hal_*.h`
- For utilities: `am_util.h`, `am_util_delay.h`, etc.

