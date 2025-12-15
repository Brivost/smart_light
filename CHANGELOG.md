# Changelog

All notable changes to this project will be documented in this file.

## [1.1.0] - 2025-12-14

### Changed
- Migrated Python dependency management from pip to uv
- Updated all documentation to reflect uv usage
- Replaced `requirements.txt` with `pyproject.toml`
- Modified Makefile to use `uv run` for flashing script
- Added `.python-version` file for Python version management

### Added
- `pyproject.toml` - Modern Python project configuration
- `uv.lock` - Lockfile for reproducible builds
- `docs/UV_GUIDE.md` - Comprehensive guide for using uv

### Removed
- `requirements.txt` - Replaced by pyproject.toml

## [1.0.0] - 2025-12-14

### Added
- Initial project setup for SparkFun Artemis ATP firmware development
- AmbiqSuiteSDK integrated as git submodule
- Complete VS Code configuration with IntelliSense
- Makefile-based build system
- Python-based firmware upload script
- Example LED blink firmware
- Board support package (BSP) for Artemis ATP
- Debug configuration for J-Link
- Comprehensive documentation and quick start guide

### Project Structure
- `/src` - Application source code
- `/linker` - Linker scripts for memory layout
- `/tools` - Utility scripts for flashing
- `/AmbiqSuiteSDK` - SDK submodule
- `/.vscode` - VS Code workspace configuration

