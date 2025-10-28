#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <string.h>

LOG_MODULE_REGISTER(ssp_test, LOG_LEVEL_DBG);

int ssp_basic_test(const struct device *spi_dev)
{
    struct spi_config spi_cfg = {
        .frequency = 1000000,  /* 1MHz */
        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA,
        .slave = 0,
        .cs = NULL,
    };

    uint8_t tx_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t rx_data[sizeof(tx_data)] = {0};

    struct spi_buf tx_buf = {
        .buf = tx_data,
        .len = sizeof(tx_data),
    };

    struct spi_buf rx_buf = {
        .buf = rx_data,
        .len = sizeof(rx_data),
    };

    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1,
    };

    LOG_INF("Starting basic SPI transfer");
    LOG_INF("TX data: %02x %02x %02x %02x %02x", 
            tx_data[0], tx_data[1], tx_data[2], tx_data[3], tx_data[4]);

    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
    if (ret != 0) {
        LOG_ERR("SPI transceive failed: %d", ret);
        return ret;
    }

    LOG_INF("RX data: %02x %02x %02x %02x %02x", 
            rx_data[0], rx_data[1], rx_data[2], rx_data[3], rx_data[4]);

    return 0;
}

int ssp_loopback_test(const struct device *spi_dev)
{
    struct spi_config spi_cfg = {
        .frequency = 2000000,  /* 2MHz */
        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
        .slave = 0,
        .cs = NULL,
    };

    uint8_t test_pattern[] = {0xAA, 0x55, 0xFF, 0x00, 0x5A, 0xA5, 0x33, 0xCC};
    uint8_t rx_data[sizeof(test_pattern)] = {0};

    struct spi_buf tx_buf = {
        .buf = test_pattern,
        .len = sizeof(test_pattern),
    };

    struct spi_buf rx_buf = {
        .buf = rx_data,
        .len = sizeof(rx_data),
    };

    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1,
    };

    LOG_INF("Starting loopback test (connect MOSI to MISO)");

    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
    if (ret != 0) {
        LOG_ERR("SPI loopback failed: %d", ret);
        return ret;
    }

    /* Verify loopback data */
    bool match = true;
    for (int i = 0; i < sizeof(test_pattern); i++) {
        if (test_pattern[i] != rx_data[i]) {
            LOG_ERR("Mismatch at index %d: TX=0x%02x, RX=0x%02x", 
                    i, test_pattern[i], rx_data[i]);
            match = false;
        }
    }

    if (match) {
        LOG_INF("Loopback test successful - all data matched");
        return 0;
    } else {
        LOG_ERR("Loopback test failed - data mismatch");
        return -1;
    }
}

int ssp_dma_test(const struct device *spi_dev)
{
    struct spi_config spi_cfg = {
        .frequency = 4000000,  /* 4MHz */
        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
        .slave = 0,
        .cs = NULL,
    };

    /* Large buffer to trigger DMA */
    uint8_t tx_data[256];
    uint8_t rx_data[256] = {0};

    /* Fill TX buffer with pattern */
    for (int i = 0; i < sizeof(tx_data); i++) {
        tx_data[i] = i & 0xFF;
    }

    struct spi_buf tx_buf = {
        .buf = tx_data,
        .len = sizeof(tx_data),
    };

    struct spi_buf rx_buf = {
        .buf = rx_data,
        .len = sizeof(rx_data),
    };

    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1,
    };

    LOG_INF("Starting DMA transfer test (256 bytes)");

    uint32_t start_time = k_cycle_get_32();
    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
    uint32_t end_time = k_cycle_get_32();

    if (ret != 0) {
        LOG_ERR("SPI DMA transfer failed: %d", ret);
        return ret;
    }

    uint32_t cycles = end_time - start_time;
    LOG_INF("DMA transfer completed in %u cycles", cycles);

    /* Verify first and last few bytes */
    LOG_INF("First 8 RX bytes: %02x %02x %02x %02x %02x %02x %02x %02x",
            rx_data[0], rx_data[1], rx_data[2], rx_data[3],
            rx_data[4], rx_data[5], rx_data[6], rx_data[7]);

    LOG_INF("Last 8 RX bytes: %02x %02x %02x %02x %02x %02x %02x %02x",
            rx_data[248], rx_data[249], rx_data[250], rx_data[251],
            rx_data[252], rx_data[253], rx_data[254], rx_data[255]);

    return 0;
}