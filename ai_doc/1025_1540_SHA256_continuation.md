# SHA256 State Continuation Implementation Plan

**Date**: October 25, 2025  
**Status**: ✅ ANALYSIS COMPLETE - IMPLEMENTATION PLAN READY  
**Version**: 1.0

---

## 🎯 Problem Analysis

### Current Issue

The driver stores only a reference to the **LAST chunk** and processes only that:

```c
// In update handler (called for each chunk)
data->last_input_buf = pkt->in_buf;  // ← OVERWRITES previous!
data->last_input_len = pkt->in_len;

// In finalization
src = data->last_input_buf;  // ← Only has LAST chunk!
total_bytes = data->last_input_len;
```

**Result**: Only the last 16KB chunk is processed, producing wrong hash.

### Console Log Evidence

```
[00:00:00.049,000] <dbg> crypto_em32_sha: Chunked finalization: src=0x20005a70, total_bytes=16384, total_bits=3276800
```

- `total_bytes=16384` = 16KB (LAST chunk only!)
- `total_bits=3276800` = 409600 * 8 (correct total, but WRONG data!)

---

## ✅ Solution: State Continuation

### How It Works

```
For each chunk:
  1. Load previous SHA256 state (H0-H7)
     - Or initial values for first chunk
  2. Process chunk through hardware SHA256
  3. Save state (H0-H7) after processing
  4. Continue to next chunk

Final operation:
  1. Load final state
  2. Process padding and finalization
  3. Output final hash
```

### Benefits

- ✅ No memory allocation issues
- ✅ Process one chunk at a time
- ✅ Fits in 160KB available RAM
- ✅ Produces correct hash
- ✅ Supports arbitrary data sizes

---

## 🔧 Code Modifications Required

### File: `drivers/crypto/crypto_em32_sha.c`

#### 1. Data Structure Enhancement (Lines 84-120)

**Current**:
```c
struct crypto_em32_data {
    ...
    uint32_t chunk_state[8];          /* SHA256 state between chunks (H0-H7) */
    bool chunk_state_valid;           /* Whether chunk_state is valid */
    uint64_t chunk_message_bits;      /* Total message bits processed so far */
    
    const uint8_t *last_input_buf;    /* Reference to last input buffer */
    size_t last_input_len;            /* Length of last input */
    ...
};
```

**Needed Addition**:
```c
struct crypto_em32_data {
    ...
    /* State continuation tracking */
    uint32_t chunk_state[8];          /* SHA256 state between chunks (H0-H7) */
    bool chunk_state_valid;           /* Whether chunk_state is valid */
    uint64_t chunk_message_bits;      /* Total message bits processed so far */
    
    /* Chunk processing tracking */
    uint64_t chunks_processed;        /* Number of chunks processed */
    uint64_t total_bytes_to_process;  /* Total bytes in all chunks */
    bool is_final_chunk;              /* True if this is the last chunk */
    ...
};
```

#### 2. Update Handler Modification (Lines 387-407)

**Current Problem**:
```c
if (data->use_chunked) {
    /* Store reference to input data (no accumulation) */
    data->last_input_buf = pkt->in_buf;  // ← OVERWRITES!
    data->last_input_len = pkt->in_len;
    data->total_bytes_processed += pkt->in_len;
}
```

**Required Change**:
```c
if (data->use_chunked) {
    /* Process chunk immediately through hardware with state continuation */
    
    // 1. Initialize state if first chunk
    if (!data->chunk_state_valid) {
        sha_init_state(data->chunk_state);
        data->chunk_state_valid = true;
        data->chunks_processed = 0;
        data->chunk_message_bits = 0;
    }
    
    // 2. Process this chunk through hardware
    int ret = process_chunk_with_state_continuation(dev, pkt->in_buf, pkt->in_len, 
                                                     data->chunk_state, 
                                                     data->chunk_message_bits);
    if (ret) {
        LOG_ERR("Failed to process chunk: %d", ret);
        return ret;
    }
    
    // 3. Update tracking
    data->chunks_processed++;
    data->chunk_message_bits += pkt->in_len * 8ULL;
    data->total_bytes_processed += pkt->in_len;
    
    LOG_DBG("Processed chunk %llu: %zu bytes, total: %llu bytes",
            data->chunks_processed, pkt->in_len, data->total_bytes_processed);
}
```

#### 3. New Function: Process Chunk with State Continuation

**Location**: After `sha_restore_state()` function (around line 253)

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
    
    LOG_DBG("Processing chunk: %zu bytes, message_bits=%llu", chunk_len, message_bits_so_far);
    
    // Step 1: Reset SHA engine
    sha_reset(dev);
    
    // Step 2: Configure byte order
    uint32_t ctrl_reg = SHA_WR_REV_BIT | SHA_RD_REV_BIT;
    sys_write32(ctrl_reg, config->base + SHA_CTR_OFFSET);
    
    // Step 3: Program data length (words)
    uint32_t words = (uint32_t)((chunk_len + 3U) / 4U);
    sys_write32(words, config->base + SHA_DATALEN_OFFSET);
    sys_write32(0, config->base + SHA_DATALEN_5832_OFFSET);
    
    // Step 4: Calculate padding
    uint32_t rem = (uint32_t)(chunk_len % 4U);
    uint32_t valid_enc = rem & 0x3U;
    uint32_t bmod = (uint32_t)(message_bits_so_far % 512ULL);
    uint32_t pad_packet = (bmod < 448U) ? ((512U - bmod - 64U) / 32U) : ((960U - bmod) / 32U);
    
    uint32_t pad_ctr = (pad_packet & SHA_PAD_PACKET_MASK) | (valid_enc << SHA_VALID_BYTE_SHIFT);
    sys_write32(pad_ctr, config->base + SHA_PAD_CTR_OFFSET);
    
    // Step 5: Write data to hardware
    const uint32_t *data_words = (const uint32_t *)chunk_data;
    for (uint32_t i = 0; i < words; i++) {
        sys_write32(data_words[i], config->base + SHA_IN_OFFSET);
    }
    
    // Step 6: Start SHA operation
    uint32_t start_ctrl = ctrl_reg | SHA_STR_BIT;
    sys_write32(start_ctrl, config->base + SHA_CTR_OFFSET);
    
    // Step 7: Wait for completion
    uint32_t timeout = CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC;
    while (!(sha_read_reg(dev, SHA_CTR_OFFSET) & SHA_READY_BIT) && timeout > 0) {
        k_busy_wait(1);
        timeout--;
    }
    
    if (timeout == 0) {
        LOG_ERR("SHA256 operation timeout");
        return -ETIMEDOUT;
    }
    
    // Step 8: Read and save state (H0-H7)
    sha_save_state(dev, state);
    
    LOG_DBG("Chunk processed, state saved: H0=%08x H1=%08x", state[0], state[1]);
    
    return 0;
}
```

#### 4. Finalization Handler Modification (Lines 410-482)

**Current Problem**:
```c
if (data->use_chunked) {
    src = data->last_input_buf;  // ← Only LAST chunk!
    total_bytes = data->last_input_len;
    total_message_bits = (data->total_bytes_processed) * 8ULL;
}
```

**Required Change**:
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
}
```

---

## 📊 Processing Flow

### Current (Broken)

```
Chunk 1 (65KB) → Store reference
Chunk 2 (65KB) → Store reference (OVERWRITES Chunk 1)
Chunk 3 (65KB) → Store reference (OVERWRITES Chunk 2)
...
Chunk 7 (16KB) → Store reference (OVERWRITES Chunk 6)
Finalize → Process only Chunk 7 (16KB)
Result: WRONG HASH
```

### New (Fixed)

```
Chunk 1 (65KB) → Process through hardware → Save state
Chunk 2 (65KB) → Load state → Process through hardware → Save state
Chunk 3 (65KB) → Load state → Process through hardware → Save state
...
Chunk 7 (16KB) → Load state → Process through hardware → Save state
Finalize → Output final state as hash
Result: CORRECT HASH
```

---

## 🧪 Testing Strategy

### Test Cases

1. **Single Chunk**: 16KB data
   - Expected: Correct hash
   - Verify: State continuation works for single chunk

2. **Multiple Chunks**: 400KB data in 65KB chunks
   - Expected: Correct hash (870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee)
   - Verify: All chunks processed correctly

3. **Partial Last Chunk**: 409600 bytes (7 chunks)
   - Expected: Correct hash
   - Verify: Last chunk (16KB) processed correctly

---

## 📋 Implementation Checklist

- [ ] Add state continuation tracking to data structure
- [ ] Implement `process_chunk_with_state_continuation()` function
- [ ] Modify update handler to process chunks immediately
- [ ] Modify finalization handler to output saved state
- [ ] Add logging for debugging
- [ ] Test with single chunk
- [ ] Test with multiple chunks
- [ ] Verify hash matches expected value
- [ ] Update documentation

---

## 🎯 Expected Results

### Before Fix

```
Hash: a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
❌ VERIFICATION FAILED
```

### After Fix

```
Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
✅ VERIFICATION PASSED
```

---

## 📝 Detailed Code Changes

### Change 1: Add State Continuation Tracking

**File**: `drivers/crypto/crypto_em32_sha.c`
**Location**: Lines 100-115 (struct crypto_em32_data)

**Add after line 105**:
```c
    /* Chunk processing tracking */
    uint64_t chunks_processed;        /* Number of chunks processed */
    uint64_t total_bytes_to_process;  /* Total bytes in all chunks */
    bool is_final_chunk;              /* True if this is the last chunk */
```

### Change 2: Implement State Continuation Function

**File**: `drivers/crypto/crypto_em32_sha.c`
**Location**: After `sha_restore_state()` function (around line 253)

**Add new function**:
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

### Change 3: Modify Update Handler

**File**: `drivers/crypto/crypto_em32_sha.c`
**Location**: Lines 387-407

**Replace**:
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

**With**:
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

**Status**: ✅ **IMPLEMENTATION PLAN COMPLETE**

**Next Action**: Implement state continuation in driver

