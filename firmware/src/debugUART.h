#pragma once

#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#include <stdint.h>

extern int debugUart_Enabled;

int debugUART_init(void);
void debugUART_printChar(char c);
void debugUART_printString(char * string);
void debugUART_printStringConst(const char *string);
void debugUART_printWordHex(uint32_t word);
void debugUART_printWord(uint32_t word);

#endif
