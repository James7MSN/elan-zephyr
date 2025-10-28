# Final Analysis Summary - SHA256 Hash Mismatch Issue

**Date**: October 25, 2025  
**Status**: ✅ ROOT CAUSE IDENTIFIED AND DOCUMENTED  
**Version**: 1.0

---

## 🎯 The Problem

The board produces an **INCORRECT SHA256 hash** for 400KB test data:

```
Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Got:      a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
```

---

## 🔍 Root Cause: Only Last Chunk Processed

### Evidence

Console log shows:
```
[00:00:00.049,000] <dbg> crypto_em32_sha: Chunked finalization: src=0x20005a70, total_bytes=16384, total_bits=3276800
```

- `total_bytes=16384` = **16KB (LAST chunk only!)**
- `total_bits=3276800` = 409600 * 8 (correct total, but WRONG data!)

### Why This Happens

The driver stores only a reference to the LAST input buffer:

```c
data->last_input_buf = pkt->in_buf;  // ← OVERWRITES previous!
data->last_input_len = pkt->in_len;
```

Each new chunk call OVERWRITES the previous reference, so only the LAST chunk is retained.

---

## 💾 Why Accumulation Fails

### Memory Constraint

| Item | Size |
|------|------|
| Available RAM | 160KB |
| Test Data | 400KB |
| Result | ❌ ENOMEM |

When trying to accumulate all 400KB:
```
[00:00:00.016,000] <err> crypto_em32_sha: Failed to accumulate chunk data: -12
```

Error -12 = ENOMEM (out of memory)

---

## ✅ Verification: Expected Hash is CORRECT

### Python Script Confirms

```bash
$ python3 ai_doc/1025_1140_SHA256_Verification_Pattern_Generator.py

Full data:    870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Chunked data: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee

✅ HASHES MATCH
```

**Conclusion**: Expected hash is CORRECT. Board is producing WRONG hash.

---

## 🔧 The Solution: State Continuation

### Current Approaches

| Approach | Problem |
|----------|---------|
| ❌ Accumulate all data | Runs out of memory (400KB > 160KB RAM) |
| ❌ Zero-copy (current) | Only stores last chunk reference |
| ✅ State continuation | Process chunks with state continuation |

### How State Continuation Works

```
For each chunk:
  1. Load previous SHA256 state (H0-H7)
     - Or initial values for first chunk
  2. Process chunk through hardware SHA256
  3. Save state (H0-H7)
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

---

## 📋 Current Status

| Item | Status | Details |
|------|--------|---------|
| Root Cause | ✅ Identified | Only last chunk processed |
| Expected Hash | ✅ Verified | 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee |
| Board Output | ❌ Wrong | a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654 |
| Python Verification | ✅ Correct | Verified with Python script |
| Memory Issue | ✅ Resolved | Using zero-copy, no allocation |
| Tests | ✅ Pass | But hash is wrong |
| Solution | ✅ Identified | State continuation required |
| Implementation | ⏳ Pending | Needs proper state continuation |

---

## 📚 Documentation Created

All analysis documents are in: `/home/james/zephyrproject/elan-zephyr/ai_doc/`

1. ✅ `1025_1140_ROOT_CAUSE_ANALYSIS.md` - Detailed root cause analysis
2. ✅ `1025_1140_ISSUE_SUMMARY.md` - Issue summary
3. ✅ `1025_1140_SHA256_Verification_Pattern_Generator.py` - Python verification script
4. ✅ `1025_1140_FINAL_ANALYSIS_SUMMARY.md` - This file

---

## 🎯 Key Findings

1. **The board is BROKEN** - Only processes last chunk
2. **The expected hash is CORRECT** - Verified with Python
3. **Memory is the constraint** - Can't accumulate 400KB in 160KB RAM
4. **State continuation is the solution** - Process chunks with state continuation
5. **Implementation is pending** - Needs proper driver modification

---

## 🚀 Recommendations

### Short Term (Current)

1. ✅ Use zero-copy approach (avoids memory allocation)
2. ✅ Document that only last chunk is processed
3. ✅ Provide Python verification script
4. ✅ Document expected hash

### Medium Term

1. Implement state continuation in driver
2. Process all chunks correctly
3. Produce correct hash
4. Update tests to verify

### Long Term

1. Consider hardware acceleration for state continuation
2. Optimize for performance
3. Support even larger data sizes

---

## 📞 Summary

| Finding | Details |
|---------|---------|
| **Root Cause** | Driver only processes LAST 16KB chunk, not all 400KB |
| **Why** | Zero-copy approach overwrites previous chunk references |
| **Expected Hash** | 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee (CORRECT) |
| **Board Output** | a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654 (WRONG) |
| **Memory Issue** | Can't accumulate 400KB in 160KB available RAM |
| **Solution** | Implement state continuation to process chunks properly |

---

**Status**: ✅ **ROOT CAUSE IDENTIFIED AND DOCUMENTED**

**Next Action**: Implement state continuation in driver to process all chunks correctly and produce the correct hash!

