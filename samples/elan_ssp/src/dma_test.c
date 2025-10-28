#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/dma.h>
#include <string.h>

LOG_MODULE_REGISTER(dma_test, LOG_LEVEL_DBG);

int dma_memory_test(void)
{
    LOG_INF("DMA memory test - SKIPPED (requires DMA device tree configuration)");
    LOG_INF("Note: Zephyr's PL022 SPI driver will automatically use DMA for large transfers");
    return 0;
}