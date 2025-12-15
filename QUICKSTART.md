# Quick Start Guide

## First Time Setup

1. **Install toolchain**:
   ```bash
   brew install --cask gcc-arm-embedded
   curl -LsSf https://astral.sh/uv/install.sh | sh
   uv sync
   ```

2. **Initialize SDK**:
   ```bash
   git submodule update --init --recursive
   ```

3. **Build**:
   ```bash
   make
   ```

4. **Flash**:
   ```bash
   make flash
   ```

## Common Commands

```bash
make              # Build firmware
make clean        # Clean build
make flash        # Upload to board
```

## VS Code Shortcuts

- `Cmd+Shift+B` - Build
- `F5` - Start debugging

## Serial Monitor

```bash
screen /dev/tty.usbserial-* 115200
```

Press `Ctrl+A` then `K` to exit screen.

