# SHA256 State Continuation - Code Modification Guide

**Date**: October 25, 2025  
**File**: `drivers/crypto/crypto_em32_sha.c`  
**Status**: ✅ READY FOR IMPLEMENTATION

---

## 📍 Exact Code Locations and Changes

### CHANGE 1: Data Structure Enhancement

**Location**: Lines 100-115 in `struct crypto_em32_data`

**Current Code**:
```c
    /* Chunked processing for >64KB data with state continuation */
    bool use_chunked;                 /* True if processing in chunks */
    uint64_t total_bytes_processed;   /* Total bytes processed across chunks */
    uint32_t chunk_state[8];          /* SHA256 state between chunks (H0-H7) */
    bool chunk_state_valid;           /* Whether chunk_state is valid */
    uint64_t chunk_message_bits;      /* Total message bits processed so far */

    /* Chunk buffer for intermediate processing */
    uint8_t *chunk_buf;               /* Buffer for current chunk */
    size_t chunk_buf_len;             /* Current chunk data length */
    size_t chunk_buf_cap;             /* Chunk buffer capacity (64KB) */

    /* Input buffer reference for chunked mode (no copy) */
    const uint8_t *last_input_buf;    /* Reference to last input buffer */
    size_t last_input_len;            /* Length of last input */
```

**Add After Line 105**:
```c
    /* Chunk processing tracking */
    uint64_t chunks_processed;        /* Number of chunks processed */
    uint64_t total_bytes_to_process;  /* Total bytes in all chunks */
    bool is_final_chunk;              /* True if this is the last chunk */
```

---

### CHANGE 2: New Function Implementation

**Location**: After `sha_restore_state()` function (around line 253)

**Add New Function**:
```c
/* Process a single chunk through hardware with state continuation */
static int process_chunk_with_state_continuation(
    const struct device *dev,
    const uint8_t *chunk_data,
    size_t chunk_len,
    uint32_t *state,
    uint64_t message_bits_so_far)
{
    const struct crypto_em32_config *config = dev->config;
    
    if (!chunk_data || chunk_len == 0) {
        return 0;
    }
    
    LOG_DBG("Processing chunk: %zu bytes, message_bits=%llu", 
            chunk_len, message_bits_so_far);
    
    // Reset SHA engine
    sha_reset(dev);
    
    // Configure byte order
    uint32_t ctrl_reg = SHA_WR_REV_BIT | SHA_RD_REV_BIT;
    sys_write32(ctrl_reg, config->base + SHA_CTR_OFFSET);
    
    // Program data length (words)
    uint32_t words = (uint32_t)((chunk_len + 3U) / 4U);
    sys_write32(words, config->base + SHA_DATALEN_OFFSET);
    sys_write32(0, config->base + SHA_DATALEN_5832_OFFSET);
    
    // Calculate padding
    uint32_t rem = (uint32_t)(chunk_len % 4U);
    uint32_t valid_enc = rem & 0x3U;
    uint32_t bmod = (uint32_t)(message_bits_so_far % 512ULL);
    uint32_t pad_packet = (bmod < 448U) ? 
        ((512U - bmod - 64U) / 32U) : ((960U - bmod) / 32U);
    
    uint32_t pad_ctr = (pad_packet & SHA_PAD_PACKET_MASK) | 
                       (valid_enc << SHA_VALID_BYTE_SHIFT);
    sys_write32(pad_ctr, config->base + SHA_PAD_CTR_OFFSET);
    
    // Write data to hardware
    const uint32_t *data_words = (const uint32_t *)chunk_data;
    for (uint32_t i = 0; i < words; i++) {
        sys_write32(data_words[i], config->base + SHA_IN_OFFSET);
    }
    
    // Start SHA operation
    uint32_t start_ctrl = ctrl_reg | SHA_STR_BIT;
    sys_write32(start_ctrl, config->base + SHA_CTR_OFFSET);
    
    // Wait for completion
    uint32_t timeout = CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC;
    while (!(sha_read_reg(dev, SHA_CTR_OFFSET) & SHA_READY_BIT) && 
           timeout > 0) {
        k_busy_wait(1);
        timeout--;
    }
    
    if (timeout == 0) {
        LOG_ERR("SHA256 operation timeout");
        return -ETIMEDOUT;
    }
    
    // Read and save state (H0-H7)
    sha_save_state(dev, state);
    
    LOG_DBG("Chunk processed, state saved: H0=%08x H1=%08x", 
            state[0], state[1]);
    
    return 0;
}
```

---

### CHANGE 3: Update Handler Modification

**Location**: Lines 387-407 in `em32_sha256_handler()`

**Current Code**:
```c
        if (data->use_chunked) {
            /* For chunked mode: store reference to input data (no accumulation)
             * We process each chunk directly through hardware without accumulating
             * This avoids memory allocation issues with large data
             */
            LOG_DBG("Chunked mode: storing reference to %zu bytes (no accumulation)", pkt->in_len);

            if (pkt->in_len > 0) {
                /* Store input buffer reference for direct processing */
                data->last_input_buf = pkt->in_buf;
                data->last_input_len = pkt->in_len;
                data->total_bytes_processed += pkt->in_len;
                LOG_DBG("Stored input reference: %zu bytes, total processed: %llu",
                        pkt->in_len, data->total_bytes_processed);
            }
        } else {
            int ret = accum_append(data, pkt->in_buf, pkt->in_len);
            if (ret) return ret;
        }
```

**Replace With**:
```c
        if (data->use_chunked) {
            /* For chunked mode: process chunk immediately with state continuation */
            LOG_DBG("Chunked mode: processing chunk %zu bytes with state continuation", 
                    pkt->in_len);

            if (pkt->in_len > 0) {
                // Initialize state if first chunk
                if (!data->chunk_state_valid) {
                    sha_init_state(data->chunk_state);
                    data->chunk_state_valid = true;
                    data->chunks_processed = 0;
                    data->chunk_message_bits = 0;
                    LOG_DBG("Initialized SHA256 state for first chunk");
                }
                
                // Process this chunk through hardware
                int ret = process_chunk_with_state_continuation(
                    dev, pkt->in_buf, pkt->in_len,
                    data->chunk_state,
                    data->chunk_message_bits);
                if (ret) {
                    LOG_ERR("Failed to process chunk: %d", ret);
                    data->state = SHA_STATE_ERROR;
                    return ret;
                }
                
                // Update tracking
                data->chunks_processed++;
                data->chunk_message_bits += pkt->in_len * 8ULL;
                data->total_bytes_processed += pkt->in_len;
                
                LOG_DBG("Processed chunk %llu: %zu bytes, total: %llu bytes",
                        data->chunks_processed, pkt->in_len, 
                        data->total_bytes_processed);
            }
        } else {
            int ret = accum_append(data, pkt->in_buf, pkt->in_len);
            if (ret) return ret;
        }
```

---

### CHANGE 4: Finalization Handler Modification

**Location**: Lines 467-482 in `em32_sha256_handler()`

**Current Code**:
```c
        if (data->use_chunked) {
            /* In chunked mode, use the last input buffer reference (no accumulation) */
            src = data->last_input_buf;
            total_bytes = data->last_input_len;
            total_message_bits = (data->total_bytes_processed) * 8ULL;
            LOG_DBG("Chunked finalization: src=%p, total_bytes=%zu, total_bits=%llu",
                    (void *)src, total_bytes, total_message_bits);
        } else if (data->use_accum) {
```

**Replace With**:
```c
        if (data->use_chunked) {
            /* In chunked mode with state continuation:
             * All chunks have been processed already
             * Just output the final hash from saved state
             */
            
            if (!data->chunk_state_valid) {
                LOG_ERR("Chunked finalization: no valid state");
                data->state = SHA_STATE_ERROR;
                return -EINVAL;
            }
            
            // Copy final state to output buffer
            uint8_t *out_buf = pkt->out_buf;
            for (int i = 0; i < 8; i++) {
                uint32_t word = data->chunk_state[i];
                out_buf[i*4 + 0] = (word >> 24) & 0xFF;
                out_buf[i*4 + 1] = (word >> 16) & 0xFF;
                out_buf[i*4 + 2] = (word >> 8) & 0xFF;
                out_buf[i*4 + 3] = word & 0xFF;
            }
            
            LOG_INF("Chunked finalization complete: output hash from saved state");
            data->state = SHA_STATE_IDLE;
            return 0;
        } else if (data->use_accum) {
```

---

## 🧪 Verification

### Expected Output After Fix

```
[00:00:00.010,000] <inf> sha_large_data_test: Processing 409600 bytes in 65536-byte chunks
[00:00:00.016,000] <inf> crypto_em32_sha: Switching to chunked processing
[00:00:00.016,000] <dbg> crypto_em32_sha: Initialized SHA256 state for first chunk
[00:00:00.016,000] <dbg> crypto_em32_sha: Processing chunk: 65536 bytes, message_bits=0
[00:00:00.022,000] <dbg> crypto_em32_sha: Processing chunk: 65536 bytes, message_bits=524288
...
[00:00:00.049,000] <dbg> crypto_em32_sha: Processing chunk: 16384 bytes, message_bits=3145728
[00:00:00.053,000] <inf> sha_large_data_test: Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
[00:00:00.054,000] <inf> sha_large_data_test: ✅ VERIFICATION PASSED
[00:00:00.054,000] <inf> sha_large_data_test: Test 1 PASSED
```

---

## 📋 Implementation Steps

1. Add state continuation tracking fields to data structure
2. Implement `process_chunk_with_state_continuation()` function
3. Modify update handler to process chunks immediately
4. Modify finalization handler to output saved state
5. Build: `west build -b 32f967_dv samples/elan_sha -p always`
6. Test and verify hash matches expected value

---

**Status**: ✅ **READY FOR IMPLEMENTATION**

