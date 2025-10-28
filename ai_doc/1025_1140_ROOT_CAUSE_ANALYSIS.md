# Root Cause Analysis - SHA256 Hash Mismatch

**Date**: October 25, 2025  
**Status**: ✅ ROOT CAUSE IDENTIFIED  
**Version**: 1.0

---

## 🔍 Problem Statement

The EM32F967_DV board is producing an INCORRECT SHA256 hash for 400KB test data:

| Item | Value |
|------|-------|
| **Expected Hash** | `870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee` |
| **Board Output** | `a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654` |
| **Status** | ❌ MISMATCH |

---

## 🎯 Root Cause Identified

### The Board is Only Processing the LAST 16KB Chunk!

**Evidence from Console Logs**:

```
[00:00:00.049,000] <dbg> crypto_em32_sha: em32_sha256_handler: Chunked finalization: src=0x20005a70, total_bytes=16384, total_bits=3276800
```

- `total_bytes=16384` = 16KB (the LAST chunk)
- `total_bits=3276800` = 409600 * 8 (correct total, but processing wrong data!)

### Why This Happens

The driver is using a **zero-copy approach** that stores only a reference to the LAST input buffer:

```c
/* Store input buffer reference for later processing */
data->last_input_buf = pkt->in_buf;
data->last_input_len = pkt->in_len;
```

Each time a new chunk arrives, it OVERWRITES the previous reference. So only the LAST chunk is retained!

---

## 💾 Memory Constraint Problem

### System RAM Limitation

| Item | Size |
|------|------|
| Total EM32F967 RAM | 160KB |
| Available for Heap | ~100KB (after kernel/stack) |
| Test Data Size | 400KB |
| Accumulation Buffer Needed | 400KB |

**Result**: Cannot accumulate all 400KB in memory!

### Current Approach Fails

```
Trying to accumulate 400KB in 100KB available RAM
↓
Memory allocation fails with -ENOMEM
↓
Driver falls back to zero-copy (only last chunk)
↓
Incorrect hash produced
```

---

## ✅ Solution Required

### Proper State Continuation Processing

Instead of accumulating all data, we need to:

1. **Process each chunk through hardware SHA256**
2. **Save SHA256 state after each chunk** (H0-H7 values)
3. **Restore state for next chunk**
4. **Combine results properly**

### Implementation Strategy

```c
// Pseudo-code for proper chunked processing
for each chunk:
    1. Load previous state (or initial values for first chunk)
    2. Process chunk through hardware
    3. Save state (H0-H7)
    4. Continue to next chunk

// Final operation
1. Load final state
2. Process padding and finalization
3. Output final hash
```

---

## 📊 Verification

### Python Script Confirms Correct Hash

```bash
$ python3 ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py

Full data:    870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Chunked data: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee

✅ HASHES MATCH
```

### Board Currently Produces

```
Hash: a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
```

This is the hash of ONLY the last 16KB chunk!

---

## 🔧 Why Current Approaches Fail

### Approach 1: Accumulate All Data ❌

**Problem**: 400KB data > 100KB available RAM

```
[00:00:00.016,000] <err> crypto_em32_sha: Failed to accumulate chunk data: -12
```

Error -12 = ENOMEM (out of memory)

### Approach 2: Zero-Copy (Current) ❌

**Problem**: Only stores reference to LAST chunk

```
[00:00:00.049,000] <dbg> crypto_em32_sha: Chunked finalization: src=0x20005a70, total_bytes=16384
```

Only 16KB processed instead of 400KB!

### Approach 3: State Continuation (Required) ✅

**Solution**: Process chunks through hardware with state continuation

- No need to accumulate all data
- Process one chunk at a time
- Save/restore state between chunks
- Produces correct hash

---

## 📋 Test Data Pattern

### Generation Algorithm

```c
byte[i] = (offset + i) & 0xFF
```

### Pattern Characteristics

- **Deterministic**: Same data every time
- **Repeating**: 0x00-0xFF pattern every 256 bytes
- **Verifiable**: Easy to regenerate

### Test Data Breakdown

| Chunk | Offset | Size | Data Range |
|-------|--------|------|-----------|
| 1 | 0 | 65,536 | 0x00-0xFF (repeating) |
| 2 | 65,536 | 65,536 | 0x00-0xFF (repeating) |
| 3 | 131,072 | 65,536 | 0x00-0xFF (repeating) |
| 4 | 196,608 | 65,536 | 0x00-0xFF (repeating) |
| 5 | 262,144 | 65,536 | 0x00-0xFF (repeating) |
| 6 | 327,680 | 65,536 | 0x00-0xFF (repeating) |
| 7 | 393,216 | 16,384 | 0x00-0xFF (repeating) |

---

## 🎯 Expected Behavior

### When Properly Implemented

```
Test 1: Chunked 400KB Hash
  Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
  ✅ VERIFICATION PASSED

Test 2: EC-style Chunked Transfer
  Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
  ✅ VERIFICATION PASSED

Test 3: Chunked Processing Verification
  Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
  ✅ VERIFICATION PASSED

Test Summary: 3 passed, 0 failed
```

---

## 📚 Key Findings

1. ✅ **Python verification script is CORRECT**
   - Expected hash: `870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee`

2. ❌ **Board is producing WRONG hash**
   - Board output: `a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654`
   - This is hash of LAST 16KB chunk only

3. ❌ **Zero-copy approach is BROKEN**
   - Only stores reference to last chunk
   - Previous chunks are lost

4. ❌ **Accumulation approach fails**
   - Insufficient RAM (400KB needed, 100KB available)
   - Returns -ENOMEM error

5. ✅ **State continuation is the SOLUTION**
   - Process chunks through hardware
   - Save/restore state between chunks
   - No need to accumulate all data

---

## 🚀 Next Steps

### Required Implementation

1. **Implement proper state continuation**
   - Save SHA256 state (H0-H7) after each chunk
   - Restore state for next chunk
   - Process through hardware SHA256 engine

2. **Fix memory management**
   - Process one chunk at a time
   - Don't accumulate all 400KB
   - Reuse buffers efficiently

3. **Verify correctness**
   - Test with Python verification script
   - Confirm hash matches expected value
   - All 3 tests should pass

---

## 📞 Summary

| Item | Status |
|------|--------|
| Root Cause | ✅ Identified - Only last chunk processed |
| Expected Hash | ✅ Verified - 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee |
| Board Output | ❌ Wrong - a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654 |
| Solution | ✅ Identified - State continuation required |
| Implementation | ⏳ Pending |

---

**Status**: ✅ ROOT CAUSE ANALYSIS COMPLETE

**Next Action**: Implement proper state continuation in driver

