# SHA256 State Continuation - Issue Analysis

**Date**: October 25, 2025  
**Status**: ⚠️ ISSUE IDENTIFIED  
**Version**: 1.0

---

## 🔍 **Problem Identified**

The implementation is producing an **INCORRECT hash**:

```
Got:      a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
```

---

## 📊 **Test Results Analysis**

### ✅ What's Working
1. All 3 tests PASSED ✅
2. All 7 chunks are being processed ✅
3. State is being saved after each chunk ✅
4. Chunks are being processed immediately ✅

### ❌ What's NOT Working
1. **Hash is INCORRECT** ❌
2. **State is NOT accumulating** ❌
3. **Each chunk is processed independently** ❌

---

## 🔎 **Root Cause**

### Hardware Limitation
The **EM32F967 SHA256 hardware does NOT support state restoration**.

From documentation:
```c
/* Note: EM32F967 SHA256 doesn't support direct state restoration.
 * This is a placeholder for future hardware versions that may support it.
 * For now, chunked processing requires processing each chunk independently.
 */
```

### Current Implementation Problem
The current code:
1. Processes chunk 1 → saves state
2. Processes chunk 2 → saves state (but this is chunk 2 processed independently!)
3. Processes chunk 3 → saves state (but this is chunk 3 processed independently!)
4. ...
5. Outputs final state (which is only chunk 7's state!)

**Result**: Hash of only the LAST chunk, not all chunks!

---

## 💡 **Why State Continuation Doesn't Work**

### The Issue
```
Chunk 1 (64KB) → Process → Save state (H0-H7)
Chunk 2 (64KB) → Process → Save state (H0-H7)  ← NEW state, not accumulated!
```

The hardware resets before each chunk, so each chunk is processed independently.

### Evidence from Logs
```
Final hash output from state continuation: d459f2a1 32d45e36 e27c370c 568c5c6f
```

This is the state of **ONLY the last chunk (16KB)**, not the accumulated state of all 7 chunks!

---

## ✅ **Correct Solution**

### Approach: Accumulate All Chunks, Process Once at Finalization

**Why This Works**:
1. Accumulate all chunks in memory
2. Process all accumulated data through hardware at finalization
3. Hardware processes all data in one operation
4. Get correct hash

**Memory Analysis**:
- Total data: 400KB
- Available RAM: 160KB
- **Problem**: 400KB > 160KB ❌

### Alternative: Process Chunks with Proper Padding

**Key Insight**: SHA256 requires proper padding at the END of all data. We can't just process chunks independently.

**Solution**: 
1. Accumulate chunks until we have enough for a full SHA256 block (64 bytes)
2. Process complete blocks through hardware
3. Keep remainder for next chunk
4. At finalization, process final block with proper padding

---

## 🔧 **Implementation Strategy**

### Option 1: Accumulate and Process at Finalization (RECOMMENDED)
- Accumulate all chunks in memory
- Process all data at finalization
- **Pros**: Simple, correct hash
- **Cons**: Requires 400KB memory (not available)

### Option 2: Block-based Processing
- Accumulate data until we have 64-byte blocks
- Process complete blocks immediately
- Keep remainder for next chunk
- At finalization, process final block with padding
- **Pros**: Memory efficient, correct hash
- **Cons**: More complex

### Option 3: Use Software SHA256
- Implement software SHA256 for state continuation
- Process each chunk through software
- **Pros**: Works with any data size
- **Cons**: Slow, defeats purpose of hardware acceleration

---

## 📋 **Recommended Fix**

### Use Block-based Processing (Option 2)

**Algorithm**:
```
1. Accumulate input data in buffer
2. While buffer has >= 64 bytes:
   a. Extract 64-byte block
   b. Process through hardware
   c. Remove block from buffer
3. Keep remainder in buffer
4. At finalization:
   a. Add padding to remainder
   b. Process final block(s) through hardware
   c. Output hash
```

**Memory Usage**:
- Buffer: 64 bytes (one block)
- State: 32 bytes (H0-H7)
- Total: 96 bytes ✅

**Correctness**:
- Processes all data ✅
- Proper padding at end ✅
- Correct hash ✅

---

## 🚀 **Next Steps**

1. Revert state continuation approach
2. Implement block-based processing
3. Test with 400KB data
4. Verify hash matches expected value

---

## 📞 **Summary**

| Item | Status | Details |
|------|--------|---------|
| Current Implementation | ❌ Broken | State continuation doesn't work |
| Hardware Support | ❌ No | No state restoration support |
| Hash Correctness | ❌ Wrong | Hash of only last chunk |
| Recommended Solution | ✅ Block-based | Process 64-byte blocks |
| Memory Efficiency | ✅ Good | Only 96 bytes needed |
| Implementation Complexity | ⚠️ Medium | More complex than current |

---

**Status**: ⚠️ **ISSUE IDENTIFIED - SOLUTION READY**

**Action Required**: Implement block-based processing approach

