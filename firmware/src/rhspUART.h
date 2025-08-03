#ifndef RHSP_UART_H
#define RHSP_UART_H

#include <stdint.h>

int8_t rhspUART_init(void);
void rhspUART_interruptHandler(void);
void rhspUART_send(uint8_t * buffer, uint16_t bufferLength);

#endif
