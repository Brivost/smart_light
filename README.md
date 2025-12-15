# Smart Fan - Artemis ATP Project

Firmware for the SparkFun RedBoard Artemis ATP.

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

- `src/` - Your application code
- `external/AmbiqSuiteSDK/` - SparkFun's AmbiqSuite SDK (git submodule)
- `Makefile` - Single file build system
- `build/` - Compiled output (generated)

## SDK Submodule

The project uses SparkFun's version of AmbiqSuiteSDK as a submodule:

```bash
git submodule add https://github.com/sparkfun/AmbiqSuiteSDK.git external/AmbiqSuiteSDK
git submodule update --init --recursive
```