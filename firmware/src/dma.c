#include "dma.h"
#include "sysctl.h"
#include "debugUART.h"
#include "register.h"

enum : uint32_t {
    channelControlTableSize = 1024
};

ChannelControlEntry channelControlTable[channelControlTableSize/4] __attribute__((aligned(channelControlTableSize)));

const uint8_t REGISTER_SYSCTL_PERIPHCTL_DMA_OFFSET = 0x0C;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_DMA_INSTANCEMASK = 0b1;

volatile uint32_t * const REGISTER_DMA_DMACFG = (uint32_t *)(REGISTER_DMA_BASE + 0x04);
const uint32_t REGISTER_DMA_DMACFG_MASTEN = 0b1;

volatile uint32_t * const REGISTER_DMA_DMACTLBASE = (uint32_t *)(REGISTER_DMA_BASE + 0x08); 
const uint32_t REGISTER_DMA_DMACTLBASE_ADDR_MASK = UINT32_MAX & (~(channelControlTableSize - 1));

int8_t dma_init(void) {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_DMA_OFFSET, REGISTER_SYSCTL_PERIPHCTL_DMA_INSTANCEMASK);
    *REGISTER_DMA_DMACFG |= REGISTER_DMA_DMACFG_MASTEN;

    // return 0;
    
    uint32_t channelControlTableAddress = (uint32_t) &channelControlTable;
    if(channelControlTableAddress % channelControlTableSize != 0) {
        debugUART_printString("dmaTable not aligned to 1024 byte boundary\n");
        return 1;
    }
    *REGISTER_DMA_DMACTLBASE = (*REGISTER_DMA_DMACTLBASE & ~(REGISTER_DMA_DMACTLBASE_ADDR_MASK)) | (channelControlTableAddress & REGISTER_DMA_DMACTLBASE_ADDR_MASK);

    return 0;
}
