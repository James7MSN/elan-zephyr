#!/bin/bash

# GPIO Test Debug Script
# This script helps debug GPIO issues by creating a more detailed test version

echo "Creating debug version of GPIO test..."

# Copy main.c to main_debug.c for debugging
cp /home/james/zephyrproject/elan-zephyr/samples/elan_gpio/src/main.c /home/james/zephyrproject/elan-zephyr/samples/elan_gpio/src/main_debug.c

echo "Debug version created at samples/elan_gpio/src/main_debug.c"
echo ""
echo "Key changes made to fix GPIO write issues:"
echo "1. Replaced gpio_port_set_bits_raw() with individual gpio_pin_set_raw()"
echo "2. Replaced gpio_port_clear_bits_raw() with individual gpio_pin_set_raw()"
echo "3. Added readback verification to show actual pin states"
echo "4. More detailed console output for debugging"
echo ""
echo "Test the updated version by flashing to the board:"
echo "  cd /home/james/zephyrproject/elan-zephyr"
echo "  west flash"
echo ""
echo "Expected improvements:"
echo "- Test 2 (Write High) should now properly set pins HIGH"
echo "- Test 3 (Write Low) should now properly set pins LOW"
echo "- Readback values should match expected values"
echo "- Individual pin control may work better than port operations"
