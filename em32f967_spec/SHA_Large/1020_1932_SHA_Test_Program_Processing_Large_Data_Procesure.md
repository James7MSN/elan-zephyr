# SHA Test Program Processing Large Data Procedure

**Document**: EM32F967 SHA256 Large Data Processing Analysis  
**Date**: October 20, 2024  
**Time**: 19:32  
**Function**: `test_sha256_large_consistency(void)` Analysis

## Executive Summary

The `test_sha256_large_consistency()` function in `./samples/elan_sha/src/main.c` tests SHA256 processing with large data sizes (300 bytes and 4097 bytes) using two different approaches: **one-shot processing** and **chunked processing**. The analysis reveals that the EM32F967 SHA256 hardware driver uses a **single-run accumulation strategy** where all data is collected before being sent to the hardware engine in one operation.

## Test Function Overview

### Test Data Sizes
```c
const size_t sizes[] = { 300, 4097 };
```

The function tests two specific sizes:
- **300 bytes**: Tests data larger than the legacy 256-byte buffer
- **4097 bytes**: Tests data larger than typical 4KB boundaries

### Test Data Pattern
```c
/* Fill deterministic pattern */
for (size_t i = 0; i < len; ++i) {
    buf[i] = (uint8_t)(i & 0xFF);
}
```
Each byte is filled with `i % 256`, creating a repeating pattern: `0x00, 0x01, 0x02, ..., 0xFF, 0x00, 0x01, ...`

## Data Processing Flow Analysis

### 1. One-Shot Processing Mode

**How it works:**
```c
/* One-shot compute */
pkt.in_buf = buf;           // Entire buffer (300 or 4097 bytes)
pkt.in_len = len;           // Complete length
hash_update(&ctx, &pkt);    // Send all data at once
ctx.hash_hndlr(&ctx, &pkt, true);  // Finalize
```

**Data flow to driver:**
- **Single call** to `hash_update()` with complete data
- **Bytes sent**: 300 bytes (for Test 1) or 4097 bytes (for Test 2)
- **Number of calls**: 1 call to driver per test size

### 2. Chunked Processing Mode

**How it works:**
```c
size_t off = 0;
size_t chunk = 73; /* non power-of-two to hit all boundaries */
while (off < len) {
    size_t remaining = len - off;
    size_t this_len = (chunk < remaining) ? chunk : remaining;
    pkt.in_buf = buf + off;
    pkt.in_len = this_len;
    hash_update(&ctx, &pkt);        // Accumulate chunk
    off += this_len;
    chunk = (chunk == 73) ? 257 : 73; /* alternate sizes */
}
ctx.hash_hndlr(&ctx, &pkt, true);   // Finalize
```

**Chunking pattern:**
- **Alternating chunk sizes**: 73 bytes, then 257 bytes, then 73 bytes, etc.
- **Why 73 and 257**: Non-power-of-two sizes to test boundary conditions

**For 300 bytes:**
1. Chunk 1: 73 bytes (offset 0-72)
2. Chunk 2: 257 bytes (offset 73-329) → **Limited to remaining 227 bytes**
3. Total chunks: 2 calls to `hash_update()`

**For 4097 bytes:**
1. Chunk 1: 73 bytes (offset 0-72)
2. Chunk 2: 257 bytes (offset 73-329)
3. Chunk 3: 73 bytes (offset 330-402)
4. Chunk 4: 257 bytes (offset 403-659)
5. ... continues alternating ...
6. Final chunk: Remaining bytes
7. **Total chunks**: Approximately **21 calls** to `hash_update()`

## Driver Internal Processing

### Buffer Management Strategy

The `crypto_em32_sha.c` driver uses a **dual-buffer strategy**:

1. **Legacy Buffer** (256 bytes max):
   ```c
   uint8_t buffer[256];              /* Backwards compatibility */
   ```

2. **Accumulation Buffer** (dynamic, up to 64KB):
   ```c
   uint8_t *accum_buf;               /* Dynamically grown buffer */
   size_t accum_len;
   size_t accum_cap;
   ```

### Driver Processing Logic

**For data ≤ 256 bytes:**
- Uses legacy `buffer[256]`
- Data accumulated in driver's internal buffer

**For data > 256 bytes:**
- Automatically switches to dynamic `accum_buf`
- Initial allocation: 8KB (`CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE = 8192`)
- Maximum size: 64KB (`CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE = 65536`)

### Hardware Data Transfer

When `hash_hndlr()` is called with `finish=true`, the driver:

1. **Calculates total words to send:**
   ```c
   uint32_t words_to_write = (uint32_t)((total_bytes + 3U) / 4U);
   ```

2. **Sends data to hardware in 32-bit words:**
   ```c
   while (words_written < words_to_write) {
       uint32_t w = 0;
       for (int j = 0; j < 4; j++) {
           if (bytes_written < total_bytes) {
               w |= ((uint32_t)src[bytes_written]) << (j * 8);
               bytes_written++;
           }
       }
       sys_write32(w, config->base + SHA_IN_OFFSET);  // Write 4 bytes
       words_written++;
       
       // Hardware synchronization every 16 words (64 bytes)
       if ((words_written % 16U) == 0U) {
           for (int j = 0; j < 6; j++) {
               __asm__ volatile ("nop");
           }
           while (!(sys_read32(config->base + SHA_CTR_OFFSET) & SHA_READY_BIT)) {}
       }
   }
   ```

## Detailed Processing Breakdown

### For 300-byte Test:

**One-shot mode:**
- Driver calls to hardware: **1 session**
- Bytes per hardware write: **4 bytes** (32-bit words)
- Total hardware writes: **75 words** (300 ÷ 4 = 75)
- Synchronization points: **4 times** (every 16 words: at words 16, 32, 48, 64)

**Chunked mode:**
- Application calls to driver: **2 calls** (73 + 227 bytes)
- Driver accumulates all chunks, then processes exactly like one-shot mode
- Hardware behavior: **Identical to one-shot** (75 words, 4 sync points)

### For 4097-byte Test:

**One-shot mode:**
- Driver calls to hardware: **1 session**
- Bytes per hardware write: **4 bytes** (32-bit words)
- Total hardware writes: **1025 words** (4097 ÷ 4 = 1024.25, rounded up)
- Synchronization points: **64 times** (every 16 words)
- Last word padding: **3 bytes** of padding (4097 % 4 = 1, so 3 bytes needed)

**Chunked mode:**
- Application calls to driver: **~21 calls** (alternating 73/257 byte chunks)
- Driver accumulates all chunks, then processes exactly like one-shot mode
- Hardware behavior: **Identical to one-shot** (1025 words, 64 sync points)

## Key Findings

### 1. **Driver Architecture**
- Uses **accumulation strategy**, not block-by-block processing
- All input data is collected before sending to hardware
- Hardware processes complete message in **single operation**

### 2. **Hardware Interface**
- **Word-based transfer**: Always sends 32-bit words (4 bytes)
- **Synchronization**: Every 16 words (64 bytes) requires wait for hardware ready
- **No streaming**: Hardware expects complete message with proper padding

### 3. **Memory Usage**
- **Small data (≤256B)**: Uses 256-byte stack buffer
- **Large data (>256B)**: Uses dynamic heap allocation (8KB initial, 64KB max)
- **Memory efficiency**: Single allocation for entire message

### 4. **Performance Characteristics**
- **Chunked vs One-shot**: Identical hardware performance
- **Overhead**: Chunked mode has accumulation overhead but same hardware timing
- **Bottleneck**: Hardware synchronization every 64 bytes, not software chunking

## Conclusion

The `test_sha256_large_consistency()` function demonstrates that the EM32F967 SHA256 driver uses a **single-run processing model**. Regardless of how the application sends data (one-shot or chunked), the driver accumulates all input and processes it as a single hardware operation. The hardware interface operates on 4-byte words with synchronization every 64 bytes, making it unsuitable for true streaming operation but highly efficient for accumulated data processing.

The test confirms that both processing modes produce identical results, validating the driver's accumulation strategy for handling various input patterns while maintaining consistency and correctness.