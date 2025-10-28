# SHA256 Hardware Timeout Issue - Debug Logging Analysis

**Date**: October 26, 2025  
**Status**: 🔍 DEBUGGING IN PROGRESS  
**Issue**: Hardware timeout waiting for SHA256 completion

---

## 🚨 **Problem Identified**

The hardware is timing out when waiting for SHA256 completion:

```
[00:00:00.283,000] <err> crypto_em32_sha: Timeout waiting for SHA256 completion
[00:00:00.283,000] <err> crypto_em32_sha: Failed to process chunk: -116
```

Error code `-116` = `-ETIMEDOUT`

---

## 🔧 **Root Cause Analysis**

The issue is likely one of the following:

1. **Hardware not being started correctly**
   - SHA_STR_BIT not being set properly
   - Control register not being written correctly

2. **Data not being written to hardware**
   - Data words not being written to SHA_IN register
   - Data length not being set correctly

3. **Hardware not ready**
   - SHA_STA_BIT never gets set
   - Hardware stuck in busy state

4. **Interrupt/Status bit issue**
   - SHA_STA_BIT definition incorrect
   - Status bit never transitions

---

## 📊 **Debug Logging Added**

Added comprehensive debug logging to `process_sha256_hardware()` function:

### **Step 1: Hardware Reset**
```c
LOG_DBG("Step 1: Resetting SHA256 hardware");
sha_reset(dev);
sha_configure(dev);
LOG_DBG("Step 1: Hardware reset and configured");
```

### **Step 2: Data Length Setup**
```c
LOG_DBG("Step 2: Setting data length - len_low=%08x, len_high=%08x", len_low, len_high);
sha_write_reg(dev, SHA_DATALEN_OFFSET, len_low);
sha_write_reg(dev, SHA_DATALEN_5832_OFFSET, len_high);
LOG_DBG("Step 2: Data length set");
```

### **Step 3: Data Writing**
```c
LOG_DBG("Step 3: Writing %zu bytes to hardware", data_len);
LOG_DBG("Step 3: Writing %zu words (32-bit values)", words);
for (size_t i = 0; i < words; i++) {
    sha_write_reg(dev, SHA_IN_OFFSET, data32[i]);
    if (i < 4 || i >= words - 4) {
        LOG_DBG("  Word[%zu]=%08x", i, data32[i]);
    }
}
LOG_DBG("Step 3: All data written to hardware");
```

### **Step 4: Start Computation**
```c
LOG_DBG("Step 4: Starting SHA256 computation");
uint32_t ctrl = sha_read_reg(dev, SHA_CTR_OFFSET);
LOG_DBG("Step 4: Current control register: %08x", ctrl);
ctrl |= SHA_STR_BIT;
LOG_DBG("Step 4: New control register (with STR bit): %08x", ctrl);
sha_write_reg(dev, SHA_CTR_OFFSET, ctrl);
LOG_DBG("Step 4: SHA256 computation started");
```

### **Step 5: Wait for Completion**
```c
LOG_DBG("Step 5: Waiting for SHA256 completion (timeout=%u usec)", CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC);
uint32_t timeout = 0;
uint32_t ctrl_val;
while (1) {
    ctrl_val = sha_read_reg(dev, SHA_CTR_OFFSET);
    if (ctrl_val & SHA_STA_BIT) {
        LOG_DBG("Step 5: SHA256 completed after %u iterations", timeout);
        break;
    }
    if (timeout++ > CONFIG_CRYPTO_EM32_SHA_TIMEOUT_USEC) {
        LOG_ERR("Step 5: Timeout waiting for SHA256 completion (ctrl=%08x)", ctrl_val);
        LOG_ERR("Step 5: SHA_STA_BIT=%08x, current ctrl=%08x", SHA_STA_BIT, ctrl_val);
        return -ETIMEDOUT;
    }
    if (timeout % 10000 == 0) {
        LOG_DBG("Step 5: Still waiting... timeout=%u, ctrl=%08x", timeout, ctrl_val);
    }
    k_busy_wait(1);
}
```

### **Step 6: Clear Interrupt**
```c
LOG_DBG("Step 6: Clearing interrupt");
uint32_t ctrl_before = sys_read32(config->base + SHA_CTR_OFFSET);
sys_write32(ctrl_before | SHA_INT_CLR_BIT, config->base + SHA_CTR_OFFSET);
uint32_t ctrl_after = sys_read32(config->base + SHA_CTR_OFFSET);
LOG_DBG("Step 6: Interrupt cleared (before=%08x, after=%08x)", ctrl_before, ctrl_after);
```

### **Step 7: Read State**
```c
LOG_DBG("Step 7: Reading state from hardware registers");
for (int i = 0; i < 8; i++) {
    state[i] = sha_read_reg(dev, SHA_OUT_OFFSET + i * 4);
    LOG_DBG("Step 7: H[%d]=%08x", i, state[i]);
}
```

---

## 📋 **Expected Debug Output**

When working correctly, you should see:

```
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 1: Resetting SHA256 hardware
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 1: Hardware reset and configured
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 2: Setting data length - len_low=00080000, len_high=00000000
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 2: Data length set
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3: Writing 65536 bytes to hardware
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3: Writing 16384 words (32-bit values)
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3:   Word[0]=00010203
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3:   Word[1]=04050607
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3:   Word[2]=08090a0b
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3:   Word[3]=0c0d0e0f
...
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 3: All data written to hardware
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 4: Starting SHA256 computation
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 4: Current control register: 00000300
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 4: New control register (with STR bit): 00000301
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 4: SHA256 computation started
[00:00:00.016,000] <dbg> crypto_em32_sha: Step 5: Waiting for SHA256 completion (timeout=1000000 usec)
[00:00:00.050,000] <dbg> crypto_em32_sha: Step 5: SHA256 completed after 34000 iterations
[00:00:00.050,000] <dbg> crypto_em32_sha: Step 6: Clearing interrupt
[00:00:00.050,000] <dbg> crypto_em32_sha: Step 6: Interrupt cleared (before=00000301, after=00000302)
[00:00:00.050,000] <dbg> crypto_em32_sha: Step 7: Reading state from hardware registers
[00:00:00.050,000] <dbg> crypto_em32_sha: Step 7: H[0]=09a2ac7d
[00:00:00.050,000] <dbg> crypto_em32_sha: Step 7: H[1]=2638045d
...
[00:00:00.050,000] <inf> crypto_em32_sha: Chunk processed and state saved: H0=09a2ac7d H1=2638045d H2=1849a80f H3=fa67fc3d
```

---

## 🎯 **What to Look For**

When you see the debug output, check:

1. **Does Step 1 complete?** - Hardware reset working?
2. **Does Step 2 complete?** - Data length set correctly?
3. **Does Step 3 complete?** - All data written?
4. **Does Step 4 complete?** - Computation started?
5. **Does Step 5 timeout?** - If yes, what is the control register value?
6. **What is SHA_STA_BIT value?** - Is it being set?

---

## 🔍 **Possible Issues to Investigate**

### **Issue 1: SHA_STA_BIT Definition**
Check if `SHA_STA_BIT` is defined correctly:
```c
#define SHA_STA_BIT         BIT(4)  /* Status */
```

### **Issue 2: Hardware Not Responding**
If Step 5 times out immediately with ctrl=0x00000301, the hardware may not be:
- Clocked properly
- Enabled properly
- Responding to commands

### **Issue 3: Data Format**
The data may need to be written in a specific format or order.

### **Issue 4: Padding/Length**
The data length or padding may be incorrect.

---

## 📞 **Next Steps**

1. **Run the firmware** with debug logging enabled
2. **Capture the console output** showing all debug steps
3. **Analyze which step fails** or times out
4. **Identify the root cause** based on the debug output
5. **Implement fix** based on findings

---

**Status**: 🔍 **AWAITING DEBUG OUTPUT**

**Confidence Level**: 🟡 **MEDIUM** - Debug logging will reveal the issue

**Ready for**: Hardware testing with detailed logging

The firmware has been built with comprehensive debug logging. When you run it, the detailed logs will show exactly where the process fails and help identify the root cause!

