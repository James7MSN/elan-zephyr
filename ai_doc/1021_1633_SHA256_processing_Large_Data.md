# SHA256 Large Data Processing Analysis Report
**Document**: EM32F967 SHA256 Driver Enhancement for >300KB Data Processing  
**Date**: October 21, 2025  
**Time**: 16:33  
**Target**: Improve crypto_em32_sha.c to support >300KB data processing

## Executive Summary

This report analyzes how to enhance the EM32F967 SHA256 driver (`crypto_em32_sha.c`) to efficiently process data larger than 300KB. The analysis compares the current accumulation-based approach with the reference Realtek RTS5912 driver's DMA-based block processing strategy.

## 1. Current EM32F967 Driver Architecture

### 1.1 Processing Strategy: Accumulation Model
- **Approach**: Collects ALL input data before sending to hardware
- **Buffer Management**: 
  - Legacy buffer: 256 bytes (stack)
  - Accumulation buffer: Dynamic heap allocation (8KB initial, 64KB max)
- **Hardware Transfer**: Single-run operation after accumulation complete
- **Limitation**: Maximum 64KB due to `CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE`

### 1.2 Data Flow
```
Input Data → Accumulation Buffer → Hardware Processing → Output
```

### 1.3 Hardware Interface
- **Word-based transfer**: 32-bit words (4 bytes)
- **Synchronization**: Every 16 words (64 bytes) requires SHA_READY check
- **No streaming**: Hardware expects complete message with padding

## 2. Reference RTS5912 Driver Analysis

### 2.1 Processing Strategy: DMA Block Processing
- **Approach**: Processes data in blocks using DMA controller
- **Block Size**: 64 bytes (512 bits)
- **Maximum Block Count**: 0x1FF (511 blocks per DMA transfer)
- **Maximum per transfer**: 511 × 64 = 32,704 bytes
- **Streaming**: Supports continuous block-by-block processing

### 2.2 Key Advantages
1. **No accumulation overhead**: Data processed directly from source
2. **DMA efficiency**: Hardware handles data transfer
3. **Large data support**: Can process >300KB via multiple DMA transfers
4. **Memory efficient**: No need to buffer entire dataset

### 2.3 RTS5912 Processing Loop
```c
for (; blk_size > 0; blk_size -= RTS5912_SHA2DMA_MAXIMUM_BLOCK_NUM) {
    sha2dma_regs->sar = (uint32_t)(&(input[idx]));
    if (blk_size <= RTS5912_SHA2DMA_MAXIMUM_BLOCK_NUM) {
        sha2dma_regs->ctrl_high = blk_size << RTS5912_SHA2DMA_8Byte_SHIFT;
    } else {
        sha2dma_regs->ctrl_high = RTS5912_SHA2DMA_MAXIMUM_BLOCK_NUM 
                                  << RTS5912_SHA2DMA_8Byte_SHIFT;
    }
    // Wait for DMA completion
    // Process next block
}
```

## 3. EM32F967 Hardware Capabilities

### 3.1 SHA256 Register Interface
- **Base Address**: 0x40016000 (APB2)
- **Data Length Registers**: 
  - SHA_DATALEN_5832: Upper 27 bits (bits [58:32])
  - SHA_DATALEN: Lower 32 bits (bits [31:0])
  - **Total range**: 2^59 bits (~288 exabytes)
- **Input Register**: SHA_IN (32-bit words)
- **Output Registers**: SHA_OUT0-7 (8 × 32-bit words)

### 3.2 Hardware Limitations
- **No DMA support**: Unlike RTS5912, EM32F967 requires CPU-driven data transfer
- **No block-based processing**: Must send complete data in single operation
- **Synchronization required**: SHA_READY check every 16 words (64 bytes)

## 4. Recommended Enhancement Strategy

### 4.1 Hybrid Approach for >300KB Support
1. **Keep accumulation for small data** (≤256KB): Current approach works well
2. **Implement chunked processing** for large data (>256KB):
   - Process data in 256KB chunks
   - Each chunk: accumulate → process → clear
   - Maintain running hash state across chunks

### 4.2 Implementation Changes Required
1. **Modify data structure**: Add chunk processing state
2. **Update handler logic**: Detect large data and switch to chunked mode
3. **Implement state preservation**: Save/restore hash state between chunks
4. **Increase max accumulation**: Raise `CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE` to 256KB

### 4.3 Processing Flow for Large Data
```
Chunk 1 (256KB) → Process → Save State
Chunk 2 (256KB) → Process → Save State
...
Final Chunk → Process → Output Result
```

## 5. Key Differences: EM32F967 vs RTS5912

| Aspect | EM32F967 | RTS5912 |
|--------|----------|---------|
| **Data Transfer** | CPU-driven (32-bit words) | DMA-driven (64-byte blocks) |
| **Block Processing** | Single-run only | Multiple blocks via DMA |
| **Max per operation** | Limited by accumulation | 32,704 bytes per DMA |
| **Large data support** | Requires chunking | Native support |
| **Memory efficiency** | Needs full buffer | Streaming capable |
| **Synchronization** | Every 16 words | Every DMA completion |

## 6. Implementation Recommendations

### 6.1 Priority 1: Increase Accumulation Buffer
- Raise `CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE` from 64KB to 256KB
- Supports up to 256KB in single operation
- Minimal code changes required

### 6.2 Priority 2: Implement Chunked Processing
- For data >256KB, process in 256KB chunks
- Requires SHA state preservation between chunks
- More complex but enables unlimited data size

### 6.3 Priority 3: Optimize Synchronization
- Current: Check SHA_READY every 16 words
- Consider: Batch writes with fewer checks
- Trade-off: Performance vs. responsiveness

## 7. Conclusion

The EM32F967 SHA256 driver can be enhanced to support >300KB data through:
1. **Immediate**: Increase accumulation buffer to 256KB
2. **Short-term**: Implement chunked processing for unlimited data
3. **Long-term**: Consider DMA integration if hardware supports it

The RTS5912 reference driver demonstrates effective block-based processing, but EM32F967's CPU-driven architecture requires a different approach optimized for its hardware capabilities.

## 8. Next Steps

1. Modify `crypto_em32_sha.c` to increase buffer size
2. Implement chunked processing for large data
3. Test with 300KB, 1MB, and larger datasets
4. Validate hash correctness across chunk boundaries

