# SHA256 Hash Mismatch - Issue Summary

**Date**: October 25, 2025  
**Status**: ✅ ROOT CAUSE IDENTIFIED  
**Version**: 1.0

---

## 🎯 The Problem

The board is producing an **INCORRECT SHA256 hash** for 400KB test data:

```
Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Got:      a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
```

**Why?** The driver is only processing the **LAST 16KB chunk** instead of all 400KB!

---

## 🔍 Root Cause

### The Driver Uses Zero-Copy Approach

```c
/* Store input buffer reference for later processing */
data->last_input_buf = pkt->in_buf;
data->last_input_len = pkt->in_len;
```

**Problem**: Each time a new chunk arrives, it OVERWRITES the previous reference!

### Console Evidence

```
[00:00:00.049,000] <dbg> crypto_em32_sha: Chunked finalization: src=0x20005a70, total_bytes=16384, total_bits=3276800
```

- `total_bytes=16384` = 16KB (LAST chunk only!)
- `total_bits=3276800` = 409600 * 8 (correct total, but wrong data!)

---

## 💾 Why Accumulation Fails

### Memory Constraint

| Item | Size |
|------|------|
| Available RAM | ~100KB |
| Test Data | 400KB |
| Result | ❌ ENOMEM error |

When trying to accumulate all 400KB:
```
[00:00:00.016,000] <err> crypto_em32_sha: Failed to accumulate chunk data: -12
```

Error -12 = ENOMEM (out of memory)

---

## ✅ Verification

### Python Script Confirms Correct Hash

```bash
$ python3 ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py

Full data:    870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Chunked data: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee

✅ HASHES MATCH - Verification successful!
```

**Conclusion**: The expected hash is CORRECT. The board is producing the WRONG hash.

---

## 🔧 The Solution

### Implement Proper State Continuation

Instead of:
- ❌ Accumulating all 400KB (runs out of memory)
- ❌ Storing only last chunk reference (loses previous chunks)

Do this:
- ✅ Process each chunk through hardware SHA256
- ✅ Save SHA256 state (H0-H7) after each chunk
- ✅ Restore state for next chunk
- ✅ Combine results properly

### Pseudo-Code

```c
// Process all chunks with state continuation
for each chunk:
    1. Load previous state (or initial values for first chunk)
    2. Process chunk through hardware SHA256
    3. Save state (H0-H7)
    4. Continue to next chunk

// Final operation
1. Load final state
2. Process padding and finalization
3. Output final hash
```

---

## 📊 Test Data Pattern

### Generation Algorithm

```c
byte[i] = (offset + i) & 0xFF
```

### Breakdown

- **Total Size**: 409,600 bytes (400KB)
- **Chunk Size**: 65,536 bytes (64KB)
- **Number of Chunks**: 7
- **Last Chunk**: 16,384 bytes (16KB)

### Pattern

Each byte cycles through 0x00-0xFF repeatedly:
```
Offset 0:     0x00, 0x01, 0x02, ..., 0xFE, 0xFF
Offset 256:   0x00, 0x01, 0x02, ..., 0xFE, 0xFF
Offset 512:   0x00, 0x01, 0x02, ..., 0xFE, 0xFF
...
```

---

## 📋 Current Status

| Item | Status |
|------|--------|
| Root Cause | ✅ Identified |
| Expected Hash | ✅ Verified |
| Board Output | ❌ Wrong (only last chunk) |
| Python Verification | ✅ Correct |
| Solution | ✅ Identified |
| Implementation | ⏳ Pending |

---

## 🚀 What Needs to Be Done

### 1. Implement State Continuation

Modify `drivers/crypto/crypto_em32_sha.c` to:
- Process each chunk through hardware
- Save state after each chunk
- Restore state for next chunk

### 2. Fix Memory Management

- Don't accumulate all 400KB
- Process one chunk at a time
- Reuse buffers efficiently

### 3. Verify Correctness

- Test with Python verification script
- Confirm hash matches: `870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee`
- All 3 tests should pass

---

## 📚 Documentation

All analysis documents are in: `/home/james/zephyrproject/elan-zephyr/ai_doc/`

- `1025_1140_ROOT_CAUSE_ANALYSIS.md` - Detailed root cause analysis
- `1025_1140_ISSUE_SUMMARY.md` - This file
- `1025_1140_SHA256_Verification_Pattern_Generator.py` - Python verification script

---

## 🎯 Key Takeaways

1. **The board is BROKEN** - Only processes last chunk
2. **The expected hash is CORRECT** - Verified with Python
3. **Memory is the constraint** - Can't accumulate 400KB in 100KB RAM
4. **State continuation is the solution** - Process chunks with state continuation
5. **Implementation is pending** - Needs proper driver modification

---

**Status**: ✅ ROOT CAUSE IDENTIFIED AND DOCUMENTED

**Next Action**: Implement state continuation in driver to process all chunks correctly

