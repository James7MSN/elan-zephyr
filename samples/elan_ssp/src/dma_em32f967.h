#ifndef DMA_EM32F967_H
#define DMA_EM32F967_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dma.h>

/* DMA Channel Definitions */
typedef enum {
    DMACH0 = 0,
    DMACH1 = 1,
    DMACH2 = 2,
    DMACH3 = 3,
    DMACH4 = 4,
    DMACH_MAX = 5
} DMAChannel;

/* DMA Width Definitions */
typedef enum {
    SRC8DST8   = 0x00,
    SRC16DST16 = 0x11,
    SRC32DST32 = 0x22
} DMAWidthDef;

/* DMA Handshake Definitions */
typedef enum {
    MEMORY_MEMORY    = 0x00,
    MEMORY_UART1_TX  = 0x01,
    UART1_MEMORY_RX  = 0x02,
    MEMORY_SSP2_TX   = 0x06,
    SSP2_MEMORY_RX   = 0x07,
    MEMORY_SPI1_TX   = 0x08,
    SPI1_MEMORY_RX   = 0x09
} DMAHandShake;

/* DMA API Functions */
uint32_t RequestDMAChannel(DMAChannel channel);
void ReleaseAllDMA(void);
uint32_t MEMDMASetting(DMAChannel channel, uint32_t SrcAddr, uint32_t DstAddr, 
                       uint32_t Length, DMAWidthDef SDWidth);
uint32_t PHERDMASetting(DMAChannel channel, DMAHandShake HSnumber, 
                        uint32_t MemAddr, uint32_t Length, DMAWidthDef SDWidth);
void DMAStart(DMAChannel channel);
bool CheckDMADone(DMAChannel channel);
void WaitDMADone(DMAChannel channel);

/* SSP DMA Functions */
void SPI2DMASetStart(uint32_t TXAddr, uint32_t RXAddr, uint32_t length);
void SPI2DMASetStop(void);
void WaitSPI2DMADone(void);
bool CheckSPI2DMADone(void);

#endif /* DMA_EM32F967_H */
