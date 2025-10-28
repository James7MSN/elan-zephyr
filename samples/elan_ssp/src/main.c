#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* External test functions */
extern int ssp_basic_test(const struct device *spi_dev);
extern int ssp_loopback_test(const struct device *spi_dev);
extern int ssp_dma_test(const struct device *spi_dev);
extern int dma_memory_test(void);
extern int spi_debug_test_main(void);

#define SPI_NODE DT_NODELABEL(ssp2)

int main(void)
{
    const struct device *spi_dev;
    int ret;

    printk("\n=== ELAN EM32F967 SSP/DMA Test Suite ===\n");
    printk("Elan EM32 SSP Driver with DMA Support\n\n");

    /* Get SPI device */
    spi_dev = DEVICE_DT_GET(SPI_NODE);
    if (!device_is_ready(spi_dev)) {
        LOG_ERR("SPI device not ready");
        return -1;
    }

    LOG_INF("SPI device ready: %s", spi_dev->name);

    /* Debug Tests: Comprehensive SPI/DMA debugging */
    printk("\n--- Debug Tests: SPI/DMA Analysis ---\n");
    spi_debug_test_main();

    /* Test 1: Basic SPI functionality */
    printk("\n--- Test 1: Basic SPI Transfer ---\n");
    ret = ssp_basic_test(spi_dev);
    if (ret == 0) {
        printk("✓ Basic SPI test PASSED\n");
    } else {
        printk("✗ Basic SPI test FAILED: %d\n", ret);
    }

    /* Test 2: SPI Loopback test */
    printk("\n--- Test 2: SPI Loopback Test ---\n");
    ret = ssp_loopback_test(spi_dev);
    if (ret == 0) {
        printk("✓ Loopback test PASSED\n");
    } else {
        printk("✗ Loopback test FAILED: %d\n", ret);
    }

    /* Test 3: DMA Memory-to-Memory */
    printk("\n--- Test 3: DMA Memory Test ---\n");
    ret = dma_memory_test();
    if (ret == 0) {
        printk("✓ DMA memory test PASSED\n");
    } else {
        printk("✗ DMA memory test FAILED: %d\n", ret);
    }

    /* Test 4: SPI with DMA */
    printk("\n--- Test 4: SPI DMA Transfer ---\n");
    ret = ssp_dma_test(spi_dev);
    if (ret == 0) {
        printk("✓ SPI DMA test PASSED\n");
    } else {
        printk("✗ SPI DMA test FAILED: %d\n", ret);
    }

    printk("\n=== Test Suite Complete ===\n");
    
    /* Keep running for continuous testing */
    while (1) {
        k_sleep(K_SECONDS(10));
        printk("System running... (press reset to restart tests)\n");
    }

    return 0;
}