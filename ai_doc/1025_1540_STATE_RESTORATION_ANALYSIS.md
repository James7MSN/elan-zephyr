# SHA256 State Restoration - Technical Analysis

**Date**: October 25, 2025  
**Status**: ⚠️ HARDWARE LIMITATION IDENTIFIED  
**Version**: 1.0

---

## 🔍 **The Challenge**

You requested: **"Use state restore method to continue processing next chunk"**

This is the correct approach for chunked SHA256 processing. However, there's a critical hardware limitation.

---

## 📋 **Hardware Register Analysis**

### **Available SHA256 Registers**

From EM32F967 specification (offset 0x40016000):

| Offset | Register | Access | Purpose |
|--------|----------|--------|---------|
| 0x00 | SHA_CTR | RW | Control register |
| 0x04 | SHA_IN | RW | **Input data** |
| 0x08 | SHA_OUT0 | **RO** | Output [255:224] |
| 0x0C | SHA_OUT1 | **RO** | Output [223:192] |
| 0x10 | SHA_OUT2 | **RO** | Output [191:160] |
| 0x14 | SHA_OUT3 | **RO** | Output [159:128] |
| 0x18 | SHA_OUT4 | **RO** | Output [127:96] |
| 0x1C | SHA_OUT5 | **RO** | Output [95:64] |
| 0x20 | SHA_OUT6 | **RO** | Output [63:32] |
| 0x24 | SHA_OUT7 | **RO** | Output [31:0] |
| 0x28 | SHA_DATALEN_5832 | RW | Data length upper |
| 0x2C | SHA_DATALEN | RW | Data length lower |
| 0x30 | SHA_PAD_CTR | RW | Padding control |

### **Critical Finding**

**SHA_OUT registers are READ-ONLY (RO)**

- ✅ Can **READ** state from SHA_OUT0-SHA_OUT7
- ❌ **CANNOT WRITE** state back to hardware
- ❌ **NO input state registers** exist
- ❌ **NO way to restore state** to hardware

---

## 🔧 **Why State Restoration Fails**

### **The Problem**

```c
// This is what we want to do:
uint32_t saved_state[8];

// Process chunk 1
process_chunk(chunk1);
save_state(saved_state);  // ✅ Works - read from SHA_OUT

// Process chunk 2
restore_state(saved_state);  // ❌ FAILS - no registers to write to!
process_chunk(chunk2);
```

### **The Hardware Limitation**

The EM32F967 SHA256 engine:
- ✅ Can compute SHA256 on input data
- ✅ Can output the result (H0-H7)
- ❌ **Cannot accept state input**
- ❌ **Always starts from initial values**
- ❌ **No state input registers**

---

## ✅ **Solution: Software SHA256 Compression**

To implement true state continuation, we need to use a **software SHA256 compression function** that:

1. **Accepts saved state** (H0-H7)
2. **Processes data blocks** (512-bit blocks)
3. **Updates state** in software
4. **Outputs new state** for next chunk

### **Implementation Approach**

```c
// Software SHA256 compression function
void sha256_compress(uint32_t state[8], const uint8_t *data, size_t len)
{
    // Process data in 64-byte blocks
    // Update state[0-7] for each block
    // This is the SHA256 compression function
}

// Chunked processing with state continuation
for each chunk:
    sha256_compress(state, chunk_data, chunk_len);
    // state is now updated for next chunk
```

### **Available Implementation**

The codebase already has **mbedtls SHA256** available:
- Path: `em32f967_spec/SHA256_test_code_20251014/testcodes/encrypt/mbedtls/library/sha256.c`
- Includes: `mbedtls_sha256_update_ret()` - software SHA256 with state continuation
- Supports: Incremental hashing with state preservation

---

## 🎯 **Recommended Implementation**

### **Option 1: Use mbedtls SHA256 (RECOMMENDED)**

```c
#include "mbedtls/sha256.h"

// In driver:
mbedtls_sha256_context ctx;

// Initialize
mbedtls_sha256_starts_ret(&ctx, 0);  // 0 = SHA256

// For each chunk
mbedtls_sha256_update_ret(&ctx, chunk_data, chunk_len);

// Finalize
mbedtls_sha256_finish_ret(&ctx, output);
```

**Pros**:
- ✅ True state continuation
- ✅ Already available in codebase
- ✅ Proven implementation
- ✅ Correct hash output

**Cons**:
- ❌ Software-based (slower than hardware)
- ❌ Uses more CPU

### **Option 2: Hybrid Approach**

```c
// Use hardware for complete blocks
// Use software for remainder and state continuation
```

---

## 📊 **Current Implementation Status**

The code has been updated to:
1. ✅ Process each chunk immediately
2. ✅ Save state after each chunk
3. ❌ **Cannot restore state** (hardware limitation)
4. ❌ **Each chunk processed independently** (not accumulated)

**Result**: Hash of only the last chunk, not all chunks combined

---

## 🚀 **Next Steps**

### **To Implement True State Continuation**

1. **Option A**: Use mbedtls SHA256 for all chunks
   - Replace hardware processing with software
   - Guaranteed correct hash
   - Slower but reliable

2. **Option B**: Hybrid approach
   - Use hardware for large blocks
   - Use software for state continuation
   - More complex but faster

3. **Option C**: Increase accumulation buffer
   - Accumulate all chunks in memory
   - Process all at once
   - Requires more RAM

---

## 📞 **Summary**

| Item | Status | Details |
|------|--------|---------|
| Hardware State Restoration | ❌ Not Supported | SHA_OUT registers are read-only |
| State Input Registers | ❌ Don't Exist | No way to write state to hardware |
| Software SHA256 Available | ✅ Yes | mbedtls implementation available |
| Current Implementation | ⚠️ Incomplete | Saves state but can't restore it |
| Recommended Solution | ✅ mbedtls | Use software SHA256 for state continuation |

---

**Status**: ⚠️ **HARDWARE LIMITATION - SOFTWARE SOLUTION REQUIRED**

**Recommendation**: Use mbedtls SHA256 for true state continuation support

**Action Required**: Implement software-based SHA256 compression function or use mbedtls

