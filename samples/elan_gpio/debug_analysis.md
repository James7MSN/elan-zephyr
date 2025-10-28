# GPIO Write Issue Analysis Report

## Problem Description
- GPIO Read Test (Test 1): ✅ **WORKING** - Successfully reads GPIO pin values
- GPIO Write Test (Test 2): ❌ **FAILING** - Writing 0xFFFF to PA/PB pins results in pins staying LOW

## Root Cause Analysis

Based on code review of the EM32F967 GPIO driver (`drivers/gpio/gpio_em32.c`), I've identified several potential issues:

### 1. **EM32F967 GPIO Architecture Issue** 
The EM32F967 has a non-standard GPIO architecture compared to typical ARM Cortex-M microcontrollers:

```c
/* From gpio_em32.c lines 231-245: */
/* In EM32F967, DATAOUT register bit controls BOTH direction AND output value:
 * - DATAOUT bit = 1: Output mode with high output  
 * - DATAOUT bit = 0: Input mode (or output mode with low output)
 * Direction is determined by the pin's usage pattern, not a separate register
 */
```

**Issue**: This architecture means that simply setting `gpio->DATAOUTSET = pins` may not properly configure pins as outputs.

### 2. **Missing Direction Control**
Unlike STM32 GPIOs which have separate direction registers, EM32F967 appears to use the DATAOUT register for both direction and value control. The current driver may not be handling this correctly.

### 3. **Port vs Individual Pin Operations**
The current test uses port-level operations:
- `gpio_port_set_bits_raw(gpioa_dev, porta_mask)`
- `gpio_port_clear_bits_raw(gpiob_dev, portb_mask)`

But individual pin configuration uses:
- `gpio_pin_configure(gpioa_dev, pin, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW)`

There may be a mismatch between how these operate internally.

## Recommended Fixes

### Fix 1: Use Individual Pin Operations Instead of Port Operations

Replace port-level operations with individual pin operations that are known to work:

```c
/* Instead of: gpio_port_set_bits_raw(gpioa_dev, 0xFF00) */
/* Use individual pin operations: */
for (int pin = 8; pin <= 15; pin++) {
    gpio_pin_set(gpioa_dev, pin, 1);
}

for (int pin = 0; pin <= 15; pin++) {
    if (pin == 11) continue; /* Skip PB11 */
    gpio_pin_set(gpiob_dev, pin, 1);
}
```

### Fix 2: Add Direction Verification

Add explicit direction verification to ensure pins are properly configured as outputs:

```c
/* Verify pin is configured as output before writing */
int gpio_flags = gpio_pin_get_config(gpioa_dev, pin);
if (!(gpio_flags & GPIO_OUTPUT)) {
    /* Reconfigure as output */
    gpio_pin_configure(gpioa_dev, pin, GPIO_OUTPUT);
}
```

### Fix 3: Debug Register Values

Add register readback to verify what's actually happening:

```c
/* Read back the DATAOUT register after writing */
GPIO_IPType *gpio = (GPIO_IPType *)0x40020000; /* GPIOA base */
printk("GPIOA DATAOUT after write: 0x%08X\n", gpio->DATAOUTSET);
printk("GPIOA DATA (pin states): 0x%08X\n", gpio->DATA);
```

### Fix 4: Alternative Driver Implementation

The EM32F967 may require a different approach for output control. Consider:

1. **Explicit Direction Setting**: EM32F967 might need explicit direction register configuration
2. **Different Output Registers**: There may be separate output enable and output value registers
3. **Clock/Power Issues**: GPIO banks might need explicit clock enabling

## Implementation Priority

1. **Immediate Fix**: Implement Fix 1 (individual pin operations) as it's most likely to work
2. **Verification**: Add Fix 2 (direction verification) to ensure pins are properly configured  
3. **Debugging**: Add Fix 3 (register readback) to understand what's happening at hardware level
4. **Long-term**: Investigate Fix 4 if the above don't resolve the issue

## Test Verification

After implementing fixes:

1. **Oscilloscope Test**: Verify actual pin voltages with oscilloscope/multimeter
2. **LED Test**: Connect LEDs to test pins to visually verify HIGH/LOW states
3. **Logic Analyzer**: Capture timing and ensure pins change state correctly
4. **Console Output**: Verify readback values match expected values

## Expected Results After Fix

- Test 2 (Write High): All test pins should read back as 0xFF00 (PORTA) and 0xF7FF (PORTB)
- Test 3 (Write Low): All test pins should read back as 0x0000 (both ports)
- Oscilloscope: Pins should show 3.3V (HIGH) and 0V (LOW) as expected
