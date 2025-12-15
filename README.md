# Smart Light - Artemis ATP Project 

Firmware for the SparkFun RedBoard Artemis ATP for turning on and off lights via speech recognition.

## Quick Start

```bash
# Build firmware
make

# Upload to board (replace with your serial port)
make bootload COM_PORT=/dev/cu.usbserial-110

# Clean build artifacts
make clean
```

See [BUILD.md](BUILD.md) for detailed build instructions.

## Project Structure

- `src/` - Application code
- `external/AmbiqSuiteSDK/` - SparkFun's AmbiqSuite SDK (git submodule)
- `Makefile` - Single file build system
- `build/` - Compiled output (generated)

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

## SDK Submodule

The project uses SparkFun's version of AmbiqSuiteSDK as a submodule:

```bash
git submodule add https://github.com/sparkfun/AmbiqSuiteSDK.git external/AmbiqSuiteSDK
git submodule update --init --recursive
```