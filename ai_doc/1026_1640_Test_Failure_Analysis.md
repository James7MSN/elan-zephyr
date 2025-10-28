# Test Failure Analysis - Test 1 Timeout vs Test 2/3 Hash Mismatch

**Date**: October 26, 2025  
**Time**: 16:40  
**Status**: Two different issues identified

---

## 📊 **Test Results Summary**

```
Test 1: FAILED - Timeout at finalization
Test 2: PASSED (but hash wrong) - Hash of first chunk only
Test 3: PASSED (but hash wrong) - Hash of first chunk only
```

---

## 🔴 **Issue 1: Test 1 Timeout**

### **Problem**

```
[00:00:00.016,000] <inf> crypto_em32_sha: Switching to chunked processing...
[00:00:00.410,000] <err> crypto_em32_sha: Timeout waiting for SHA256 completion at finalization
```

**Timeout occurs**: ~394ms after first chunk

### **Root Cause**

Test 1 uses a **different finalization method** than Tests 2 & 3:

```c
/* Test 1: Direct handler call */
ret = ctx.hash_hndlr(&ctx, &pkt, true);  /* Calls hash_free_session directly */

/* Test 2 & 3: Explicit hash_free_session call */
hash_free_session(crypto_dev, &ctx);
```

**The Problem**: Test 1 calls `ctx.hash_hndlr()` with `true` flag, which is the **internal handler function**, not the public API.

This bypasses the normal flow and may not properly handle the chunked finalization.

### **Why Test 2 & 3 Don't Timeout**

Tests 2 & 3 call `hash_free_session()` explicitly, which:
1. Properly detects chunked mode
2. Sets padding control
3. Waits for completion
4. Returns successfully

---

## 🟡 **Issue 2: Hash Mismatch (Tests 2 & 3)**

### **Problem**

```
Expected: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
Got:      7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2
```

**Finding**: Still producing hash of **first chunk only**!

### **Root Cause: Padding Control Not Being Set**

The issue is that we're setting padding control in `hash_free_session()`, but:

1. **Test 2 & 3 call `hash_free_session()` AFTER all chunks**
2. **But the driver's `hash_free_session()` handler doesn't know about chunked mode**

Looking at the code flow:

```c
/* In hash_free_session() */
if (data->use_chunked) {
    /* Set padding control */
    sys_write32(pad_ctrl, config->base + SHA_PAD_CTR_OFFSET);
    
    /* Wait for completion */
    while (!(sys_read32(...) & SHA_STA_BIT)) { ... }
}
```

**Problem**: The `hash_free_session()` is being called, but the hardware might have **already completed** after the first chunk because:

1. We removed padding control from first chunk ✓
2. But we're not preventing the hardware from completing early

The hardware needs a **signal** that more data is coming. Without padding control, it might still complete after processing the first chunk.

---

## ✅ **Solution**

### **For Test 1: Use Proper Finalization**

Test 1 should call `hash_free_session()` explicitly, not `ctx.hash_hndlr()`:

```c
/* WRONG - Test 1 */
ret = ctx.hash_hndlr(&ctx, &pkt, true);

/* CORRECT - Test 1 */
ret = hash_free_session(crypto_dev, &ctx);
```

### **For Tests 2 & 3: Ensure Padding Control is Set**

The real issue is that **padding control must be set BEFORE the hardware completes**.

We need to ensure the hardware **doesn't complete** until we explicitly tell it to via padding control.

**Options**:
1. Set a **dummy padding control** on first chunk to keep hardware running
2. Or: Set padding control **immediately after last chunk**, not in `hash_free_session()`

---

## 🔧 **Recommended Fix**

### **Approach: Set Padding Control After Last Chunk**

Instead of setting padding in `hash_free_session()`, set it in `hash_update()` when we detect the **last chunk**:

```c
/* In hash_update() - after writing chunk data */
if (is_last_chunk) {
    /* Calculate and set padding control */
    uint32_t pad_ctrl = ...;
    sys_write32(pad_ctrl, config->base + SHA_PAD_CTR_OFFSET);
}
```

**Problem**: We don't know if it's the last chunk in `hash_update()`.

### **Better Approach: Use a Flag**

```c
/* In hash_update() */
if (pkt->in_len == 0) {
    /* Zero-length input = finalization signal */
    /* Set padding control here */
    sys_write32(pad_ctrl, config->base + SHA_PAD_CTR_OFFSET);
}
```

This way:
- Chunks 1-6: Write data, no padding
- Chunk 7: Write data, no padding
- Finalization (zero-length): Set padding, wait for completion

---

## 📋 **Implementation Plan**

### **Step 1: Fix Test 1**

Change test to use `hash_free_session()` instead of `ctx.hash_hndlr()`.

### **Step 2: Fix Hash Mismatch**

Modify `hash_update()` to detect finalization (zero-length input) and set padding control then.

### **Step 3: Verify**

All tests should:
- ✅ Complete without timeout
- ✅ Produce correct hash (all 400KB)
- ✅ Pass verification

---

## 🎯 **Key Insight**

The hardware needs **two signals**:
1. **Data length register**: "I'm expecting 409600 bytes total"
2. **Padding control**: "I've received all data, add padding and complete"

Without the second signal, the hardware might complete prematurely or wait indefinitely.

The timing of when we set padding control is **critical** for correct operation.

