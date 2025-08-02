#include "dma.h"
#include "sysctl.h"
#include "debugUART.h"
#include "register.h"
#include "uart.h"

enum : uint32_t {
    channelControlTableSize = 1024
};

typedef struct  {
    volatile void * DMASRCENDP;
    volatile void * DMADSTENDP;
    volatile uint32_t DMACHCTL;
    volatile uint32_t RESERVED;

} ChannelControlEntry;
ChannelControlEntry channelControlTable[channelControlTableSize/4] __attribute__((aligned(channelControlTableSize)));

const uint32_t DMACHCTL_XFERMODE_SHIFT = 0;
const uint32_t DMACHCTL_XFERMODE_MASK = 0b111 << DMACHCTL_XFERMODE_SHIFT;
const uint32_t DMACHCTL_XFERMODE_BASIC = 0x1 << DMACHCTL_XFERMODE_SHIFT;

const uint32_t DMACHCTL_XFERSIZE_SHIFT = 4;
const uint32_t DMACHCTL_XFERSIZE_MASK = 0b1111111111 << DMACHCTL_XFERSIZE_SHIFT;

const uint32_t DMACHCTL_ARBSIZE_SHIFT = 14;
const uint32_t DMACHCTL_ARBSIZE_MASK = 0b1111 << DMACHCTL_ARBSIZE_SHIFT;
const uint32_t DMACHCTL_ARBSIZE_FOUR = 0x2 << DMACHCTL_ARBSIZE_SHIFT;

const uint32_t DMACHCTL_SRCSIZE_SHIFT = 24;
const uint32_t DMACHCTL_SRCSIZE_MASK = 0b11 << DMACHCTL_SRCSIZE_SHIFT;
const uint32_t DMACHCTL_SRCSIZE_BYTE = 0b0 << DMACHCTL_SRCSIZE_SHIFT;

const uint32_t DMACHCTL_SRCINC_SHIFT = 26;
const uint32_t DMACHCTL_SRCINC_MASK = 0b11 << DMACHCTL_SRCINC_SHIFT;
const uint32_t DMACHCTL_SRCINC_BYTE = 0x0 << DMACHCTL_SRCINC_SHIFT;
const uint32_t DMACHCTL_SRCINC_NONE = 0x3 << DMACHCTL_SRCINC_SHIFT;

const uint32_t DMACHCTL_DSTSIZE_SHIFT = 28;
const uint32_t DMACHCTL_DSTSIZE_MASK = 0b11 << DMACHCTL_DSTSIZE_SHIFT;
const uint32_t DMACHCTL_DSTSIZE_BYTE = 0b0 << DMACHCTL_DSTSIZE_SHIFT;

const uint32_t DMACHCTL_DSTINC_SHIFT = 30;
const uint32_t DMACHCTL_DSTINC_MASK = 0b11 << DMACHCTL_DSTINC_SHIFT;
const uint32_t DMACHCTL_DSTINC_NONE = 0x3 << DMACHCTL_DSTINC_SHIFT;


const uint8_t REGISTER_SYSCTL_PERIPHCTL_DMA_OFFSET = 0x0C;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_DMA_INSTANCEMASK = 0b1;

const uint32_t REGISTER_DMA_BASE = 0x400FF000;

const uint32_t REGISTER_DMA_CHANNEL_1 = 0b1 << 1;
const uint32_t REGISTER_DMA_CHANNEL_2 = 0b1 << 2;
const uint32_t REGISTER_DMA_CHANNEL_3 = 0b1 << 3;
const uint32_t REGISTER_DMA_CHANNEL_4 = 0b1 << 4;
const uint32_t REGISTER_DMA_CHANNEL_5 = 0b1 << 5;
const uint32_t REGISTER_DMA_CHANNEL_6 = 0b1 << 6;
const uint32_t REGISTER_DMA_CHANNEL_7 = 0b1 << 7;
const uint32_t REGISTER_DMA_CHANNEL_8 = 0b1 << 8;
const uint32_t REGISTER_DMA_CHANNEL_9 = 0b1 << 9;

volatile uint32_t * const REGISTER_DMA_DMACFG = (uint32_t *)(REGISTER_DMA_BASE + 0x04);
const uint32_t REGISTER_DMA_DMACFG_MASTEN = 0b1;

volatile uint32_t * const REGISTER_DMA_DMACTLBASE = (uint32_t *)(REGISTER_DMA_BASE + 0x08); 
const uint32_t REGISTER_DMA_DMACTLBASE_ADDR_MASK = UINT32_MAX & (~(channelControlTableSize - 1));

volatile uint32_t * const REGISTER_DMA_DMAUSEBURSTSET = (uint32_t *)(REGISTER_DMA_BASE + 0x18);
volatile uint32_t * const REGISTER_DMA_DMAENASET = (uint32_t *)(REGISTER_DMA_BASE + 0x028);

volatile uint32_t * const REGISTER_DMA_DMACHMAP1 = (uint32_t *)(REGISTER_DMA_BASE + 0x514);
const uint32_t REGISTER_DMA_DMACHMAPn_CHANNEL_0_SHIFT = 0;
const uint32_t REGISTER_DMA_DMACHMAPn_CHANNEL_0_MASK = 0b1111 << REGISTER_DMA_DMACHMAPn_CHANNEL_0_SHIFT;
const uint32_t REGISTER_DMA_DMACHMAPn_CHANNEL_1_SHIFT = 4;
const uint32_t REGISTER_DMA_DMACHMAPn_CHANNEL_1_MASK = REGISTER_DMA_DMACHMAPn_CHANNEL_0_MASK << REGISTER_DMA_DMACHMAPn_CHANNEL_1_SHIFT;
const uint32_t REGISTER_DMA_DMACHMAP1_CHANNEL_9_UART_0_TX = 0 << REGISTER_DMA_DMACHMAPn_CHANNEL_1_SHIFT;

volatile uint32_t * const REGISTER_UART_UART0_UARTDR = (uint32_t *)(REGISTER_UART_0_BASE + REGISTER_UART_UARTDR_OFFSET);

const uint32_t REGISTER_UART_UARTDMACTL_OFFSET = 0x048;
const uint32_t REGISTER_UART_UARTDMACTL_TXDMAE = 0b1 << 1;

volatile uint32_t * const REGISTER_UART_UART0_UARTDMACTL = (uint32_t *)(REGISTER_UART_0_BASE + REGISTER_UART_UARTDMACTL_OFFSET);

const char * message = "Hello! This is transferred via DMA!";
const uint32_t messageLength = 35;

int8_t dma_init(void) {
    uart_printString(REGISTER_UART_0_BASE, "Hello this is sent normally");

    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_DMA_OFFSET, REGISTER_SYSCTL_PERIPHCTL_DMA_INSTANCEMASK);
    *REGISTER_DMA_DMACFG |= REGISTER_DMA_DMACFG_MASTEN;

    // return 0;
    
    uint32_t channelControlTableAddress = (uint32_t) &channelControlTable;
    if(channelControlTableAddress % channelControlTableSize != 0) {
        debugUART_printString("dmaTable not aligned to 1024 byte boundary\n");
        return 1;
    }
    *REGISTER_DMA_DMACTLBASE = (*REGISTER_DMA_DMACTLBASE & ~(REGISTER_DMA_DMACTLBASE_ADDR_MASK)) | (channelControlTableAddress & REGISTER_DMA_DMACTLBASE_ADDR_MASK);

    *REGISTER_DMA_DMACHMAP1 = (*REGISTER_DMA_DMACHMAP1 & (~REGISTER_DMA_DMACHMAPn_CHANNEL_1_MASK)) | REGISTER_DMA_DMACHMAP1_CHANNEL_9_UART_0_TX;
     //Has to be reset after each transfer
    // *REGISTER_DMA_DMAUSEBURSTSET |= REGISTER_DMA_CHANNEL_9;
    
    ChannelControlEntry * entry = &channelControlTable[9];
    entry->DMADSTENDP = REGISTER_UART_UART0_UARTDR;
    entry->DMASRCENDP = message;
    entry->DMACHCTL =
        DMACHCTL_XFERMODE_BASIC |
        (DMACHCTL_XFERSIZE_MASK & (messageLength << DMACHCTL_XFERSIZE_SHIFT)) |
        DMACHCTL_ARBSIZE_FOUR |
        DMACHCTL_SRCSIZE_BYTE |
        DMACHCTL_SRCINC_BYTE |
        DMACHCTL_DSTSIZE_BYTE |
        DMACHCTL_DSTINC_NONE
    ;

    *REGISTER_UART_UART0_UARTDMACTL |= REGISTER_UART_UARTDMACTL_TXDMAE;
    *REGISTER_DMA_DMAENASET |= REGISTER_DMA_CHANNEL_9;

    return 0;
}
