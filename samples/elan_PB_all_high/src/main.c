/*
 * Copyright (c) 2024 ELAN Microelectronics Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Simple GPIO test program - PB8-PB15 always HIGH
 * 
 * This program sets PB8-PB15 to output HIGH and keeps them high always.
 * Simple demonstration of GPIO output functionality.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Get GPIO port device */
static const struct device *gpiob_dev;

/**
 * @brief Configure and set PB8-PB15 as outputs with HIGH value
 */
static int configure_pb_high_outputs(void)
{
    int ret;
    
    printk("Configuring PB8-PB15 as outputs and setting HIGH...\n");
    
    /* Configure PORTB pins PB8-PB15 as outputs with HIGH value */
    for (int pin = 8; pin <= 15; pin++) {
        /* First configure pin as output */
        ret = gpio_pin_configure(gpiob_dev, pin, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Failed to configure PB%d as output: %d\n", pin, ret);
            return ret;
        }
        
        /* Then set pin to HIGH */
        ret = gpio_pin_set_raw(gpiob_dev, pin, 1);
        if (ret != 0) {
            printk("Failed to set PB%d high: %d\n", pin, ret);
            return ret;
        }
        
        printk("PB%d configured as output and set HIGH\n", pin);
    }
    
    return 0;
}

/**
 * @brief Read and display PB8-PB15 values
 */
static void display_pb_status(void)
{
    uint32_t portb_value;
    
    /* Read PORTB */
    int ret = gpio_port_get_raw(gpiob_dev, &portb_value);
    if (ret != 0) {
        printk("Failed to read PORTB: %d\n", ret);
        return;
    }
    
    /* Extract PB8-PB15 values */
    uint16_t pb_high_pins = (portb_value >> 8) & 0xFF; /* PB8-PB15 */
    
    /* Display binary representation of PB8-PB15 */
    char pb_binary[9];
    for (int i = 7; i >= 0; i--) {
        pb_binary[7 - i] = (pb_high_pins & (1 << i)) ? '1' : '0';
    }
    pb_binary[8] = '\0';
    
    printk("PB15-PB8: %s (0x%02X) - Expected: 11111111 (0xFF)\n", 
           pb_binary, pb_high_pins);
    
    /* Check if all pins are HIGH as expected */
    if (pb_high_pins == 0xFF) {
        printk("✅ SUCCESS: All PB8-PB15 pins are HIGH\n");
    } else {
        printk("❌ ERROR: Some PB8-PB15 pins are not HIGH (Expected: 0xFF, Got: 0x%02X)\n", 
               pb_high_pins);
    }
}

/**
 * @brief Main function
 */
int main(void)
{
    int ret;
    
    printk("\n");
    printk("ELAN EM32F967 PB8-PB15 HIGH Test\n");
    printk("=================================\n");
    printk("This program sets PB8-PB15 to HIGH and keeps them high\n");
    printk("\n");
    
    /* Get GPIOB device */
    gpiob_dev = DEVICE_DT_GET(DT_NODELABEL(gpiob));
    if (!device_is_ready(gpiob_dev)) {
        printk("ERROR: GPIOB device not ready\n");
        return -1;
    }
    
    printk("GPIOB device ready\n");
    
    /* Configure PB8-PB15 as outputs and set HIGH */
    ret = configure_pb_high_outputs();
    if (ret != 0) {
        printk("ERROR: Failed to configure PB8-PB15 outputs: %d\n", ret);
        return -1;
    }
    
    printk("\nPB8-PB15 configuration complete!\n");
    printk("All pins should now be outputting HIGH (3.3V)\n");
    printk("You can measure with multimeter or oscilloscope\n");
    printk("\n");
    
    /* Main loop - periodically display status and ensure pins stay HIGH */
    int loop_count = 0;
    while (1) {
        loop_count++;
        
        printk("\n=== Status Check %d ===\n", loop_count);
        display_pb_status();
        
        /* Re-ensure pins are HIGH every 10 seconds (just to be safe) */
        if (loop_count % 2 == 0) {
            printk("Re-confirming PB8-PB15 are set HIGH...\n");
            for (int pin = 8; pin <= 15; pin++) {
                gpio_pin_set_raw(gpiob_dev, pin, 1);
            }
        }
        
        printk("Pins will remain HIGH. Waiting 5 seconds...\n");
        k_msleep(5000); /* Wait 5 seconds */
    }
    
    return 0;
}
