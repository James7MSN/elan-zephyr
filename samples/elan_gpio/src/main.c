/*
 * Copyright (c) 2024 ELAN Microelectronics Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief GPIO test program for EM32F967
 * 
 * This program tests GPIO functionality on PA8-PA15 and PB0-PB15.
 * Avoids console pins (PA1, PA2) and SW0 button (PA6).
 * Uses SW0 button to switch between test functions.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

/* Get button configuration from devicetree sw0 alias */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

/* Get GPIO port devices */
static const struct device *gpioa_dev;
static const struct device *gpiob_dev;

/* Test state variables */
static int current_test = 0;
static bool button_pressed_flag = false;

#define NUM_TESTS 4

/**
 * @brief Convert 16-bit value to binary string
 */
static void uint16_to_binary(uint16_t value, char *buffer)
{
    for (int i = 15; i >= 0; i--) {
        buffer[15 - i] = (value & (1 << i)) ? '1' : '0';
    }
    buffer[16] = '\0';
}

/**
 * @brief Button interrupt callback
 */
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    button_pressed_flag = true;
    printk("Button pressed - switching to next test\n");
}

/**
 * @brief Configure test GPIO pins as outputs
 * Test pins: PA8-PA15 and PB0-PB15 (excluding PB11 which is SW1)
 */
static int configure_test_gpio_outputs(void)
{
    int ret;
    
    printk("Configuring test GPIO pins as outputs...\n");
    
    /* Configure PORTA pins PA8-PA15 as outputs */
    for (int pin = 8; pin <= 15; pin++) {
        ret = gpio_pin_configure(gpioa_dev, pin, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
        if (ret != 0) {
            printk("Failed to configure PA%d as output: %d\n", pin, ret);
            return ret;
        }
    }
    
    /* Configure PORTB pins PB0-PB15 as outputs (except PB11 which is SW1) */
    for (int pin = 0; pin <= 15; pin++) {
        if (pin == 11) continue; /* Skip PB11 (SW1 button) */
        
        ret = gpio_pin_configure(gpiob_dev, pin, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW);
        if (ret != 0) {
            printk("Failed to configure PB%d as output: %d\n", pin, ret);
            return ret;
        }
    }
    
    printk("Test GPIO pins configured successfully\n");
    printk("Test pins: PA8-PA15, PB0-PB10, PB12-PB15\n");
    return 0;
}

/**
 * @brief Test 1: Read all test GPIO values and display in binary format
 */
static void test_read_all_gpio(void)
{
    uint32_t porta_value, portb_value;
    char porta_binary[17], portb_binary[17];
    
    printk("\n=== GPIO READ TEST ===\n");
    
    /* Read both ports */
    int ret = gpio_port_get_raw(gpioa_dev, &porta_value);
    if (ret != 0) {
        printk("Failed to read PORTA: %d\n", ret);
        return;
    }
    
    ret = gpio_port_get_raw(gpiob_dev, &portb_value);
    if (ret != 0) {
        printk("Failed to read PORTB: %d\n", ret);
        return;
    }
    
    /* Convert to binary strings */
    uint16_to_binary((uint16_t)porta_value, porta_binary);
    uint16_to_binary((uint16_t)portb_value, portb_binary);
    
    /* Display results */
    printk("PORTA (PA15-PA0): %s (0x%04X)\n", porta_binary, (uint16_t)porta_value);
    printk("PORTB (PB15-PB0): %s (0x%04X)\n", portb_binary, (uint16_t)portb_value);
    
    /* Extract and display test pin values */
    uint16_t test_porta = (porta_value >> 8) & 0xFF; /* PA8-PA15 */
    uint16_t test_portb = portb_value & 0xF7FF;       /* PB0-PB15 excluding PB11 */
    
    char test_porta_binary[9], test_portb_binary[17];
    for (int i = 7; i >= 0; i--) {
        test_porta_binary[7 - i] = (test_porta & (1 << i)) ? '1' : '0';
    }
    test_porta_binary[8] = '\0';
    
    uint16_to_binary(test_portb, test_portb_binary);
    
    printk("Test pins PA8-PA15: %s (0x%02X)\n", test_porta_binary, test_porta);
    printk("Test pins PB0-PB15: %s (0x%04X) [excluding PB11]\n", test_portb_binary, test_portb);
    printk("Press SW0 for next test\n");
}

/**
 * @brief Test 2: Write all 1s to test GPIO pins
 */
static void test_write_all_high(void)
{
    printk("\n=== WRITE ALL HIGH TEST ===\n");
    printk("Configuring pins as outputs and setting HIGH...\n");
    
    /* Configure and set PORTA pins PA8-PA15 as outputs with HIGH value */
    for (int pin = 8; pin <= 15; pin++) {
        /* First ensure pin is configured as output */
        int ret = gpio_pin_configure(gpioa_dev, pin, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Failed to configure PA%d as output: %d\n", pin, ret);
            return;
        }
        
        /* Then set pin to HIGH */
        ret = gpio_pin_set_raw(gpioa_dev, pin, 1);
        if (ret != 0) {
            printk("Failed to set PA%d high: %d\n", pin, ret);
            return;
        }
    }
    
    /* Configure and set PORTB pins as outputs with HIGH value (except PB11) */
    for (int pin = 0; pin <= 15; pin++) {
        if (pin == 11) continue; /* Skip PB11 (SW1 button) */
        
        /* First ensure pin is configured as output with pull-up */
        int ret = gpio_pin_configure(gpiob_dev, pin, GPIO_OUTPUT | GPIO_PULL_UP);
        if (ret != 0) {
            printk("Failed to configure PB%d as output: %d\n", pin, ret);
            return;
        }
        
        /* Then set pin to HIGH */
        ret = gpio_pin_set_raw(gpiob_dev, pin, 1);
        if (ret != 0) {
            printk("Failed to set PB%d high: %d\n", pin, ret);
            return;
        }
    }
    
    printk("PORTA test pins (PA8-PA15): Set to HIGH\n");
    printk("PORTB test pins (PB0-PB15): Set to HIGH [excluding PB11]\n");
    
    /* Verify by reading back the values */
    uint32_t porta_readback, portb_readback;
    gpio_port_get_raw(gpioa_dev, &porta_readback);
    gpio_port_get_raw(gpiob_dev, &portb_readback);
    
    printk("Readback - PORTA: 0x%04X, PORTB: 0x%04X\n", 
           (uint16_t)porta_readback, (uint16_t)portb_readback);
    printk("All test pins set HIGH! Press SW0 for next test\n");
}

/**
 * @brief Test 3: Write all 0s to test GPIO pins
 */
static void test_write_all_low(void)
{
    printk("\n=== WRITE ALL LOW TEST ===\n");
    printk("Configuring pins as outputs and setting LOW...\n");
    
    /* Configure and set PORTA pins PA8-PA15 as outputs with LOW value */
    for (int pin = 8; pin <= 15; pin++) {
        /* First ensure pin is configured as output */
        int ret = gpio_pin_configure(gpioa_dev, pin, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Failed to configure PA%d as output: %d\n", pin, ret);
            return;
        }
        
        /* Then set pin to LOW */
        ret = gpio_pin_set_raw(gpioa_dev, pin, 0);
        if (ret != 0) {
            printk("Failed to set PA%d low: %d\n", pin, ret);
            return;
        }
    }
    
    /* Configure and set PORTB pins as outputs with LOW value (except PB11) */
    for (int pin = 0; pin <= 15; pin++) {
        if (pin == 11) continue; /* Skip PB11 (SW1 button) */
        
        /* First ensure pin is configured as output */
        int ret = gpio_pin_configure(gpiob_dev, pin, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Failed to configure PB%d as output: %d\n", pin, ret);
            return;
        }
        
        /* Then set pin to LOW */
        ret = gpio_pin_set_raw(gpiob_dev, pin, 0);
        if (ret != 0) {
            printk("Failed to set PB%d low: %d\n", pin, ret);
            return;
        }
    }
    
    printk("PORTA test pins (PA8-PA15): Set to LOW\n");
    printk("PORTB test pins (PB0-PB15): Set to LOW [excluding PB11]\n");
    
    /* Verify by reading back the values */
    uint32_t porta_readback, portb_readback;
    gpio_port_get_raw(gpioa_dev, &porta_readback);
    gpio_port_get_raw(gpiob_dev, &portb_readback);
    
    printk("Readback - PORTA: 0x%04X, PORTB: 0x%04X\n", 
           (uint16_t)porta_readback, (uint16_t)portb_readback);
    printk("All test pins set LOW! Press SW0 for next test\n");
}

/**
 * @brief Test 4: Toggle test GPIO pins
 */
static void test_toggle_pins(void)
{
    static int toggle_count = 0;
    static bool toggle_state = false;
    
    if (toggle_count == 0) {
        printk("\n=== TOGGLE PINS TEST ===\n");
        printk("Configuring pins as outputs and toggling every 2 seconds...\n");
    }
    
    /* Toggle state */
    toggle_state = !toggle_state;
    
    /* Configure and set PORTA pins PA8-PA15 to toggle state */
    for (int pin = 8; pin <= 15; pin++) {
        /* Ensure pin is configured as output */
        int ret = gpio_pin_configure(gpioa_dev, pin, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Failed to configure PA%d as output: %d\n", pin, ret);
            return;
        }
        
        /* Set pin to toggle state */
        ret = gpio_pin_set_raw(gpioa_dev, pin, toggle_state ? 1 : 0);
        if (ret != 0) {
            printk("Failed to set PA%d: %d\n", pin, ret);
            return;
        }
    }
    
    /* Configure and set PORTB pins to toggle state (except PB11) */
    for (int pin = 0; pin <= 15; pin++) {
        if (pin == 11) continue; /* Skip PB11 (SW1 button) */
        
        /* Ensure pin is configured as output */
        int ret = gpio_pin_configure(gpiob_dev, pin, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Failed to configure PB%d as output: %d\n", pin, ret);
            return;
        }
        
        /* Set pin to toggle state */
        ret = gpio_pin_set_raw(gpiob_dev, pin, toggle_state ? 1 : 0);
        if (ret != 0) {
            printk("Failed to set PB%d: %d\n", pin, ret);
            return;
        }
    }
    
    toggle_count++;
    printk("Toggle count: %d, State: %s\n", toggle_count, toggle_state ? "HIGH" : "LOW");
    
    if (toggle_count % 5 == 0) {
        printk("Press SW0 for next test\n");
    }
}

/**
 * @brief Switch to next test
 */
static void switch_test(void)
{
    current_test++;
    if (current_test >= NUM_TESTS) {
        current_test = 0;
    }
    
    printk("\n====================================================\n");
    
    switch(current_test) {
        case 0:
            printk("SWITCHING TO TEST 1: Read All GPIO Values\n");
            break;
        case 1:
            printk("SWITCHING TO TEST 2: Write All High\n");
            break;
        case 2:
            printk("SWITCHING TO TEST 3: Write All Low\n");
            break;
        case 3:
            printk("SWITCHING TO TEST 4: Toggle Pins\n");
            break;
    }
    
    printk("====================================================\n");
}

/**
 * @brief Main function
 */
int main(void)
{
    int ret;
    
    printk("\n");
    printk("ELAN EM32F967 GPIO Test Program\n");
    printk("===============================\n");
    printk("Test pins: PA8-PA15, PB0-PB15 (excluding PB11)\n");
    printk("Avoids: PA1,PA2 (console), PA6 (SW0), PB11 (SW1)\n");
    printk("\n");
    
    /* Get GPIO devices */
    gpioa_dev = DEVICE_DT_GET(DT_NODELABEL(gpioa));
    if (!device_is_ready(gpioa_dev)) {
        printk("ERROR: GPIOA device not ready\n");
        return -1;
    }
    
    gpiob_dev = DEVICE_DT_GET(DT_NODELABEL(gpiob));
    if (!device_is_ready(gpiob_dev)) {
        printk("ERROR: GPIOB device not ready\n");
        return -1;
    }
    
    printk("GPIO devices ready: GPIOA and GPIOB\n");
    
    /* Configure button */
    if (!gpio_is_ready_dt(&button)) {
        printk("ERROR: Button device not ready\n");
        return -1;
    }
    
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        printk("ERROR: Failed to configure button pin: %d\n", ret);
        return -1;
    }
    
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("ERROR: Failed to configure button interrupt: %d\n", ret);
        return -1;
    }
    
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    
    printk("Button configured at %s pin %d\n", button.port->name, button.pin);
    
    /* Configure test GPIO pins as outputs */
    ret = configure_test_gpio_outputs();
    if (ret != 0) {
        printk("ERROR: Failed to configure test GPIO outputs: %d\n", ret);
        return -1;
    }
    
    printk("\nAvailable Tests:\n");
    printk("1. Read All GPIO Values\n");
    printk("2. Write All High\n");
    printk("3. Write All Low\n");
    printk("4. Toggle Pins\n");
    printk("\nPress SW0 to switch between tests\n");
    printk("Starting with Test 1...\n");
    
    /* Main loop */
    while (1) {
        /* Check for button press */
        if (button_pressed_flag) {
            button_pressed_flag = false;
            switch_test();
            k_msleep(200); /* Debounce delay */
        }
        
        /* Run current test */
        switch(current_test) {
            case 0:
                test_read_all_gpio();
                break;
            case 1:
                test_write_all_high();
                break;
            case 2:
                test_write_all_low();
                break;
            case 3:
                test_toggle_pins();
                break;
        }
        
        k_msleep(2000); /* Wait 2 seconds between operations */
    }
    
    return 0;
}
