#ifndef RHSP_UART_H
#define RHSP_UART_H

#include <stdint.h>

extern uint8_t rawBuffer[];
extern uint8_t * rawBufferEnd;

int8_t rhspUART_init(void);
void rhspUART_interruptHandler(void);
void rhspUART_send(uint8_t * buffer, uint16_t bufferLength);
void rhspUART_receive(uint8_t * dest, uint16_t length);

#endif
