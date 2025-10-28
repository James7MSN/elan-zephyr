/*
 * Copyright (c) 2024 Elan Microelectronics Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Large Data EC Communication Simulation Test
 * Simulates Chrome EC sending >400KB firmware data in 64KB chunks
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/crypto/crypto.h>
#include <zephyr/crypto/hash.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>

LOG_MODULE_REGISTER(sha_large_data_test, LOG_LEVEL_INF);
/* Driver helper to inform total message length for chunked mode */
extern int crypto_em32_sha_set_total_length(const struct device *dev, size_t total_bytes);


/* Test data size: 400KB (multiple of 64 bytes, no padding needed) */
#define TEST_DATA_SIZE      (400 * 1024)  /* 400KB */
#define CHUNK_SIZE          (64 * 1024)   /* 64KB chunks */
#define NUM_CHUNKS          ((TEST_DATA_SIZE + CHUNK_SIZE - 1) / CHUNK_SIZE)

/* ========================================
 * SHA256 VERIFICATION PATTERN
 * ========================================
 * Test Pattern: 400KB (409600 bytes) of deterministic data
 * Pattern: byte[i] = (offset + i) & 0xFF
 *
 * IMPORTANT: Test size is multiple of 64 bytes (512 bits)
 * This means NO PADDING is needed - simplifies debugging!
 *
 * CORRECT Expected SHA256 Hash (verified with Python):
 * 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
 *
 * Verification Method:
 * 1. Run Python script to verify:
 *    import hashlib
 *    data = bytearray(409600)
 *    for i in range(409600): data[i] = i & 0xFF
 *    print(hashlib.sha256(data).hexdigest())
 * 2. Compare board output with expected hash
 * 3. If hashes match: SHA256 implementation is correct
 * ======================================== */
#define EXPECTED_SHA256_HASH "870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee"

/* Expected SHA-256 for padding tests with pattern data[i] = i & 0xFF */
#define EXPECTED_SHA256_409601 "0ae9a6992c813d5cc36e2e6486abe52f66181996eb160677e8f4d86dc5ab2dc5"
#define EXPECTED_SHA256_409602 "eb1b5c717787585e081a6a49175f7b744db3f072c75413aef3e8f756518c1238"
#define EXPECTED_SHA256_409603 "759b9d2304a64cdf25679501b73cbc89ee612a3eb63aa56b85b888630cf313e9"


static const struct device *crypto_dev = DEVICE_DT_GET(DT_NODELABEL(crypto0));

/* Generate deterministic test data pattern */
static void generate_test_data(uint8_t *buf, size_t len, size_t offset)
{
    for (size_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)((offset + i) & 0xFF);
    }
}

/* Print hash in hex format and verify against expected pattern */
static void print_hash(const uint8_t *hash, size_t len)
{
    char hash_str[65];
    char *ptr = hash_str;

    for (size_t i = 0; i < len; i++) {
        ptr += snprintf(ptr, 3, "%02x", hash[i]);
    }

    LOG_INF("Hash: %s", hash_str);

    /* Verify against expected SHA256 pattern */
    if (strcmp(hash_str, EXPECTED_SHA256_HASH) == 0) {
        LOG_INF("✅ VERIFICATION PASSED - Hash matches expected pattern!");
    } else {
        LOG_WRN("⚠️  VERIFICATION FAILED - Hash does NOT match expected pattern");
        LOG_WRN("Expected: %s", EXPECTED_SHA256_HASH);
        LOG_WRN("Got:      %s", hash_str);
    }
}

/* Print hash and verify against a provided expected hex string.
 * Returns 0 on match, -EIO on mismatch.
 */
static int verify_hash_with_expected(const uint8_t *hash, size_t len, const char *expected_hex)
{
    char hash_str[65];
    char *ptr = hash_str;

    for (size_t i = 0; i < len; i++) {
        ptr += snprintf(ptr, 3, "%02x", hash[i]);
    }

    LOG_INF("Hash: %s", hash_str);

    if (strcmp(hash_str, expected_hex) == 0) {
        LOG_INF("✅ VERIFICATION PASSED - Hash matches expected pattern!");
        return 0;
    } else {
        LOG_WRN("⚠️  VERIFICATION FAILED - Hash does NOT match expected pattern");
        LOG_WRN("Expected: %s", expected_hex);
        LOG_WRN("Got:      %s", hash_str);
        return -EIO;
    }
}


/* Test 1: Chunked 400KB hash (simulating EC communication) */
static int test_single_shot_400kb(void)
{
    struct hash_ctx ctx;
    struct hash_pkt pkt;
    uint8_t hash_output[32];
    uint8_t *chunk_buf;
    size_t offset = 0;
    int ret;

    LOG_INF("=== Test 1: Chunked 400KB Hash (EC Communication Pattern) ===");

    /* Allocate only one chunk buffer (64KB) */
    chunk_buf = k_malloc(CHUNK_SIZE);
    if (!chunk_buf) {
        LOG_ERR("Failed to allocate %u bytes for chunk", CHUNK_SIZE);
        return -ENOMEM;
    }

    /* Begin session */
    memset(&ctx, 0, sizeof(ctx));
    ctx.flags = CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;
    ret = hash_begin_session(crypto_dev, &ctx, CRYPTO_HASH_ALGO_SHA256);
    if (ret) {
        LOG_ERR("Failed to begin session: %d", ret);
        k_free(chunk_buf);
        return ret;
    }
    /* Inform driver of full message size for chunked mode */
    (void)crypto_em32_sha_set_total_length(crypto_dev, TEST_DATA_SIZE);


    /* Process data in 64KB chunks */
    LOG_INF("Processing %u bytes in %u-byte chunks", TEST_DATA_SIZE, CHUNK_SIZE);
    while (offset < TEST_DATA_SIZE) {
        size_t this_chunk = (TEST_DATA_SIZE - offset < CHUNK_SIZE) ?
                           (TEST_DATA_SIZE - offset) : CHUNK_SIZE;

        /* Generate test data for this chunk */
        generate_test_data(chunk_buf, this_chunk, offset);

        /* Send chunk to driver */
        memset(&pkt, 0, sizeof(pkt));
        pkt.in_buf = chunk_buf;
        pkt.in_len = this_chunk;
        pkt.out_buf = hash_output;

        ret = hash_update(&ctx, &pkt);
        if (ret) {
            LOG_ERR("Failed to update hash at offset %zu: %d", offset, ret);
            hash_free_session(crypto_dev, &ctx);
            k_free(chunk_buf);
            return ret;
        }

        LOG_DBG("Processed chunk at offset %zu, size %zu", offset, this_chunk);
        offset += this_chunk;
    }

    /* Finalize with zero-length input */
    memset(&pkt, 0, sizeof(pkt));
    pkt.in_buf = NULL;
    pkt.in_len = 0;
    pkt.out_buf = hash_output;

    ret = ctx.hash_hndlr(&ctx, &pkt, true);
    if (ret) {
        LOG_ERR("Failed to finalize hash: %d", ret);
        hash_free_session(crypto_dev, &ctx);
        k_free(chunk_buf);
        return ret;
    }

    LOG_INF("Chunked 400KB hash completed successfully");
    print_hash(hash_output, 32);

    hash_free_session(crypto_dev, &ctx);
    k_free(chunk_buf);
    return 0;
}

/* Test 2: EC-style chunked transfer (64KB chunks) */
static int test_ec_chunked_transfer(void)
{
    struct hash_ctx ctx;
    struct hash_pkt pkt;
    uint8_t hash_output[32];
    uint8_t *chunk_buf;
    int ret;
    size_t offset = 0;
    int chunk_num = 0;

    LOG_INF("=== Test 2: EC-style Chunked Transfer (64KB chunks) ===");
    LOG_INF("Total data: %u bytes, Chunk size: %u bytes, Num chunks: %u",
            TEST_DATA_SIZE, CHUNK_SIZE, NUM_CHUNKS);

    chunk_buf = k_malloc(CHUNK_SIZE);
    if (!chunk_buf) {
        LOG_ERR("Failed to allocate chunk buffer (%u bytes)", CHUNK_SIZE);
        return -ENOMEM;
    }

    /* Begin session (EC init phase) */
    memset(&ctx, 0, sizeof(ctx));
    ctx.flags = CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;
    ret = hash_begin_session(crypto_dev, &ctx, CRYPTO_HASH_ALGO_SHA256);
    if (ret) {
        LOG_ERR("Failed to begin session: %d", ret);
        k_free(chunk_buf);
        return ret;
    }
    /* Inform driver of full message size for chunked mode */
    (void)crypto_em32_sha_set_total_length(crypto_dev, TEST_DATA_SIZE);


    LOG_INF("Session started (EC init phase)");

    /* EC update phase: send data in 64KB chunks */
    while (offset < TEST_DATA_SIZE) {
        size_t remaining = TEST_DATA_SIZE - offset;
        size_t this_chunk = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;

        chunk_num++;
        LOG_INF("Processing chunk %d: offset=%zu, size=%zu", chunk_num, offset, this_chunk);

        /* Generate chunk data */
        generate_test_data(chunk_buf, this_chunk, offset);

        /* Send chunk to driver */
        memset(&pkt, 0, sizeof(pkt));
        pkt.in_buf = chunk_buf;
        pkt.in_len = this_chunk;
        pkt.out_buf = hash_output;

        ret = hash_update(&ctx, &pkt);
        if (ret) {
            LOG_ERR("Failed to update hash at chunk %d: %d", chunk_num, ret);
            hash_free_session(crypto_dev, &ctx);
            k_free(chunk_buf);
            return ret;
        }

        LOG_DBG("Chunk %d processed successfully", chunk_num);
        offset += this_chunk;

        /* Small delay between chunks to simulate EC communication */
        k_msleep(10);
    }

    LOG_INF("All %d chunks sent, finalizing hash (EC final phase)...", chunk_num);

    /* EC final phase: finalize with zero-length input */
    memset(&pkt, 0, sizeof(pkt));
    pkt.in_buf = NULL;
    pkt.in_len = 0;
    pkt.out_buf = hash_output;

    ret = ctx.hash_hndlr(&ctx, &pkt, true);
    if (ret) {
        LOG_ERR("Failed to finalize hash: %d", ret);
        hash_free_session(crypto_dev, &ctx);
        k_free(chunk_buf);
        return ret;
    }

    LOG_INF("EC-style chunked transfer completed successfully");
    print_hash(hash_output, 32);

    hash_free_session(crypto_dev, &ctx);
    k_free(chunk_buf);
    return 0;
}

/* Test 3: Chunked processing verification */
static int test_consistency_check(void)
{
    struct hash_ctx ctx;
    struct hash_pkt pkt;
    uint8_t hash_output[32];
    uint8_t *chunk_buf;
    int ret;
    size_t offset = 0;
    int chunk_count = 0;

    LOG_INF("=== Test 3: Chunked Processing Verification ===");

    /* Allocate only chunk buffer */
    chunk_buf = k_malloc(CHUNK_SIZE);
    if (!chunk_buf) {
        LOG_ERR("Failed to allocate chunk buffer");
        return -ENOMEM;
    }

    LOG_INF("Verifying chunked processing with %u bytes in %u-byte chunks",
            TEST_DATA_SIZE, CHUNK_SIZE);

    /* Begin session */
    memset(&ctx, 0, sizeof(ctx));
    ctx.flags = CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;
    ret = hash_begin_session(crypto_dev, &ctx, CRYPTO_HASH_ALGO_SHA256);
    if (ret) {
        LOG_ERR("Failed to begin session: %d", ret);
        k_free(chunk_buf);
        return ret;
    }
    /* Inform driver of full message size for chunked mode */
    (void)crypto_em32_sha_set_total_length(crypto_dev, TEST_DATA_SIZE);


    /* Process all chunks */
    offset = 0;
    while (offset < TEST_DATA_SIZE) {
        size_t remaining = TEST_DATA_SIZE - offset;
        size_t this_chunk = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;

        chunk_count++;
        LOG_DBG("Chunk %d: offset=%zu, size=%zu", chunk_count, offset, this_chunk);

        /* Generate test data for this chunk */
        generate_test_data(chunk_buf, this_chunk, offset);

        /* Send chunk to driver */
        memset(&pkt, 0, sizeof(pkt));
        pkt.in_buf = chunk_buf;
        pkt.in_len = this_chunk;
        pkt.out_buf = hash_output;

        ret = hash_update(&ctx, &pkt);
        if (ret) {
            LOG_ERR("Failed to update hash at chunk %d: %d", chunk_count, ret);
            hash_free_session(crypto_dev, &ctx);
            k_free(chunk_buf);
            return ret;
        }

        offset += this_chunk;
    }

    /* Finalize */
    memset(&pkt, 0, sizeof(pkt));
    pkt.in_buf = NULL;
    pkt.in_len = 0;
    pkt.out_buf = hash_output;

    ret = ctx.hash_hndlr(&ctx, &pkt, true);
    if (ret) {
        LOG_ERR("Failed to finalize hash: %d", ret);
        hash_free_session(crypto_dev, &ctx);
        k_free(chunk_buf);
        return ret;
    }

    hash_free_session(crypto_dev, &ctx);
    LOG_INF("✓ Chunked processing verification PASSED - processed %d chunks successfully", chunk_count);
    print_hash(hash_output, 32);
    k_free(chunk_buf);
    return 0;
}

/* Test N: Padding verification for arbitrary total_size (e.g., 400KB+1/2/3) */
static int test_padding_case(size_t total_size, const char *expected_hex, const char *label)
{
    struct hash_ctx ctx;
    struct hash_pkt pkt;
    uint8_t hash_output[32];
    uint8_t *chunk_buf;
    size_t offset = 0;
    int ret;

    LOG_INF("=== Padding Test: %s ===", label);
    LOG_INF("Total data: %zu bytes, Chunk size: %u bytes, Num chunks: %zu",
            total_size, CHUNK_SIZE,
            (total_size + CHUNK_SIZE - 1) / CHUNK_SIZE);

    chunk_buf = k_malloc(CHUNK_SIZE);
    if (!chunk_buf) {
        LOG_ERR("Failed to allocate chunk buffer (%u bytes)", CHUNK_SIZE);
        return -ENOMEM;
    }

    /* Begin session */
    memset(&ctx, 0, sizeof(ctx));
    ctx.flags = CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;
    ret = hash_begin_session(crypto_dev, &ctx, CRYPTO_HASH_ALGO_SHA256);
    if (ret) {
        LOG_ERR("Failed to begin session: %d", ret);
        k_free(chunk_buf);
        return ret;
    }
    /* Inform driver of full message size for chunked mode */
    (void)crypto_em32_sha_set_total_length(crypto_dev, total_size);

    /* Process data in CHUNK_SIZE chunks */
    while (offset < total_size) {
        size_t remaining = total_size - offset;
        size_t this_chunk = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;

        /* Generate test data for this chunk */
        generate_test_data(chunk_buf, this_chunk, offset);

        /* Send chunk to driver */
        memset(&pkt, 0, sizeof(pkt));
        pkt.in_buf = chunk_buf;
        pkt.in_len = this_chunk;
        pkt.out_buf = hash_output;

        ret = hash_update(&ctx, &pkt);
        if (ret) {
            LOG_ERR("Failed to update hash at offset %zu: %d", offset, ret);
            hash_free_session(crypto_dev, &ctx);
            k_free(chunk_buf);
            return ret;
        }

        offset += this_chunk;
    }

    /* Finalize with zero-length input */
    memset(&pkt, 0, sizeof(pkt));
    pkt.in_buf = NULL;
    pkt.in_len = 0;
    pkt.out_buf = hash_output;

    ret = ctx.hash_hndlr(&ctx, &pkt, true);
    if (ret) {
        LOG_ERR("Failed to finalize hash: %d", ret);
        hash_free_session(crypto_dev, &ctx);
        k_free(chunk_buf);
        return ret;
    }

    /* Verify padding correctness by comparing against expected hex */
    ret = verify_hash_with_expected(hash_output, 32, expected_hex);

    hash_free_session(crypto_dev, &ctx);
    k_free(chunk_buf);
    return ret;
}


int main(void)
{
    int ret;
    int passed = 0;
    int failed = 0;

    LOG_INF("========================================");
    LOG_INF("Large Data EC Communication Simulation");
    LOG_INF("Test Data Size: %u bytes (400KB)", TEST_DATA_SIZE);
    LOG_INF("Chunk Size: %u bytes (64KB)", CHUNK_SIZE);
    LOG_INF("Number of Chunks: %u", NUM_CHUNKS);
    LOG_INF("========================================");

    if (!device_is_ready(crypto_dev)) {
        LOG_ERR("Crypto device not ready");
        return -ENODEV;
    }

    /* Test 1: Single-shot */
    ret = test_single_shot_400kb();
    if (ret) {
        LOG_ERR("Test 1 FAILED");
        failed++;
    } else {
        LOG_INF("Test 1 PASSED");
        passed++;
    }
    k_msleep(500);

    /* Test 2: EC-style chunked */
    ret = test_ec_chunked_transfer();
    if (ret) {
        LOG_ERR("Test 2 FAILED");
        failed++;
    } else {
        LOG_INF("Test 2 PASSED");
        passed++;
    }
    k_msleep(500);

    /* Test 3: Consistency */
    ret = test_consistency_check();
    if (ret) {
        LOG_ERR("Test 3 FAILED");
        failed++;
    } else {
        LOG_INF("Test 3 PASSED");
        passed++;
    }

    /* Test 4: Padding 400KB + 1 byte */
    ret = test_padding_case(409601, EXPECTED_SHA256_409601, "409,601 bytes (400KB + 1)");
    if (ret) {
        LOG_ERR("Test 4 FAILED");
        failed++;
    } else {
        LOG_INF("Test 4 PASSED");
        passed++;
    }
    k_msleep(200);

    /* Test 5: Padding 400KB + 2 bytes */
    ret = test_padding_case(409602, EXPECTED_SHA256_409602, "409,602 bytes (400KB + 2)");
    if (ret) {
        LOG_ERR("Test 5 FAILED");
        failed++;
    } else {
        LOG_INF("Test 5 PASSED");
        passed++;
    }
    k_msleep(200);

    /* Test 6: Padding 400KB + 3 bytes */
    ret = test_padding_case(409603, EXPECTED_SHA256_409603, "409,603 bytes (400KB + 3)");
    if (ret) {
        LOG_ERR("Test 6 FAILED");
        failed++;
    } else {
        LOG_INF("Test 6 PASSED");
        passed++;
    }


    LOG_INF("========================================");
    LOG_INF("Test Summary: %d passed, %d failed", passed, failed);
    LOG_INF("========================================");

    return (failed == 0) ? 0 : -1;
}

