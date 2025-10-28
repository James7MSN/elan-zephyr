# SHA256 Test Pattern Integration - EM32F967_DV Board

**Date**: October 25, 2025  
**Status**: ✅ VERIFICATION PATTERN INTEGRATED  
**Version**: 1.0

---

## Executive Summary

Successfully integrated SHA256 verification pattern into the test program. The test program now includes the expected SHA256 hash and automatically verifies the board's output against the expected pattern.

---

## Expected SHA256 Hash

### Test Pattern Specification

| Parameter | Value |
|-----------|-------|
| Data Size | 409,600 bytes (400KB) |
| Chunk Size | 65,536 bytes (64KB) |
| Number of Chunks | 7 |
| Pattern | `byte[i] = (offset + i) & 0xFF` |

### Expected Hash Value

```
870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
```

**Verification Status**: ✅ Verified with Python script

---

## Integration Details

### File Modified

**File**: `samples/elan_sha/src/main_large_data_ec_sim.c`

### Changes Made

#### 1. Added Verification Pattern Definition

```c
/* ========================================
 * SHA256 VERIFICATION PATTERN
 * ========================================
 * Test Pattern: 400KB (409600 bytes) of deterministic data
 * Pattern: byte[i] = (offset + i) & 0xFF
 * 
 * Expected SHA256 Hash (verified with Python):
 * 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
 * 
 * Verification Method:
 * 1. Run Python script: ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py
 * 2. Compare board output with expected hash
 * 3. If hashes match: SHA256 implementation is correct
 * ======================================== */
#define EXPECTED_SHA256_HASH "870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee"
```

#### 2. Enhanced print_hash() Function

**Before**:
```c
static void print_hash(const uint8_t *hash, size_t len)
{
    char hash_str[65];
    char *ptr = hash_str;

    for (size_t i = 0; i < len; i++) {
        ptr += snprintf(ptr, 3, "%02x", hash[i]);
    }

    LOG_INF("Hash: %s", hash_str);
}
```

**After**:
```c
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
```

---

## Expected Test Output

### When Hash Matches (Correct Implementation)

```
[00:00:00.058,000] <inf> sha_large_data_test: Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
[00:00:00.058,000] <inf> sha_large_data_test: ✅ VERIFICATION PASSED - Hash matches expected pattern!
[00:00:00.058,000] <inf> sha_large_data_test: Test 1 PASSED
```

### When Hash Does NOT Match (Implementation Issue)

```
[00:00:00.058,000] <inf> sha_large_data_test: Hash: a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
[00:00:00.058,000] <wrn> sha_large_data_test: ⚠️  VERIFICATION FAILED - Hash does NOT match expected pattern
[00:00:00.058,000] <wrn> sha_large_data_test: Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
[00:00:00.058,000] <wrn> sha_large_data_test: Got:      a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
[00:00:00.058,000] <inf> sha_large_data_test: Test 1 FAILED
```

---

## Verification Process

### Step 1: Verify Expected Hash with Python Script

```bash
cd /home/james/zephyrproject/elan-zephyr
python3 ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py
```

**Output**:
```
Expected SHA256 for EM32F967_DV board:
  870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
```

### Step 2: Flash Firmware to Board

```bash
west flash
```

### Step 3: Run Tests and Monitor Console

```bash
west monitor
```

### Step 4: Check Verification Results

Look for one of these messages:
- ✅ `VERIFICATION PASSED - Hash matches expected pattern!`
- ⚠️ `VERIFICATION FAILED - Hash does NOT match expected pattern`

---

## Test Data Pattern

### Data Generation Algorithm

```c
static void generate_test_data(uint8_t *buf, size_t len, size_t offset)
{
    for (size_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)((offset + i) & 0xFF);
    }
}
```

### Pattern Characteristics

- **Deterministic**: Same data generated every time
- **Repeating**: Pattern repeats every 256 bytes (0x00-0xFF)
- **Verifiable**: Easy to regenerate and verify
- **Uniform Distribution**: Each byte value appears 1,600 times

### Example Data Pattern

```
Offset 0:     0x00, 0x01, 0x02, ..., 0xFE, 0xFF
Offset 256:   0x00, 0x01, 0x02, ..., 0xFE, 0xFF
Offset 512:   0x00, 0x01, 0x02, ..., 0xFE, 0xFF
...
Offset 409344: 0x00, 0x01, 0x02, ..., 0xFE, 0xFF
Offset 409600: (end of data)
```

---

## Verification Tools

### Python Verification Script

**File**: `ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py`

**Features**:
- Generates 400KB test data
- Computes SHA256 (full data)
- Computes SHA256 (chunked data)
- Verifies consistency
- Generates C code for verification

**Usage**:
```bash
python3 ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py
```

### Test Program

**File**: `samples/elan_sha/src/main_large_data_ec_sim.c`

**Features**:
- Generates same test data pattern
- Computes SHA256 using EM32F967 hardware
- Automatically verifies against expected hash
- Provides clear pass/fail indication

---

## Build Information

### Compilation Status

```
Memory region         Used Size  Region Size  %age Used
           FLASH:       45068 B       536 KB      8.21%
             RAM:      127328 B       160 KB     77.71%
        IDT_LIST:          0 GB        32 KB      0.00%
```

**Status**: ✅ BUILD SUCCESSFUL

---

## Troubleshooting

### Issue: Hash Mismatch

**Symptom**: Board produces different hash than expected

**Possible Causes**:
1. Data generation pattern incorrect
2. SHA256 algorithm implementation issue
3. Memory corruption during processing
4. Incorrect chunked processing

**Solution**:
1. Verify data generation pattern matches specification
2. Check SHA256 hardware implementation
3. Review memory allocation and buffer management
4. Verify chunked processing logic

### Issue: Verification Not Showing

**Symptom**: No verification message in console output

**Possible Causes**:
1. Test program not updated
2. Firmware not flashed
3. Console output not captured

**Solution**:
1. Rebuild firmware: `west build -p always`
2. Flash to board: `west flash`
3. Monitor console: `west monitor`

---

## Summary

✅ **Verification Pattern Integrated**
- Expected hash: `870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee`
- Test program updated with automatic verification
- Python verification script provided
- Build successful

✅ **Ready for Testing**
- Flash firmware to board
- Run tests
- Check verification results
- Compare with expected hash

---

**Status**: ✅ INTEGRATION COMPLETE  
**Expected Hash**: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee  
**Verification**: Automatic (built into test program)  
**Ready for Testing**: ✅ YES

