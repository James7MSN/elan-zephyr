/*
 * Copyright (c) 2024 Elan Microelectronics Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT elan_em32_crypto

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/crypto/crypto.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/clock_control.h>
#include <errno.h>
#include <string.h>
#include <soc.h>
#include "../../include/zephyr/drivers/clock_control/clock_control_em32_apb.h"

LOG_MODULE_REGISTER(crypto_em32_sha, CONFIG_CRYPTO_LOG_LEVEL);

/* EM32F967 SHA256 Hardware Registers - Based on Hardware Specification */
#define SHA_CTR_OFFSET      0x00
#define SHA_IN_OFFSET       0x04
#define SHA_OUT_OFFSET      0x08
#define SHA_DATALEN_5832_OFFSET 0x28  /* Data Length Upper [58:32] */
#define SHA_DATALEN_OFFSET  0x2C      /* Data Length Lower [31:0] */
#define SHA_PAD_CTR_OFFSET  0x30      /* Padding Control */

/* SHA Control Register Bits */
#define SHA_STR_BIT         BIT(0)  /* Start */
#define SHA_INT_CLR_BIT     BIT(1)  /* Interrupt Clear */
#define SHA_RST_BIT         BIT(2)  /* Reset */
#define SHA_READY_BIT       BIT(3)  /* Ready */
#define SHA_STA_BIT         BIT(4)  /* Status */
#define SHA_INT_MASK_BIT    BIT(5)  /* Interrupt Mask */
#define SHA_WR_REV_BIT      BIT(8)  /* Write Reverse */
#define SHA_RD_REV_BIT      BIT(9)  /* Read Reverse */

/* SHA Padding Control Register Bits */
#define SHA_PAD_PACKET_MASK 0x1F    /* Padding packet count (bits 4:0) */
#define SHA_VALID_BYTE_SHIFT 8      /* Valid byte count (bits 9:8) */
#define SHA_VALID_BYTE_MASK 0x3

/* SHA256 constants */
#define SHA256_DIGEST_SIZE  32
#define SHA256_BLOCK_SIZE   64



/* Clock gating constants */
#define EM32_CLKGATEREG     0x40030100  /* Clock Gating Control Register */

enum sha_operation_state {
    SHA_STATE_IDLE,
    SHA_STATE_BUSY,
    SHA_STATE_ERROR
};

struct crypto_em32_config {
    uint32_t base;
    const struct device *clock_dev;
    uint32_t clock_group_id;
#ifdef CONFIG_CRYPTO_EM32_SHA_INTERRUPT
    void (*irq_config_func)(const struct device *dev);
#endif
};

struct crypto_em32_data {
    struct hash_ctx *ctx;
    enum sha_operation_state state;
    hash_completion_cb callback;

    /* Legacy small-data buffer (<=255B; 256B goes to accumulation path) */
    uint8_t buffer[256];              /* Backwards compatibility */
    uint32_t total_len;               /* Legacy total length */
    uint16_t buffer_len;              /* Legacy buffer length (avoid overflow) */

    /* Accumulation buffer for large data (single-run hashing) */
    uint8_t *accum_buf;               /* Dynamically grown buffer */
    size_t accum_len;
    size_t accum_cap;
    bool use_accum;                   /* True if using accumulation path */

    bool session_active;
#ifdef CONFIG_CRYPTO_EM32_SHA_INTERRUPT
    struct k_sem op_complete;
#endif
};

/* Register access macros */
#define SHA_REG(dev, offset) \
    ((volatile uint32_t *)(((const struct crypto_em32_config *)dev->config)->base + offset))

#define SHA_CTR(dev)        SHA_REG(dev, SHA_CTR_OFFSET)
#define SHA_IN(dev)         SHA_REG(dev, SHA_IN_OFFSET)
#define SHA_OUT(dev)        SHA_REG(dev, SHA_OUT_OFFSET)
#define SHA_DATALEN_5832(dev) SHA_REG(dev, SHA_DATALEN_5832_OFFSET)
#define SHA_DATALEN(dev)    SHA_REG(dev, SHA_DATALEN_OFFSET)
#define SHA_PAD_CTR(dev)    SHA_REG(dev, SHA_PAD_CTR_OFFSET)

static inline void sha_write_reg(const struct device *dev, uint32_t offset, uint32_t value)
{
    *SHA_REG(dev, offset) = value;
}

static inline uint32_t sha_read_reg(const struct device *dev, uint32_t offset)
{
    return *SHA_REG(dev, offset);
}

/* Initialize hash state with SHA256 initial values */
/* Helpers for dynamic accumulation buffer */
static int ensure_accum_capacity(struct crypto_em32_data *data, size_t need)
{
    if (data->accum_cap >= need) {
        return 0;
    }
    /* Enforce maximum accumulation size from Kconfig */
    if (need > CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE) {
        return -ENOMEM;
    }
    size_t new_cap = data->accum_cap ? data->accum_cap : 512;
    while (new_cap < need) {
        size_t next = new_cap * 2U;
        if (next > CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE) {
            new_cap = CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE;
            break;
        }
        new_cap = next;
    }
    uint8_t *new_buf = k_malloc(new_cap);
    if (!new_buf) {
        return -ENOMEM;
    }
    if (data->accum_len && data->accum_buf) {
        memcpy(new_buf, data->accum_buf, data->accum_len);
        k_free(data->accum_buf);
    }
    data->accum_buf = new_buf;
    data->accum_cap = new_cap;
    return 0;
}

static int accum_append(struct crypto_em32_data *data, const uint8_t *src, size_t len)
{
    if (len == 0) return 0;
    int ret = ensure_accum_capacity(data, data->accum_len + len);
    if (ret) return ret;
    memcpy(data->accum_buf + data->accum_len, src, len);
    data->accum_len += len;
    return 0;
}


static void sha_disable_clkgate(void)
{
    /* Read current clock gating register value */
    uint32_t clkgate_reg = *((volatile uint32_t *)EM32_CLKGATEREG);

    /* Clear the ENCRYPT clock gate bit (bit 6) to enable crypto clock */
    clkgate_reg &= ~(1U << HCLKG_ENCRYPT);

    /* Write back the modified value */
    *((volatile uint32_t *)EM32_CLKGATEREG) = clkgate_reg;


}

static void sha_reset(const struct device *dev)
{
    uint32_t ctrl;

    /* Reset SHA engine */
    ctrl = sha_read_reg(dev, SHA_CTR_OFFSET);
    ctrl |= SHA_RST_BIT;
    sha_write_reg(dev, SHA_CTR_OFFSET, ctrl);

    /* Wait for reset completion */
    while (sha_read_reg(dev, SHA_CTR_OFFSET) & SHA_RST_BIT) {
        k_busy_wait(1);
    }
}

static void sha_configure(const struct device *dev)
{
    uint32_t ctrl = 0;

    /* Configure byte reversal for input and output */
    ctrl |= SHA_WR_REV_BIT | SHA_RD_REV_BIT;

#ifdef CONFIG_CRYPTO_EM32_SHA_INTERRUPT
    ctrl |= SHA_INT_MASK_BIT;
#endif

    sha_write_reg(dev, SHA_CTR_OFFSET, ctrl);
}

/* Zephyr Crypto API Implementation */

static int crypto_em32_query_hw_caps(const struct device *dev)
{
    return CAP_SEPARATE_IO_BUFS | CAP_SYNC_OPS;
}

static int em32_sha256_handler(struct hash_ctx *ctx, struct hash_pkt *pkt, bool finish)
{
    const struct device *dev = ctx->device;
    struct crypto_em32_data *data = dev->data;
    const struct crypto_em32_config *config = dev->config;

    if (!data->session_active || data->ctx != ctx) {
        return -EINVAL;
    }

    if (data->state == SHA_STATE_ERROR) {
        return -EIO;
    }

    /* Handle data input (non-finish calls) */
    if (!finish && pkt->in_len > 0) {
        if (!pkt->in_buf) {
            LOG_ERR("Null input buffer pointer");
            return -EINVAL;
        }
        /* Prefer legacy small-buffer path if it fits and we haven't switched */
        if (!data->use_accum && (data->buffer_len + pkt->in_len) < sizeof(data->buffer)) {
            memcpy(&data->buffer[data->buffer_len], pkt->in_buf, pkt->in_len);
            data->buffer_len += pkt->in_len;
            data->total_len = data->buffer_len;
            return 0;
        }
        /* Use accumulation buffer for larger inputs */
        if (!data->use_accum) {
            /* Pre-allocate a larger initial capacity to avoid late realloc failure */
            size_t need_now = data->buffer_len + pkt->in_len;
            size_t prealloc = CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE;
            if (need_now > prealloc) {
                prealloc = need_now;
            }
            if (prealloc > CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE) {
                prealloc = CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE;
            }
            int ret = ensure_accum_capacity(data, prealloc);
            if (ret) {
                LOG_ERR("Accum buffer initial alloc failed (need=%zu): %d", prealloc, ret);
                return ret;
            }
            if (data->buffer_len) {
                memcpy(data->accum_buf, data->buffer, data->buffer_len);
                data->accum_len = data->buffer_len;
                data->buffer_len = 0;
            }
            data->use_accum = true;
        }
        int ret = accum_append(data, pkt->in_buf, pkt->in_len);
        if (ret) return ret;
        return 0;
    }

    /* Handle finalization */
    if (finish) {
        data->state = SHA_STATE_BUSY;

        /* Determine total length and source buffer */
        const uint8_t *src;
        size_t total_bytes;
        if (data->use_accum) {
            src = data->accum_buf;
            total_bytes = data->accum_len;
        } else {
            src = data->buffer;
            total_bytes = data->buffer_len;
        }

        /* Step 1: Configure byte order */
        uint32_t ctrl_reg = SHA_WR_REV_BIT | SHA_RD_REV_BIT;
        sys_write32(ctrl_reg, config->base + SHA_CTR_OFFSET);

        /* Step 2: Program data length (words) and padding */
        uint64_t bits = (uint64_t)total_bytes * 8ULL;
        uint32_t words_lo = (uint32_t)((total_bytes + 3U) / 4U);
        sys_write32(words_lo, config->base + SHA_DATALEN_OFFSET);
        sys_write32(0,       config->base + SHA_DATALEN_5832_OFFSET);

        /* Valid byte encoding per spec [9:8]:
         * 0: all 4 bytes valid (rem=0), 1: [31:24] valid (rem=1),
         * 2: [31:16] valid (rem=2), 3: [31:8] valid (rem=3)
         */
        uint32_t rem = (uint32_t)(total_bytes % 4U);
        uint32_t valid_enc = rem & 0x3U; /* direct mapping: 0..3 */
        uint32_t bmod = (uint32_t)(bits % 512ULL);
        uint32_t pad_packet = (bmod < 448U) ? ((512U - bmod - 64U) / 32U)
                                            : ((512U - bmod + 448U) / 32U);
        uint32_t pad_ctrl = (valid_enc << 8) | (pad_packet & 0x1F);
        sys_write32(pad_ctrl, config->base + SHA_PAD_CTR_OFFSET);

        /* Step 3: Start operation and feed all input words */
        ctrl_reg |= SHA_STR_BIT;
        sys_write32(ctrl_reg, config->base + SHA_CTR_OFFSET);

        uint32_t words_to_write = (uint32_t)((total_bytes + 3U) / 4U);
        uint32_t words_written = 0;
        size_t bytes_written = 0;
        while (words_written < words_to_write) {
            uint32_t w = 0;
            for (int j = 0; j < 4; j++) {
                if (bytes_written < total_bytes) {
                    w |= ((uint32_t)src[bytes_written]) << (j * 8);
                    bytes_written++;
                }
            }
            sys_write32(w, config->base + SHA_IN_OFFSET);

            words_written++;
            if ((words_written % 16U) == 0U) {
                for (int j = 0; j < 6; j++) {
                    __asm__ volatile ("nop");
                }
                while (!(sys_read32(config->base + SHA_CTR_OFFSET) & SHA_READY_BIT)) {}
            }
        }

        /* Validate output buffer (if length provided) */
        if (!pkt->out_buf) {
            LOG_ERR("Null output buffer");
            data->state = SHA_STATE_ERROR;
            return -EINVAL;
        }

        /* Step 4: Wait for completion with timeout */
        uint32_t timeout = 0;
        while (!(sys_read32(config->base + SHA_CTR_OFFSET) & SHA_STA_BIT)) {
            if (timeout++ > CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC) {
                LOG_ERR("Timeout");
                data->state = SHA_STATE_ERROR;
                return -ETIMEDOUT;
            }
            k_busy_wait(1);
        }

        /* Step 5: Clear interrupt and read result */
        sys_write32(sys_read32(config->base + SHA_CTR_OFFSET) | SHA_INT_CLR_BIT,
                    config->base + SHA_CTR_OFFSET);

        uint32_t *output32 = (uint32_t *)pkt->out_buf;
        for (int i = 0; i < 8; i++) {
            output32[i] = sys_read32(config->base + SHA_OUT_OFFSET + i * 4);
        }

        data->state = SHA_STATE_IDLE;
    }

    return 0;
}

static int crypto_em32_hash_begin_session(const struct device *dev,
                                         struct hash_ctx *ctx,
                                         enum hash_algo algo)
{
    struct crypto_em32_data *data = dev->data;

    if (data->session_active) {
        return -EBUSY;
    }

    if (algo != CRYPTO_HASH_ALGO_SHA256) {
        return -ENOTSUP;
    }

    data->ctx = ctx;
    data->state = SHA_STATE_IDLE;

    /* Initialize legacy small-data buffer */
    data->total_len = 0;
    data->buffer_len = 0;

    /* Initialize accumulation buffer state for large data */
    data->accum_buf = NULL;
    data->accum_len = 0;
    data->accum_cap = 0;
    data->use_accum = false;

    data->session_active = true;

    ctx->hash_hndlr = em32_sha256_handler;

    /* Reset and configure hardware */
    sha_reset(dev);
    sha_configure(dev);



    return 0;
}

static int crypto_em32_hash_free_session(const struct device *dev,
                                        struct hash_ctx *ctx)
{
    struct crypto_em32_data *data = dev->data;

    if (!data->session_active || data->ctx != ctx) {
        return -EINVAL;
    }

    /* Clear sensitive data and free accumulation buffer */
    if (data->accum_buf) {
        if (data->accum_len) {
            memset(data->accum_buf, 0, data->accum_len);
        }
        k_free(data->accum_buf);
        data->accum_buf = NULL;
    }
    data->accum_len = 0;
    data->accum_cap = 0;
    data->use_accum = false;
    memset(data->buffer, 0, sizeof(data->buffer));

    data->session_active = false;
    data->ctx = NULL;
    data->state = SHA_STATE_IDLE;



    return 0;
}

#ifdef CONFIG_CRYPTO_EM32_SHA_INTERRUPT
static int crypto_em32_hash_async_callback_set(const struct device *dev,
                                              hash_completion_cb cb)
{
    struct crypto_em32_data *data = dev->data;
    data->callback = cb;
    return 0;
}

static void crypto_em32_isr(const struct device *dev)
{
    struct crypto_em32_data *data = dev->data;
    uint32_t status;

    status = sha_read_reg(dev, SHA_CTR_OFFSET);

    if (status & SHA_STA_BIT) {
        /* Clear interrupt */
        uint32_t ctrl = status | SHA_INT_CLR_BIT;
        sha_write_reg(dev, SHA_CTR_OFFSET, ctrl);

        /* Signal completion */
        k_sem_give(&data->op_complete);

        /* Call callback if set */
        if (data->callback) {
            struct hash_pkt pkt = { .ctx = data->ctx };
            data->callback(&pkt, 0);
        }
    }
}
#endif

static const struct crypto_driver_api crypto_em32_api = {
    .query_hw_caps = crypto_em32_query_hw_caps,
    .hash_begin_session = crypto_em32_hash_begin_session,
    .hash_free_session = crypto_em32_hash_free_session,
#ifdef CONFIG_CRYPTO_EM32_SHA_INTERRUPT
    .hash_async_callback_set = crypto_em32_hash_async_callback_set,
#endif
};

static int crypto_em32_init(const struct device *dev)
{
    const struct crypto_em32_config *cfg = dev->config;
    struct crypto_em32_data *data = dev->data;
    int ret;

    /* Disable crypto clock gate first to enable crypto clock */
    sha_disable_clkgate();

    /* Enable clock using the same pattern as TRNG driver */
    if (cfg->clock_dev) {
        if (!device_is_ready(cfg->clock_dev)) {
            LOG_ERR("Crypto clock device not ready");
            return -ENODEV;
        }

        struct elan_em32_clock_control_subsys clk_subsys = {
            .clock_group = cfg->clock_group_id
        };

        ret = clock_control_on(cfg->clock_dev, &clk_subsys);
        if (ret < 0) {
            LOG_ERR("Failed to enable clock: %d", ret);
            return ret;
        }

    }

    /* Initialize data structure */
    data->session_active = false;
    data->state = SHA_STATE_IDLE;
    data->ctx = NULL;
    data->callback = NULL;

#ifdef CONFIG_CRYPTO_EM32_SHA_INTERRUPT
    k_sem_init(&data->op_complete, 0, 1);

    /* Configure interrupts */
    if (cfg->irq_config_func) {
        cfg->irq_config_func(dev);
    }
#endif

    /* Small delay to ensure clocks are stable */
    k_msleep(10);

    /* Reset hardware */
    sha_reset(dev);



    return 0;
}

#define CRYPTO_EM32_INIT(n)                                                    \
    IF_ENABLED(CONFIG_CRYPTO_EM32_SHA_INTERRUPT,                              \
              (static void crypto_em32_irq_config_##n(const struct device *dev);)) \
                                                                               \
    static const struct crypto_em32_config crypto_em32_config_##n = {         \
        .base = DT_INST_REG_ADDR(n),                                         \
        .clock_dev = DEVICE_DT_GET_OR_NULL(DT_INST_CLOCKS_CTLR(n)),          \
        .clock_group_id = HCLKG_ENCRYPT, \
        IF_ENABLED(CONFIG_CRYPTO_EM32_SHA_INTERRUPT,                          \
                  (.irq_config_func = crypto_em32_irq_config_##n,))           \
    };                                                                         \
                                                                               \
    static struct crypto_em32_data crypto_em32_data_##n;                      \
                                                                               \
    DEVICE_DT_INST_DEFINE(n, crypto_em32_init, NULL,                         \
                         &crypto_em32_data_##n, &crypto_em32_config_##n,      \
                         POST_KERNEL, CONFIG_CRYPTO_INIT_PRIORITY,             \
                         &crypto_em32_api);                                    \
                                                                               \
    IF_ENABLED(CONFIG_CRYPTO_EM32_SHA_INTERRUPT,                              \
              (static void crypto_em32_irq_config_##n(const struct device *dev) \
               {                                                               \
                   IRQ_CONNECT(DT_INST_IRQN(n), DT_INST_IRQ(n, priority),    \
                              crypto_em32_isr, DEVICE_DT_INST_GET(n), 0);     \
                   irq_enable(DT_INST_IRQN(n));                              \
               }))

DT_INST_FOREACH_STATUS_OKAY(CRYPTO_EM32_INIT)
