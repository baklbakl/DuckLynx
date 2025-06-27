#include "rhsp.h"

#include <stdint.h>
#include "register.h"

//Ideally this file should be platform agnostic so it can be tested without a lynx board

typedef enum : uint16_t {
    RHSP_COMMAND_ACK = 0x7f01,
    RHSP_COMMAND_NACK = 0x7f02,
    RHSP_COMMAND_GET_MODULE_STATUS = 0x7f03,
    RHSP_COMMAND_KEEP_ALIVE = 0x7f04,
    RHSP_COMMAND_FAIL_SAFE = 0x7f05,
    RHSP_COMMAND_SET_NEW_MODULE_ADDRESS = 0x7f06,
    RHSP_COMMAND_QUERY_INTERFACE = 0x7f07,
    RHSP_COMMAND_START_DOWNLOAD = 0x7f08,
    RHSP_COMMAND_DOWNLOAD_CHUNK = 0x7f09,
    RHSP_COMMAND_SET_MODULE_LED_COLOR = 0x7f0a,
    RHSP_COMMAND_GET_MODULE_LED_COLOR = 0x7f0b,
    RHSP_COMMAND_SET_MODULE_LED_PATTERN = 0x7f0c,
    RHSP_COMMAND_GET_MODULE_LED_PATTERN = 0x7f0d,
    RHSP_COMMAND_DEBUG_LOG_LEVEL = 0x7f0e,
    RHSP_COMMAND_DISCOVERY = 0x7f0f
} RHSP_COMMAND;

//Use an enum because for some reason consts aren't actually read only and this needs to size an array, but enums are so ...
enum : uint32_t {
    RHSP_PAYLOAD_MAX_SIZE = 512,
    RHSP_HEADER_SIZE = 10,
    RHSP_CHECKSUM_SIZE = 1,
    RHSP_PAYLOAD_SIZE_SIZE = 1,
    RHSP_BUFFER_SIZE = RHSP_PAYLOAD_MAX_SIZE + RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE
};


union {
    uint8_t buffer[RHSP_PAYLOAD_MAX_SIZE + RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE];

    struct {
        uint16_t magicNumber;
        //The endianness of this may be backwards
        uint16_t packetSize;
        uint8_t destAddress;
        uint8_t srcAddress;
        uint8_t messageNumber;
        uint8_t referenceNumber;
        //The endianness of this may be backwards
        RHSP_COMMAND command;
        //Payload
        //Payload size
        //CRC
    } header __attribute__((packed));
} packet;

void clearBuffer(void) {
    for(int i = 0; i < RHSP_BUFFER_SIZE; i++) {
        packet.buffer[i] = 0;
    }
}   

uint8_t calcChecksum() {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < packet.header.packetSize; i++) {
        sum += packet.buffer[i];
    }
    return sum;
}

uint8_t checkChecksum() {
    return packet.buffer[packet.header.packetSize-1] == calcChecksum();
}

int rhsp_parse(void) {
    clearBuffer();

    uint16_t packetSize = RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE + RHSP_PAYLOAD_SIZE_SIZE;

    packet.buffer[0] = 0x44;
    packet.buffer[1] = 0x4B;
    packet.buffer[2] = packetSize;
    packet.buffer[3] = packetSize >> 8;
    packet.buffer[4] = rhspAddress;
    packet.buffer[5] = 0;
    packet.buffer[6] = 1;
    packet.buffer[7] = 0;
    packet.buffer[8] = RHSP_COMMAND_DISCOVERY;
    packet.buffer[9] = RHSP_COMMAND_DISCOVERY >> 8;
    packet.buffer[11] = 0x12;
    
    return 0;
}

