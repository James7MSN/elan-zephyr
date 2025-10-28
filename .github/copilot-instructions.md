# Elan-Zephyr Copilot Instructions

## Project Overview

This is a Zephyr RTOS module that adds support for Elan's EM32F967 microcontroller family. It extends the official Zephyr RTOS with custom board definitions, drivers, and device tree sources for the `32f967_dv` development board.

## Architecture

### Module Structure
- **Zephyr Module**: Defined in `zephyr/module.yml` with board_root, dts_root, and soc_root pointing to this directory
- **Integration**: Lives as `zephyrproject/elan-zephyr/` alongside the main `zephyrproject/zephyr/` repository
- **Build System**: Uses standard Zephyr west build system with module auto-discovery

### Key Components
- `boards/elan/32f967_dv/` - Board definition for EM32F967 development board
- `soc/elan/` - SoC-specific definitions for EM32 family
- `dts/arm/elan/` - Device tree sources (`em32fxxx.dtsi`, `em32f967-pinctrl.dtsi`)
- `drivers/` - Custom drivers (GPIO, SPI, UART, USB, flash, clock control)
- `samples/` - Example applications demonstrating peripheral usage

## Driver Architecture Patterns

### Device Tree Compatibility Strings
All custom drivers use `elan,em32*` compatible strings:
- `elan,em32-gpio` - GPIO controller
- `elan,em32` - SPI controller (PL022-based)
- `elan,elandev-uart` - UART controller
- `elan,em32-ahb`/`elan,em32-apb` - Clock controllers

### Driver Registration Pattern
```c
#define DT_DRV_COMPAT elan_em32_gpio  // Must match DTS compatible
// Driver implementation
DT_INST_FOREACH_STATUS_OKAY(DRIVER_INIT_MACRO)
```

### Clock Integration
Drivers expect clock references in DTS but handle missing `clk_id` gracefully:
```c
// Pattern: Default to clock ID 0 when not specified
static const clock_control_subsys_t em32_clk_id = (clock_control_subsys_t)0;
```

## Build Workflow

### Standard Build Commands
```bash
# From elan-zephyr directory
west build -b 32f967_dv samples/elanspi  # Basic SPI test
west build -b 32f967_dv samples/elan_gpio  # GPIO test
west build -p always  # Clean build
```

### Module Integration
The module auto-registers through `ZEPHYR_EXTRA_MODULES` in sample CMakeLists:
```cmake
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/../..)
```

## Device Tree Conventions

### Peripheral Addressing
- GPIO: `0x40020000` (GPIOA), `0x40021000` (GPIOB)
- SPI2: `0x40013000` 
- UART0: `0x40002000`
- Flash Controller: `0x40034000`

### Clock Tree
Two-level hierarchy: AHB clock feeds APB clock:
```dts
clk_ahb: clk-ahb { compatible = "elan,em32-ahb"; };
clk_apb: clk-apb { compatible = "elan,em32-apb"; clocks = <&clk_ahb>; };
```

## Debugging Patterns

### Common Issues
- **Clock ID errors**: Drivers expect `clk_id` parameter, default to 0 if missing
- **Devicetree generation**: Check `build/zephyr/zephyr.dts` for final DT compilation
- **Module loading**: Verify `zephyr/module.yml` paths are correct

### Key Files for Troubleshooting
- `build/zephyr/include/generated/zephyr/devicetree_generated.h` - Generated DT macros
- `build/zephyr/zephyr.dts` - Final compiled device tree
- `drivers/*/Kconfig` - Driver configuration options

## Sample Applications
- `samples/elanspi/` - SPI loopback test, reads sensor ID
- `samples/elan_gpio/` - Basic GPIO toggle
- `samples/elan_PB_all_high/` - Push button example

Focus on understanding the Zephyr module integration pattern and EM32-specific peripheral addressing when working with this codebase.
