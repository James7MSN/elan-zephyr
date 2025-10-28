/*
 * Copyright (c) 2025 Elan Microelectronics Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

LOG_MODULE_REGISTER(spi_debug_test, LOG_LEVEL_DBG);

#define SPI_DEVICE_NODE DT_NODELABEL(ssp2)
#define DMA_DEVICE_NODE DT_NODELABEL(dma0)

/* Test data patterns */
static uint8_t tx_buffer_small[16] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};

static uint8_t rx_buffer_small[16];

static uint8_t tx_buffer_large[256];
static uint8_t rx_buffer_large[256];

/* Test configuration */
static struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA,
	.frequency = 1000000, /* 1 MHz */
	.slave = 0,
};

/* Test results */
struct test_result {
	const char *name;
	int result;
	uint32_t duration_ms;
};

static struct test_result test_results[10];
static int test_count = 0;

static void record_test_result(const char *name, int result, uint32_t duration_ms)
{
	if (test_count < ARRAY_SIZE(test_results)) {
		test_results[test_count].name = name;
		test_results[test_count].result = result;
		test_results[test_count].duration_ms = duration_ms;
		test_count++;
	}
}

static void print_test_summary(void)
{
	printk("\n=== SPI Debug Test Summary ===\n");
	for (int i = 0; i < test_count; i++) {
		printk("%-25s: %s (%d ms)\n", 
		       test_results[i].name,
		       test_results[i].result == 0 ? "PASS" : "FAIL",
		       test_results[i].duration_ms);
	}
	printk("===============================\n");
}

static void dump_spi_registers(const struct device *spi_dev)
{
	/* Access SPI registers directly - SSP2 base address is 0x40013000 */
	volatile uint32_t *ssp_base = (volatile uint32_t *)0x40013000;

	LOG_INF("=== SPI Register Dump ===");
	LOG_INF("SSP_CR0   (0x00): 0x%08x", ssp_base[0x00/4]);
	LOG_INF("SSP_CR1   (0x04): 0x%08x", ssp_base[0x04/4]);
	LOG_INF("SSP_DR    (0x08): 0x%08x", ssp_base[0x08/4]);
	LOG_INF("SSP_SR    (0x0C): 0x%08x", ssp_base[0x0C/4]);
	LOG_INF("SSP_CPSR  (0x10): 0x%08x", ssp_base[0x10/4]);
	LOG_INF("SSP_IMSC  (0x14): 0x%08x", ssp_base[0x14/4]);
	LOG_INF("SSP_RIS   (0x18): 0x%08x", ssp_base[0x18/4]);
	LOG_INF("SSP_MIS   (0x1C): 0x%08x", ssp_base[0x1C/4]);
	LOG_INF("SSP_ICR   (0x20): 0x%08x", ssp_base[0x20/4]);
	LOG_INF("SSP_DMACR (0x24): 0x%08x", ssp_base[0x24/4]);
	LOG_INF("========================");
}

static void dump_dma_registers(const struct device *dma_dev)
{
	/* This would need to be implemented with actual register access */
	LOG_INF("=== DMA Register Dump ===");
	LOG_INF("Note: Register dump requires direct register access");
	LOG_INF("========================");
}

static int test_spi_polling_mode(const struct device *spi_dev)
{
	uint32_t start_time = k_uptime_get_32();
	int ret;

	LOG_INF("=== Testing SPI Polling Mode ===");
	LOG_INF("Note: This test should use polling mode, but driver may still use interrupts");

	/* Prepare test data */
	for (int i = 0; i < sizeof(tx_buffer_small); i++) {
		tx_buffer_small[i] = i + 1;
	}
	memset(rx_buffer_small, 0, sizeof(rx_buffer_small));

	struct spi_buf tx_buf = {
		.buf = tx_buffer_small,
		.len = sizeof(tx_buffer_small)
	};
	struct spi_buf rx_buf = {
		.buf = rx_buffer_small,
		.len = sizeof(rx_buffer_small)
	};
	struct spi_buf_set tx_bufs = {
		.buffers = &tx_buf,
		.count = 1
	};
	struct spi_buf_set rx_bufs = {
		.buffers = &rx_buf,
		.count = 1
	};

	LOG_INF("TX data: %02x %02x %02x %02x %02x %02x %02x %02x",
		tx_buffer_small[0], tx_buffer_small[1], tx_buffer_small[2], tx_buffer_small[3],
		tx_buffer_small[4], tx_buffer_small[5], tx_buffer_small[6], tx_buffer_small[7]);

	ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);

	uint32_t duration = k_uptime_get_32() - start_time;

	if (ret == 0) {
		LOG_INF("RX data: %02x %02x %02x %02x %02x %02x %02x %02x",
			rx_buffer_small[0], rx_buffer_small[1], rx_buffer_small[2], rx_buffer_small[3],
			rx_buffer_small[4], rx_buffer_small[5], rx_buffer_small[6], rx_buffer_small[7]);
		LOG_INF("Polling mode test PASSED");
	} else {
		LOG_ERR("Polling mode test FAILED: %d", ret);
		dump_spi_registers(spi_dev);
	}

	record_test_result("SPI Polling Mode", ret, duration);
	return ret;
}

static volatile bool async_complete = false;
static volatile int async_result = 0;

static void spi_async_callback(const struct device *dev, int result, void *data)
{
	LOG_DBG("SPI async callback: result=%d", result);
	async_result = result;
	async_complete = true;
}

static int test_spi_interrupt_mode(const struct device *spi_dev)
{
	uint32_t start_time = k_uptime_get_32();
	int ret;

	LOG_INF("=== Testing SPI Interrupt Mode ===");

	/* Prepare test data */
	for (int i = 0; i < sizeof(tx_buffer_small); i++) {
		tx_buffer_small[i] = 0xA0 + i;
	}
	memset(rx_buffer_small, 0, sizeof(rx_buffer_small));

	struct spi_buf tx_buf = {
		.buf = tx_buffer_small,
		.len = sizeof(tx_buffer_small)
	};
	struct spi_buf rx_buf = {
		.buf = rx_buffer_small,
		.len = sizeof(rx_buffer_small)
	};
	struct spi_buf_set tx_bufs = {
		.buffers = &tx_buf,
		.count = 1
	};
	struct spi_buf_set rx_bufs = {
		.buffers = &rx_buf,
		.count = 1
	};

	async_complete = false;
	async_result = 0;

	LOG_INF("TX data: %02x %02x %02x %02x %02x %02x %02x %02x",
		tx_buffer_small[0], tx_buffer_small[1], tx_buffer_small[2], tx_buffer_small[3],
		tx_buffer_small[4], tx_buffer_small[5], tx_buffer_small[6], tx_buffer_small[7]);

#ifdef CONFIG_SPI_ASYNC
	ret = spi_transceive_cb(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs, spi_async_callback, NULL);
	if (ret == 0) {
		/* Wait for completion with timeout */
		int timeout_count = 0;
		while (!async_complete && timeout_count < 1000) {
			k_msleep(1);
			timeout_count++;
		}
		
		if (!async_complete) {
			LOG_ERR("Interrupt mode test TIMEOUT");
			ret = -ETIMEDOUT;
		} else {
			ret = async_result;
		}
	}
#else
	LOG_WRN("SPI_ASYNC not enabled, skipping interrupt test");
	ret = -ENOTSUP;
#endif

	uint32_t duration = k_uptime_get_32() - start_time;

	if (ret == 0) {
		LOG_INF("RX data: %02x %02x %02x %02x %02x %02x %02x %02x",
			rx_buffer_small[0], rx_buffer_small[1], rx_buffer_small[2], rx_buffer_small[3],
			rx_buffer_small[4], rx_buffer_small[5], rx_buffer_small[6], rx_buffer_small[7]);
		LOG_INF("Interrupt mode test PASSED");
	} else {
		LOG_ERR("Interrupt mode test FAILED: %d", ret);
		dump_spi_registers(spi_dev);
	}

	record_test_result("SPI Interrupt Mode", ret, duration);
	return ret;
}

static int test_dma_controller(const struct device *dma_dev)
{
	uint32_t start_time = k_uptime_get_32();
	int ret = 0;

	LOG_INF("=== Testing DMA Controller ===");

	if (!device_is_ready(dma_dev)) {
		LOG_ERR("DMA device not ready");
		ret = -ENODEV;
	} else {
		LOG_INF("DMA controller is ready");
		
		/* Test DMA status */
		struct dma_status status;
		ret = dma_get_status(dma_dev, 0, &status);
		if (ret == 0) {
			LOG_INF("DMA channel 0 status: busy=%d", status.busy);
		} else {
			LOG_ERR("Failed to get DMA status: %d", ret);
		}
	}

	uint32_t duration = k_uptime_get_32() - start_time;
	record_test_result("DMA Controller", ret, duration);
	return ret;
}

static int test_spi_dma_mode(const struct device *spi_dev)
{
	uint32_t start_time = k_uptime_get_32();
	int ret;

	LOG_INF("=== Testing SPI DMA Mode ===");

	/* Prepare large test data */
	for (int i = 0; i < sizeof(tx_buffer_large); i++) {
		tx_buffer_large[i] = (i & 0xFF);
	}
	memset(rx_buffer_large, 0, sizeof(rx_buffer_large));

	struct spi_buf tx_buf = {
		.buf = tx_buffer_large,
		.len = sizeof(tx_buffer_large)
	};
	struct spi_buf rx_buf = {
		.buf = rx_buffer_large,
		.len = sizeof(rx_buffer_large)
	};
	struct spi_buf_set tx_bufs = {
		.buffers = &tx_buf,
		.count = 1
	};
	struct spi_buf_set rx_bufs = {
		.buffers = &rx_buf,
		.count = 1
	};

	LOG_INF("TX data (first 8): %02x %02x %02x %02x %02x %02x %02x %02x",
		tx_buffer_large[0], tx_buffer_large[1], tx_buffer_large[2], tx_buffer_large[3],
		tx_buffer_large[4], tx_buffer_large[5], tx_buffer_large[6], tx_buffer_large[7]);

	ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);

	uint32_t duration = k_uptime_get_32() - start_time;

	if (ret == 0) {
		LOG_INF("RX data (first 8): %02x %02x %02x %02x %02x %02x %02x %02x",
			rx_buffer_large[0], rx_buffer_large[1], rx_buffer_large[2], rx_buffer_large[3],
			rx_buffer_large[4], rx_buffer_large[5], rx_buffer_large[6], rx_buffer_large[7]);
		LOG_INF("DMA mode test PASSED");
	} else {
		LOG_ERR("DMA mode test FAILED: %d", ret);
		dump_spi_registers(spi_dev);
	}

	record_test_result("SPI DMA Mode", ret, duration);
	return ret;
}

int spi_debug_test_main(void)
{
	const struct device *spi_dev = DEVICE_DT_GET(SPI_DEVICE_NODE);
	const struct device *dma_dev = DEVICE_DT_GET_OR_NULL(DMA_DEVICE_NODE);

	printk("\n=== EM32F967 SPI Debug Test Suite ===\n");
	printk("SPI Device: %s\n", spi_dev->name);
	if (dma_dev) {
		printk("DMA Device: %s\n", dma_dev->name);
	} else {
		printk("DMA Device: Not available\n");
	}

	if (!device_is_ready(spi_dev)) {
		LOG_ERR("SPI device not ready");
		return -ENODEV;
	}

	/* Test 1: SPI Polling Mode */
	test_spi_polling_mode(spi_dev);
	k_msleep(100);

	/* Test 2: DMA Controller */
	if (dma_dev) {
		test_dma_controller(dma_dev);
		k_msleep(100);
	}

	/* Test 3: SPI Interrupt Mode */
	test_spi_interrupt_mode(spi_dev);
	k_msleep(100);

	/* Test 4: SPI DMA Mode */
	test_spi_dma_mode(spi_dev);
	k_msleep(100);

	/* Print summary */
	print_test_summary();

	return 0;
}
