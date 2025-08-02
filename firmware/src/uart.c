#include "uart.h"
#include "debugUART.h"
#include "register.h"

const uint32_t REGISTER_UART_UARTFR_OFFSET = 0x018;
const uint32_t REGISTER_UART_UARTFR_TXFF = 0x1 << 5;
const uint32_t REGISTER_UART_UARTFR_RXFE = 0x1 << 4;

const uint32_t REGISTER_UART_UARTIBRD_OFFSET = 0x024;
const uint32_t REGISTER_UART_UARTIBRD_MASK = 0xFFFF;
const uint32_t REGISTER_UART_UARTIBRD_SHIFT = 0;

const uint32_t REGISTER_UART_UARTFBRD_OFFSET = 0x028;
const uint32_t REGISTER_UART_UARTFBRD_MASK = 0b111111;
const uint32_t REGISTER_UART_UARTFBRD_SHIFT = 0;

const uint32_t REGISTER_UART_UARTLCTL_OFFSET = 0x02C;
const uint32_t REGISTER_UART_UARTLCTL_WLEN_SHIFT = 5; 
const uint32_t REGISTER_UART_UARTLCTL_WLEN_MASK = 0b11 << REGISTER_UART_UARTLCTL_WLEN_SHIFT; 
const uint32_t REGISTER_UART_UARTLCTL_WLEN_8 = 0b11 << REGISTER_UART_UARTLCTL_WLEN_SHIFT; 
const uint32_t REGISTER_UART_UARTLCTL_FEN = 0b1 << 4; 

const uint32_t REGISTER_UART_UARTCTL_OFFSET = 0x030;
const uint32_t REGISTER_UART_UARTCTL_UARTEN = 0b1;

void uart_configure(const uint32_t baseAddress, const uint16_t integerBaudDivider, const uint8_t fractionalBaudDivisor) {
    //Disable the UART
    volatile uint32_t * const UARTCTL = (uint32_t *)(baseAddress + REGISTER_UART_UARTCTL_OFFSET);
    *UARTCTL &= ~REGISTER_UART_UARTCTL_UARTEN;

    volatile uint32_t * const UARTIBRD = (uint32_t *)(baseAddress + REGISTER_UART_UARTIBRD_OFFSET);
    volatile uint32_t * const UARTFBRD = (uint32_t *)(baseAddress + REGISTER_UART_UARTFBRD_OFFSET);

    //Set the baud rate
    // To compute see datasheet page 926.
    // The fractional componenet is 64ths
    *UARTIBRD = (*UARTIBRD & ~REGISTER_UART_UARTIBRD_MASK) | (integerBaudDivider & REGISTER_UART_UARTIBRD_MASK);
    *UARTFBRD = (*UARTFBRD & ~REGISTER_UART_UARTFBRD_MASK) | (fractionalBaudDivisor & REGISTER_UART_UARTFBRD_MASK);

    //Enable FIFOs and 8 bit words
    volatile uint32_t * const UARTLCTL = (uint32_t *)(baseAddress + REGISTER_UART_UARTLCTL_OFFSET);
    *UARTLCTL = (*UARTLCTL & ~REGISTER_UART_UARTLCTL_WLEN_MASK) | REGISTER_UART_UARTLCTL_WLEN_8 | REGISTER_UART_UARTLCTL_FEN;

    //Re-enable the UART
    *UARTCTL |= REGISTER_UART_UARTCTL_UARTEN;
    
}

void uart_send(const uint32_t baseAddress, const uint8_t c) {
    volatile uint32_t * const UARTFR = (uint32_t *)(baseAddress + REGISTER_UART_UARTFR_OFFSET);
    //Wait for the FIFO to have space
    while(*(UARTFR) & REGISTER_UART_UARTFR_TXFF) {}

    // Print out a character
    *(volatile uint32_t * const)(baseAddress + REGISTER_UART_UARTDR_OFFSET) = c;
}

void uart_printChar(const uint32_t baseAddress, const char c) {
    uart_send(baseAddress, c);
}

void uart_printString(const uint32_t baseAddress, char * string) {
    while((*string) != 0) {
        uart_printChar(baseAddress, *string);
        string++;
    }
}

int8_t uart_read(const uint32_t baseAddress, uint8_t* out) {
    volatile uint32_t * const UARTFR = (uint32_t *)(baseAddress + REGISTER_UART_UARTFR_OFFSET);
    if(*(UARTFR) & REGISTER_UART_UARTFR_RXFE) {
        return -1;
    }

    *out = *(volatile uint32_t * const)(baseAddress + REGISTER_UART_UARTDR_OFFSET);
    return 0;
}
