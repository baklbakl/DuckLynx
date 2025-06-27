#ifndef UART_H
#define UART_H

#include "stdint.h"

void UART_configure(const uint32_t baseAddress, const uint16_t integerBaudDivider, const uint8_t fractionalBaudDivisor);

void UART_send(const uint32_t baseAddress, const uint8_t c);
void UART_printChar(const uint32_t baseAddress, const char c);
void UART_printString(const uint32_t baseAddress, char * string);

#endif
