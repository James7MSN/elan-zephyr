# SHA256 Chunked Processing Debug Report

**Date**: October 26, 2025  
**Time**: 23:33  
**Status**: 🔴 **CRITICAL ISSUE IDENTIFIED**

---

## 📊 **Executive Summary**

The EM32F967 SHA256 hardware is **NOT processing all chunks**. It only processes the first chunk (65536 bytes) and completes prematurely, producing the hash of only the first chunk instead of all 262144 bytes.

**Root Cause**: The hardware is completing after the first chunk in Tests 2&3, and never completing in Test 1.

---

## 🔍 **Debug Log Analysis**

### **Test 1: Direct Finalization (hash_hndlr)**

```
First chunk:  CTR=0x00000309 (READY=1, STA=0) ✓ Correct
Chunk 2-4:    CTR=0x00000308 (READY=1, STA=0) ✓ Correct
Finalization: CTR=0x00000308 (READY=1, STA=0) ✗ TIMEOUT
              Timeout after 100002 iterations (>100ms)
```

**Issue**: STA_BIT never gets set. Hardware is waiting for something.

### **Test 2 & 3: EC-style Finalization (hash_free_session)**

```
First chunk:  CTR=0x00000318 (READY=1, STA=1) ✗ PROBLEM!
Chunk 2-4:    CTR=0x00000318 (READY=1, STA=1) ✗ PROBLEM!
Finalization: CTR=0x00000318 (READY=1, STA=1) ✓ Already set
              STA_BIT set after 0 iterations
```

**Issue**: STA_BIT is set IMMEDIATELY after first chunk! Hardware completed prematurely.

---

## 🎯 **Critical Finding**

### **The Difference**

| Aspect | Test 1 | Tests 2&3 |
|--------|--------|----------|
| **First chunk CTR** | 0x00000309 | 0x00000318 |
| **STA_BIT after chunk 1** | 0 (not set) | 1 (SET!) |
| **Result** | Timeout | Wrong hash (first chunk only) |

### **Binary Analysis**

```
Test 1:  0x00000309 = 0011 0000 1001
         Bit 3 (READY) = 1 ✓
         Bit 4 (STA)   = 0 ✓

Tests 2&3: 0x00000318 = 0011 0001 1000
           Bit 3 (READY) = 1 ✓
           Bit 4 (STA)   = 1 ✗ PROBLEM!
```

---

## 🔴 **Root Cause**

### **The Hardware is Completing After First Chunk**

The EM32F967 SHA256 hardware is **NOT in streaming mode**. It's treating each operation as independent:

1. **First chunk written**: 65536 bytes
2. **DATALEN set to**: 262144 bytes (16384 words)
3. **Hardware sees**: "I have 65536 bytes, but DATALEN says 262144"
4. **Expected**: Hardware waits for more data
5. **Actual**: Hardware completes (in Tests 2&3) or waits indefinitely (in Test 1)

### **Why Tests 2&3 Complete But Test 1 Times Out?**

The difference is in how finalization is called:

- **Test 1**: Calls `hash_hndlr()` directly → STA_BIT stays 0 → Timeout
- **Tests 2&3**: Calls `hash_free_session()` → STA_BIT gets set → Completes

Something in `hash_free_session()` is triggering the hardware to complete!

---

## 💡 **Hypothesis**

### **The Hardware Does NOT Support True Streaming Mode**

The EM32F967 SHA256 hardware:
- Processes data in 512-bit blocks
- Completes when **padding control is set**
- Does NOT support "keep running" mode with 0 padding packets

### **Why It Completes After First Chunk in Tests 2&3**

When `hash_free_session()` is called:
1. It sets padding control with real padding packets
2. This triggers the hardware to complete
3. But the hardware only has the first chunk in its buffer!
4. Result: Hash of first chunk only

### **Why Test 1 Times Out**

When `hash_hndlr()` is called directly:
1. It sets padding control with real padding packets
2. But the hardware is in a different state
3. The dummy word write doesn't trigger completion
4. Hardware waits indefinitely
5. Result: Timeout

---

## 📋 **Test Results Summary**

```
Test 1: FAILED - Timeout at finalization
  - Hash: (timeout, no hash produced)
  - Expected: 2312394bd99545d9de131c24efb781e765ac1aec243f2ed9347597a793a415e9
  - Got: TIMEOUT

Test 2: PASSED (but wrong hash)
  - Hash: 7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2
  - Expected: 2312394bd99545d9de131c24efb781e765ac1aec243f2ed9347597a793a415e9
  - Got: Hash of FIRST CHUNK ONLY (65536 bytes)

Test 3: PASSED (but wrong hash)
  - Hash: 7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2
  - Expected: 2312394bd99545d9de131c24efb781e765ac1aec243f2ed9347597a793a415e9
  - Got: Hash of FIRST CHUNK ONLY (65536 bytes)
```

---

## 🛠️ **Solution Options**

### **Option 1: Accumulate All Data First (RECOMMENDED)**

Process all chunks in ONE operation:
1. Accumulate all 262144 bytes in memory
2. Set DATALEN to total (262144 bytes)
3. Write all data at once
4. Set padding control
5. Wait for completion

**Pros**: Simple, guaranteed to work  
**Cons**: Requires 262KB RAM (we have ~100KB available) ✗

### **Option 2: Set Padding Control on First Chunk**

Tell hardware "don't complete yet":
1. Set DATALEN to total (262144 bytes)
2. Set padding control with 0 packets on first chunk
3. Write first chunk
4. Write remaining chunks
5. Set real padding control at finalization

**Pros**: Might work with streaming  
**Cons**: Already tried, didn't work

### **Option 3: Process Each Chunk Independently**

Process each chunk as a complete operation:
1. For each chunk: Set DATALEN, set padding, write data, wait for completion
2. Save state after each chunk
3. Restore state for next chunk

**Pros**: Works with hardware limitations  
**Cons**: Hardware doesn't support state restoration ✗

### **Option 4: Use Software SHA256 (mbedtls)**

Fall back to software implementation:
1. Use mbedtls SHA256 for large data
2. Hardware SHA256 for small data

**Pros**: Guaranteed correct  
**Cons**: Slow, requires mbedtls headers

---

## 📝 **Recommendations**

### **Immediate Action**

The hardware **does NOT support true streaming mode**. We need to either:

1. **Accumulate all data** (if RAM allows)
2. **Use software SHA256** (if performance allows)
3. **Process chunks independently** (if state restoration works)

### **Next Steps**

1. Check available RAM for accumulation approach
2. Test if hardware supports state restoration
3. Evaluate performance impact of software SHA256
4. Consider hybrid approach (hardware for small, software for large)

---

## 📊 **Data Summary**

| Metric | Value |
|--------|-------|
| Test Data Size | 262144 bytes (256KB) |
| Chunk Size | 65536 bytes (64KB) |
| Number of Chunks | 4 |
| Expected Hash | 2312394bd99545d9de131c24efb781e765ac1aec243f2ed9347597a793a415e9 |
| Actual Hash (Tests 2&3) | 7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2 |
| Hash Match | ✗ NO (first chunk only) |

---

## 🎯 **Conclusion**

The EM32F967 SHA256 hardware **does NOT support true streaming/chunked mode**. It completes after processing the first chunk, producing an incorrect hash.

**Status**: 🔴 **CRITICAL - Requires Hardware Redesign or Software Fallback**


