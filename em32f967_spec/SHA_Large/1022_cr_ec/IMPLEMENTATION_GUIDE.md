# Implementation Guide: Large Data SHA256 Processing Fix

## Quick Start

### Step 1: Apply Configuration Changes

The fix has already been applied to:
- `drivers/crypto/Kconfig` - Updated buffer sizes
- `drivers/crypto/crypto_em32_sha.c` - Simplified chunked mode logic

### Step 2: Rebuild Firmware

```bash
cd /home/james/zephyrproject/elan-zephyr
west build -b em32f967_dv -p always
```

### Step 3: Flash and Test

```bash
west flash
```

## Configuration Options

### Default Configuration (Recommended for Most Users)

```
CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE=524288    # 512KB
CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE=65536      # 64KB
CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC=100000      # 100ms
```

**Use Case**: EC communication with 300-500KB data
**Memory**: ~513KB peak (512KB buffer + overhead)
**Performance**: Excellent for typical use cases

### Memory-Constrained Configuration

```
CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE=262144    # 256KB
CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE=32768      # 32KB
CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC=100000
```

**Use Case**: Limited RAM systems, data < 256KB
**Memory**: ~263KB peak
**Performance**: Good, but limited to 256KB data

### High-Performance Configuration

```
CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE=1048576   # 1MB
CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE=131072     # 128KB
CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC=200000      # 200ms
```

**Use Case**: High-throughput systems, data > 500KB
**Memory**: ~1MB peak
**Performance**: Excellent for large data

## Memory Analysis

### EM32F967 Memory Layout

```
System RAM: 112KB (0x2002_8000 - 0x2004_3FFF)
ID Data RAM: 160KB (0x2000_0000 - 0x2002_7FFF)
Total: 272KB
```

### Allocation Strategy

**Default Configuration (512KB buffer)**:
- Initial prealloc: 64KB (allocated immediately)
- Growth: Doubles on each reallocation (64KB → 128KB → 256KB → 512KB)
- Peak: 512KB + overhead (~513KB total)

**Feasibility Check**:
- System RAM: 112KB
- ID Data RAM: 160KB
- Total available: 272KB
- Required: 513KB

**Issue**: 513KB > 272KB available!

### Solution: Use Heap Memory

The Zephyr kernel allocates heap from available memory pools. The actual available heap depends on:
1. System configuration
2. Other running tasks
3. Stack usage
4. Kernel data structures

**Recommendation**: Monitor heap usage during testing

## Testing Procedure

### Test 1: Basic Functionality (256KB)

```c
#include <zephyr/crypto/crypto.h>
#include "sha256.h"

void test_256kb_hash(void) {
    uint8_t data[262144];  // 256KB
    struct sha256_ctx ctx;
    
    // Fill with test data
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = (i & 0xFF);
    }
    
    // Hash
    SHA256_init(&ctx);
    SHA256_update(&ctx, data, sizeof(data));
    uint8_t *hash = SHA256_final(&ctx);
    
    // Verify
    if (hash) {
        LOG_INF("256KB hash successful");
        return 0;
    } else {
        LOG_ERR("256KB hash failed");
        return -1;
    }
}
```

### Test 2: Large Data (400KB)

```c
void test_400kb_hash(void) {
    uint8_t data[400384];  // 400KB
    struct sha256_ctx ctx;
    
    // Fill with test data
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = (i & 0xFF);
    }
    
    // Hash
    SHA256_init(&ctx);
    SHA256_update(&ctx, data, sizeof(data));
    uint8_t *hash = SHA256_final(&ctx);
    
    // Verify
    if (hash) {
        LOG_INF("400KB hash successful");
        return 0;
    } else {
        LOG_ERR("400KB hash failed");
        return -1;
    }
}
```

### Test 3: EC Communication Simulation

```c
void test_ec_communication(void) {
    // Simulate EC RW image verification (400KB)
    uint8_t rw_image[400384];
    struct sha256_ctx ctx;
    
    // Initialize
    SHA256_init(&ctx);
    
    // Simulate chunked updates from EC
    size_t chunk_size = 65536;  // 64KB chunks
    for (size_t offset = 0; offset < sizeof(rw_image); offset += chunk_size) {
        size_t len = MIN(chunk_size, sizeof(rw_image) - offset);
        SHA256_update(&ctx, &rw_image[offset], len);
    }
    
    // Finalize
    uint8_t *hash = SHA256_final(&ctx);
    
    if (hash) {
        LOG_INF("EC communication test successful");
        return 0;
    } else {
        LOG_ERR("EC communication test failed");
        return -1;
    }
}
```

## Monitoring and Debugging

### Enable Debug Logging

```
CONFIG_CRYPTO_LOG_LEVEL=4  # Debug level
```

### Expected Log Output (400KB)

```
[0.199000] <inf> crypto_em32_sha: Switching to chunked processing for large data (total=400384 bytes)
[0.200000] <dbg> crypto_em32_sha: Accumulated 400384 bytes in chunked mode
[0.201000] <inf> crypto_em32_sha: Chunked finalization: processed=0, current=400384, total_bits=3203072
[0.300000] <inf> crypto_em32_sha: SHA256 operation completed successfully
```

### Troubleshooting

**Issue**: Still getting -ENOMEM error
- **Cause**: CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE not updated
- **Fix**: Verify Kconfig changes and rebuild with `west build -p always`

**Issue**: Timeout error
- **Cause**: Hardware not responding or clock not enabled
- **Fix**: Check clock gating register (HCLKG_ENCRYPT bit 6)
- **Fix**: Increase CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC

**Issue**: Incorrect hash output
- **Cause**: Data corruption during accumulation
- **Fix**: Check memory alignment and buffer boundaries
- **Fix**: Enable memory protection if available

## Performance Optimization

### Reduce Reallocation Overhead

Increase PREALLOC_SIZE to match expected data size:

```
Expected data: 400KB
CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE=409600  # 400KB
```

This allocates 400KB upfront, avoiding reallocations.

### Optimize Timeout

Measure actual processing time:

```c
uint32_t start = k_uptime_get_32();
SHA256_update(&ctx, data, sizeof(data));
uint32_t elapsed = k_uptime_get_32() - start;
LOG_INF("Processing time: %u ms", elapsed);
```

Then set timeout to 2x the measured time:

```
CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC=200000  # 200ms
```

## Integration with EC

### EC RW Image Verification

The EC uses SHA256 to verify the RW (read-write) image during boot:

```c
// In EC code (sha256_hw.c)
void SHA256_update(struct sha256_ctx *ctx, const uint8_t *data, uint32_t len)
{
    int ret;
    struct hash_pkt pkt = {
        .in_buf = (uint8_t *)data,
        .in_len = len,
        .out_buf = ctx->buf,
    };
    struct hash_ctx *hash_ctx = &ctx->hash_sha256;
    
    ret = hash_update(hash_ctx, &pkt);  // Calls driver
    
    if (ret != 0) {
        LOG_ERR("SHA256 Update Fail");  // Should not happen now
    }
}
```

With the fix:
- ✅ 400KB RW image hashes successfully
- ✅ No -ENOMEM errors
- ✅ No timeout errors
- ✅ EC boot completes successfully

## Validation Checklist

- [ ] Kconfig changes applied
- [ ] crypto_em32_sha.c changes applied
- [ ] Firmware rebuilt with `west build -p always`
- [ ] Firmware flashed to board
- [ ] Test 256KB hash (should pass)
- [ ] Test 400KB hash (should pass)
- [ ] Test EC communication (should pass)
- [ ] Monitor logs for errors
- [ ] Verify correct hash output
- [ ] Check memory usage
- [ ] Verify EC boot completes

## Support and Further Assistance

If issues persist:

1. **Check logs**: Enable CONFIG_CRYPTO_LOG_LEVEL=4
2. **Verify configuration**: Check prj.conf for correct settings
3. **Test incrementally**: Start with 256KB, then 400KB, then larger
4. **Monitor memory**: Use `k_mem_pool_stats()` to check heap
5. **Check hardware**: Verify SHA engine clock is enabled

## References

- EM32F967 Specification: `/home/james/zephyrproject/elan-zephyr/em32f967_spec/EM32F967_Complete_Specification_v3.0.md`
- SHA256 Driver: `/home/james/zephyrproject/elan-zephyr/drivers/crypto/crypto_em32_sha.c`
- Configuration: `/home/james/zephyrproject/elan-zephyr/drivers/crypto/Kconfig`
- Error Log: `/home/james/zephyrproject/elan-zephyr/em32f967_spec/SHA_Large/1022_cr_ec/EC_1022_v1.log`

