#ifndef RHSP_H
#define RHSP_H

#include <stdint.h>
#include "hardware/timer.h"

//Use an enum because for some reason consts aren't actually read only and this needs to size an array, but enums are so ...
enum : uint16_t {
    RHSP_PAYLOAD_MAX_SIZE = 512,
    RHSP_HEADER_SIZE = 10,
    RHSP_CHECKSUM_SIZE = 1,    
    RHSP_PACKET_MIN_SIZE = RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE,
    RHSP_PACKET_MAX_SIZE = RHSP_PAYLOAD_MAX_SIZE + RHSP_PACKET_MIN_SIZE,
    //The buffer size has an extra RHSP_PACKET_MIN_SIZE - 1 tacked onto the end so that if the last 
    // byte of waiting-for-packet RHSP_PACKET_MIN_SIZE byte dma transfer is the start of a packet 
    // and it is the max length that we support there will be enough room in the buffer for it
    RHSP_BUFFER_SIZE = RHSP_PACKET_MAX_SIZE + RHSP_PACKET_MIN_SIZE - 1
};

typedef enum : uint8_t {
    RHSP_MODULE_STATUS_KEEP_ALIVE_TIMEOUT    = 0b000001,
    RHSP_MODULE_STATUS_DEVICE_RESET          = 0b000010,
    RHSP_MODULE_STATUS_FAIL_SAFE             = 0b000100,
    RHSP_MODULE_STATUS_CONTROLLER_OVER_TEMP  = 0b001000,
    RHSP_MODULE_STATUS_BATTERY_LOW           = 0b010000,
    RHSP_MODULE_STATUS_HIB_FAULT             = 0b100000,
} RHSP_MODULE_STATUS;
volatile extern RHSP_MODULE_STATUS rhsp_moduleStatus;

static const uint32_t RHSP_TIMEOUT = TIMER_TICKS_PER_100MS * 25;

int8_t rhsp_init(void);
void rhsp_tick(uint8_t * location, uint16_t count);

volatile extern uint32_t rhsp_lastCommandTime;

#endif
