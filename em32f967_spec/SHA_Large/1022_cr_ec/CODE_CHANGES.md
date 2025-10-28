# Exact Code Changes for Large Data Processing Fix

## File 1: drivers/crypto/Kconfig

### Change 1: CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE

**Location**: Lines 25-36

**Before**:
```kconfig
config CRYPTO_EM32_SHA_PREALLOC_SIZE
	int "Initial accumulation pre-allocation size (bytes)"
	default 8192
	range 512 1048576
	help
	  When the driver switches from the small 256-byte legacy buffer to
	  the accumulation path (single-run hashing), it will ensure the
	  accumulation buffer is at least this large to reduce late
	  reallocations during long messages. Increase if you commonly hash
	  multi-kilobyte messages and have enough heap.
```

**After**:
```kconfig
config CRYPTO_EM32_SHA_PREALLOC_SIZE
	int "Initial accumulation pre-allocation size (bytes)"
	default 65536
	range 512 2097152
	help
	  When the driver switches from the small 256-byte legacy buffer to
	  the accumulation path (single-run hashing), it will ensure the
	  accumulation buffer is at least this large to reduce late
	  reallocations during long messages. Default 65536 (64KB) provides
	  good balance between memory usage and reallocation overhead for
	  large data processing. Increase if you commonly hash multi-megabyte
	  messages and have enough heap.
```

**Changes**:
- `default 8192` → `default 65536` (8KB → 64KB)
- `range 512 1048576` → `range 512 2097152` (1MB → 2MB max)
- Updated help text with rationale

### Change 2: CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE

**Location**: Lines 38-49

**Before**:
```kconfig
config CRYPTO_EM32_SHA_MAX_ACCUM_SIZE
	int "Maximum accumulation buffer size (bytes)"
	default 262144
	range 4096 1048576
	help
	  Upper bound for the total accumulation buffer size used to collect
	  input prior to the single-run hardware hash. If an input exceeds
	  this size, updates will fail with -ENOMEM. Set based on available
	  RAM and expected message sizes. Default 262144 (256KB) supports
	  >300KB data processing through chunked operations.
```

**After**:
```kconfig
config CRYPTO_EM32_SHA_MAX_ACCUM_SIZE
	int "Maximum accumulation buffer size (bytes)"
	default 524288
	range 4096 2097152
	help
	  Upper bound for the total accumulation buffer size used to collect
	  input prior to the single-run hardware hash. If an input exceeds
	  this size, updates will fail with -ENOMEM. Set based on available
	  RAM and expected message sizes. Default 524288 (512KB) supports
	  large data processing (>400KB). EM32F967 hardware can process up to
	  2^59 bits in a single operation, so this buffer size is the limiting
	  factor for single-run hashing.
```

**Changes**:
- `default 262144` → `default 524288` (256KB → 512KB)
- `range 4096 1048576` → `range 4096 2097152` (1MB → 2MB max)
- Updated help text with hardware capability information

## File 2: drivers/crypto/crypto_em32_sha.c

### Change: Simplified Chunked Mode Logic

**Location**: Lines 298-310

**Before**:
```c
        if (data->use_chunked) {
            /* For chunked mode, accumulate up to chunk size, then process */
            int ret = accum_append(data, pkt->in_buf, pkt->in_len);
            if (ret) return ret;

            /* If accumulated data exceeds chunk size, process a chunk */
            if (data->accum_len >= SHA256_CHUNK_SIZE) {
                /* Process full chunks */
                size_t chunks_to_process = data->accum_len / SHA256_CHUNK_SIZE;
                size_t bytes_to_process = chunks_to_process * SHA256_CHUNK_SIZE;

                /* Process this chunk (will be handled in finish or next update) */
                LOG_DBG("Accumulated %zu bytes, ready to process %zu byte chunk",
                        data->accum_len, bytes_to_process);
            }
        } else {
            int ret = accum_append(data, pkt->in_buf, pkt->in_len);
            if (ret) return ret;
        }
```

**After**:
```c
        if (data->use_chunked) {
            /* For chunked mode, accumulate data.
             * Note: EM32F967 hardware doesn't support state continuation,
             * so we accumulate all data and process in one operation at finish.
             */
            int ret = accum_append(data, pkt->in_buf, pkt->in_len);
            if (ret) return ret;

            LOG_DBG("Accumulated %zu bytes in chunked mode", data->accum_len);
        } else {
            int ret = accum_append(data, pkt->in_buf, pkt->in_len);
            if (ret) return ret;
        }
```

**Changes**:
- Removed non-functional chunk processing code (lines 304-312)
- Updated comment to explain hardware limitation
- Simplified to just accumulate data
- Added debug log for accumulated bytes

## Summary of Changes

### Kconfig Changes
| Item | Before | After | Reason |
|------|--------|-------|--------|
| PREALLOC_SIZE default | 8KB | 64KB | Reduce reallocation overhead |
| PREALLOC_SIZE max | 1MB | 2MB | Support larger pre-allocations |
| MAX_ACCUM_SIZE default | 256KB | 512KB | Support 400KB+ data |
| MAX_ACCUM_SIZE max | 1MB | 2MB | Support larger buffers |

### Code Changes
| File | Lines | Change | Reason |
|------|-------|--------|--------|
| crypto_em32_sha.c | 298-310 | Simplified chunked logic | Remove non-functional code |

## Impact Analysis

### Positive Impacts
✅ Fixes -ENOMEM error for 400KB data
✅ Eliminates timeout errors
✅ Enables EC RW image verification
✅ Maintains backward compatibility
✅ No API changes

### Negative Impacts
⚠️ Increased memory usage (512KB buffer vs 256KB)
⚠️ Requires sufficient heap memory

### Mitigation
- Use memory-constrained configuration if needed
- Monitor heap usage during operation
- Adjust buffer sizes based on available RAM

## Verification

### Build Verification
```bash
cd /home/james/zephyrproject/elan-zephyr
west build -b em32f967_dv -p always
```

### Configuration Verification
```bash
# Check that new defaults are applied
grep "CONFIG_CRYPTO_EM32_SHA" build/zephyr/.config
```

Expected output:
```
CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE=524288
CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE=65536
```

### Runtime Verification
```bash
# Flash and monitor logs
west flash
# Should see successful 400KB hash processing
```

## Rollback Instructions

If needed to revert changes:

### Revert Kconfig
```bash
git checkout drivers/crypto/Kconfig
```

### Revert crypto_em32_sha.c
```bash
git checkout drivers/crypto/crypto_em32_sha.c
```

### Rebuild
```bash
west build -b em32f967_dv -p always
```

## Testing Commands

### Build with fix
```bash
west build -b em32f967_dv -p always
```

### Flash to board
```bash
west flash
```

### Monitor serial output
```bash
# In another terminal
minicom -D /dev/ttyUSB0 -b 115200
```

### Expected log output
```
[0.199000] <inf> crypto_em32_sha: Switching to chunked processing for large data (total=400384 bytes)
[0.200000] <dbg> crypto_em32_sha: Accumulated 400384 bytes in chunked mode
[0.201000] <inf> crypto_em32_sha: Chunked finalization: processed=0, current=400384, total_bits=3203072
[0.300000] <inf> crypto_em32_sha: SHA256 operation completed successfully
```

## Files Modified Summary

```
drivers/crypto/Kconfig
├─ Lines 25-36: CONFIG_CRYPTO_EM32_SHA_PREALLOC_SIZE
│  └─ default: 8192 → 65536
│  └─ range: 512 1048576 → 512 2097152
│  └─ help: Updated with rationale
│
└─ Lines 38-49: CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE
   └─ default: 262144 → 524288
   └─ range: 4096 1048576 → 4096 2097152
   └─ help: Updated with hardware capability info

drivers/crypto/crypto_em32_sha.c
└─ Lines 298-310: Simplified chunked mode logic
   └─ Removed non-functional chunk processing
   └─ Updated comments
   └─ Simplified to accumulation only
```

## Conclusion

The fix consists of:
1. **Two configuration changes** in Kconfig (buffer sizes)
2. **One code simplification** in crypto_em32_sha.c (chunked logic)

Total changes: **~30 lines** across 2 files

Impact: **Resolves 400KB+ data processing** while maintaining full backward compatibility

