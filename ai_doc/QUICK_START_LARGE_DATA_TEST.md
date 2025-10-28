# Quick Start: Large Data Test (>300KB)

## 🚀 Quick Commands

### Build
```bash
cd /home/james/zephyrproject/elan-zephyr
west build -b 32f967_dv samples/elan_sha
```

### Flash
```bash
west flash
```

### Monitor (Serial Console)
```bash
# Using minicom
minicom -D /dev/ttyUSB0 -b 115200

# Or using screen
screen /dev/ttyUSB0 115200

# Or using picocom
picocom -b 115200 /dev/ttyUSB0
```

## 📊 What Gets Tested

| Test | Size | Type | Purpose |
|------|------|------|---------|
| Capability | N/A | Hardware | Verify device capabilities |
| Pattern | <1KB | Known vectors | Validate basic hashing |
| Incremental | <1KB | Multi-chunk | Validate state preservation |
| Large Consistency | 300B, 4KB | Consistency | Validate chunked processing |
| Boundary | 255B-4KB | Edge cases | Validate boundary conditions |
| **Large Data** | **300KB, 512KB, 1MB** | **>300KB** | **NEW: Validate large data** |

## ✅ Expected Results

### All Tests Pass
```
FINAL TEST SUMMARY:
Total test suites: 6
Passed: 6
Failed: 0
<<< ALL SHA256 TESTS PASSED! >>>
```

### Large Data Test Output
```
=== Running Large Data Test (>300KB) ===
This test validates SHA256 processing of data larger than 300KB
***** SHA-256 Large Data Test (>300KB) *****
Testing data sizes: 300KB, 512KB, 1MB

--- Testing 307200 bytes (0.3 MB) ---
  Computing reference hash (single chunk)...
  Reference hash computed
  Computing test hash (64KB chunks)...
    Processed 0 MB / 0.3 MB
    Processed 1 MB / 0.3 MB
  Test hash computed (5 chunks)
Large data test PASSED for 307200 bytes (0.3 MB)

--- Testing 524288 bytes (0.5 MB) ---
  Computing reference hash (single chunk)...
  Reference hash computed
  Computing test hash (64KB chunks)...
    Processed 0 MB / 0.5 MB
    Processed 1 MB / 0.5 MB
  Test hash computed (8 chunks)
Large data test PASSED for 524288 bytes (0.5 MB)

--- Testing 1048576 bytes (1.0 MB) ---
  Computing reference hash (single chunk)...
  Reference hash computed
  Computing test hash (64KB chunks)...
    Processed 0 MB / 1.0 MB
    Processed 1 MB / 1.0 MB
  Test hash computed (16 chunks)
Large data test PASSED for 1048576 bytes (1.0 MB)
Large data test PASSED
```

## 🔍 What to Look For

### ✅ Success Indicators
- All 6 test suites show "PASSED"
- Large data test shows all 3 sizes (300KB, 512KB, 1MB)
- Hash computation messages appear for each size
- Final summary shows "Passed: 6, Failed: 0"
- Message: "<<< ALL SHA256 TESTS PASSED! >>>"

### ❌ Failure Indicators
- Any test shows "FAILED"
- Memory allocation errors
- Hash mismatch messages
- Timeout errors
- Device not ready errors
- Final summary shows "Failed: > 0"

## 📈 Performance Expectations

### Timing
- 300KB: ~30ms
- 512KB: ~50ms
- 1MB: ~100ms

### Total Test Time
- All 6 suites: ~2-3 seconds

## 🛠️ Troubleshooting

### Issue: Build Fails
```bash
# Clean and rebuild
west build -b 32f967_dv samples/elan_sha --pristine=always
```

### Issue: Flash Fails
```bash
# Check device connection
ls -la /dev/ttyUSB*

# Try manual reset before flashing
```

### Issue: Serial Monitor Shows Nothing
```bash
# Check baud rate (should be 115200)
# Check device is powered
# Try different USB port
# Check cable connection
```

### Issue: Tests Fail with Memory Error
```
Alloc failed for X bytes
```
**Solution**: Increase heap in `prj.conf`:
```
CONFIG_HEAP_MEM_POOL_SIZE=1200000
```

### Issue: Tests Fail with Timeout
```
timeout error
```
**Solution**: Increase timeout in `drivers/crypto/Kconfig`:
```
CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC=200000
```

### Issue: Hash Mismatch
```
Large data test FAILED for len=X
```
**Solution**:
1. Verify driver modifications are in place
2. Check hardware is functioning
3. Run smaller tests first
4. Check system clock frequency

## 📝 Test Log Capture

### Save Output to File
```bash
# Using minicom
# Press Ctrl+A, then Z, then L to start logging

# Using screen
# Press Ctrl+A, then H to start logging

# Using picocom
# Press Ctrl+A, Ctrl+L to start logging
```

### Manual Capture
```bash
# Redirect to file
minicom -D /dev/ttyUSB0 -b 115200 -C test_output.log
```

## 🎯 Verification Steps

1. ✅ Build completes successfully
2. ✅ Flash to device
3. ✅ Open serial monitor
4. ✅ See boot messages
5. ✅ See "=== Running Capability Test ===" 
6. ✅ See all 6 test suites run
7. ✅ See "Large data test PASSED" for all 3 sizes
8. ✅ See final summary with "Passed: 6"
9. ✅ See "<<< ALL SHA256 TESTS PASSED! >>>"

## 📊 Test Execution Timeline

```
[00:00:00] Boot
[00:00:00] Capability Test
[00:00:00] Pattern Test (8 patterns)
[00:00:01] Incremental Test
[00:00:01] Large Consistency Test (300B, 4KB)
[00:00:01] Boundary Size Test (6 sizes)
[00:00:02] Large Data Test (300KB, 512KB, 1MB) ← NEW
[00:00:03] Final Summary
```

## 💾 Configuration

### Default Settings (Recommended)
```
CONFIG_HEAP_MEM_POOL_SIZE=1200000
CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE=262144
CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC=100000
```

### For Memory-Constrained Systems
```
CONFIG_HEAP_MEM_POOL_SIZE=800000
CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE=65536
```

## 📚 Documentation

- **LARGE_DATA_TEST_GUIDE.md** - Detailed test guide
- **LARGE_DATA_TEST_IMPLEMENTATION.md** - Implementation details
- **QUICK_START_LARGE_DATA_TEST.md** - This file

## 🎓 Understanding the Test

### What It Tests
- SHA256 can process 300KB+ data
- Single-chunk processing works
- Multi-chunk processing works
- Results are consistent
- No memory leaks
- No timeouts

### How It Works
1. Allocate buffer (300KB, 512KB, or 1MB)
2. Fill with pseudo-random data
3. Hash entire buffer at once (reference)
4. Hash in 64KB chunks (test)
5. Compare results
6. Free buffer

### Why It Matters
- Validates driver enhancement
- Ensures large data support works
- Confirms no data corruption
- Verifies memory management
- Proves chunked processing

## ✨ Success!

If you see:
```
<<< ALL SHA256 TESTS PASSED! >>>
```

**Congratulations!** Your 32f967_dv board successfully processes SHA256 data larger than 300KB! 🎉

---

**Ready to test?** Follow the Quick Commands above!

