#ifndef UART_H
#define UART_H

#include "stdint.h"

void uart_configure(const uint32_t baseAddress, const uint16_t integerBaudDivider, const uint8_t fractionalBaudDivisor);

void uart_send(const uint32_t baseAddress, const uint8_t c);
int8_t uart_read(const uint32_t baseAddress, uint8_t* out);
void uart_printChar(const uint32_t baseAddress, const char c);
void uart_printString(const uint32_t baseAddress, char * string);


#endif
