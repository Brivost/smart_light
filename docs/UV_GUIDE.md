# UV Python Environment Guide

This project uses [uv](https://github.com/astral-sh/uv) for fast Python dependency management.

## Why uv?

- ⚡ **10-100x faster** than pip
- 🔒 **Lockfile support** for reproducible builds
- 🐍 **Python version management** built-in
- 💾 **Disk-efficient** caching
- 🛠️ **Drop-in replacement** for pip workflows

## Quick Commands

### Install Dependencies

```bash
# First time setup
uv sync

# This creates a virtual environment in .venv and installs all dependencies
```

### Run Python Scripts

```bash
# Run any Python script with project dependencies
uv run python tools/artemis_svl.py -f build/firmware.bin

# This automatically uses the virtual environment
```

### Add New Dependencies

```bash
# Add a new dependency
uv add <package-name>

# Example:
uv add requests

# This updates pyproject.toml and installs the package
```

### Update Dependencies

```bash
# Update all dependencies
uv lock --upgrade

# Update a specific package
uv lock --upgrade-package <package-name>
```

## Project Files

- **`pyproject.toml`** - Project metadata and dependencies
- **`.python-version`** - Python version (3.11)
- **`.venv/`** - Virtual environment (auto-created by uv)
- **`uv.lock`** - Lockfile for reproducible installs (will be created)

## How It's Integrated

### Makefile

The `make flash` command uses uv:

```makefile
flash: $(BUILDDIR)/$(PROJECT).bin
    @uv run python tools/artemis_svl.py -f $(BUILDDIR)/$(PROJECT).bin -b 921600
```

### VS Code

You can configure VS Code to use the uv-managed virtual environment:

1. Open Command Palette (`Cmd+Shift+P`)
2. Select "Python: Select Interpreter"
3. Choose `.venv/bin/python`

## Manual Virtual Environment Activation

If you prefer to activate the virtual environment manually:

```bash
# Activate (bash/zsh)
source .venv/bin/activate

# Now you can use python directly
python tools/artemis_svl.py -f build/firmware.bin

# Deactivate when done
deactivate
```

## Troubleshooting

### "uv: command not found"

Install uv:

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

### Virtual Environment Issues

Remove and recreate:

```bash
rm -rf .venv
uv sync
```

### Python Version Mismatch

uv will automatically use the Python version specified in `.python-version`. If you need a different version:

```bash
# Change .python-version file
echo "3.12" > .python-version

# Recreate environment
rm -rf .venv
uv sync
```

## Comparison with pip

| Task | uv | pip |
|------|-----|-----|
| Install deps | `uv sync` | `pip install -r requirements.txt` |
| Run script | `uv run python script.py` | `python script.py` |
| Add package | `uv add package` | `pip install package` |
| Speed | ⚡ Very Fast | 🐌 Slow |

## Learn More

- [uv Documentation](https://docs.astral.sh/uv/)
- [uv GitHub](https://github.com/astral-sh/uv)

