#ifndef DMA_H
#define DMA_H

#include <stdint.h>

typedef struct {
    volatile void * DMASRCENDP;
    volatile void * DMADSTENDP;
    volatile uint32_t DMACHCTL;
    volatile uint32_t RESERVED;
} ChannelControlEntry;

extern ChannelControlEntry channelControlTable[];

int8_t dma_init(void);

#endif
