#include "rhsp.h"

#include <stdint.h>

#include "register.h"
#include "debugUART.h"
#include "rhspUART.h"
#include "uart.h"
#include "led.h"

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

typedef enum : uint8_t {
    // 0-9 are for params
    RHSP_NACK_PARAM_0_WRONG = 0,
    RHSP_NACK_PARAM_1_WRONG = 1,
    RHSP_NACK_PARAM_2_WRONG = 2,
    RHSP_NACK_PARAM_3_WRONG = 3,
    RHSP_NACK_PARAM_4_WRONG = 4,
    RHSP_NACK_PARAM_5_WRONG = 5,
    RHSP_NACK_PARAM_6_WRONG = 6,
    RHSP_NACK_PARAM_7_WRONG = 7,
    RHSP_NACK_PARAM_8_WRONG = 8,
    RHSP_NACK_PARAM_9_WRONG = 9,

    // 10-19 are for GPIO output
    RHSP_NACK_GPIO_0_NOT_OUTPUT = 10,
    RHSP_NACK_GPIO_1_NOT_OUTPUT = 11,
    RHSP_NACK_GPIO_2_NOT_OUTPUT = 12,
    RHSP_NACK_GPIO_3_NOT_OUTPUT = 13,
    RHSP_NACK_GPIO_4_NOT_OUTPUT = 14,
    RHSP_NACK_GPIO_5_NOT_OUTPUT = 15,
    RHSP_NACK_GPIO_6_NOT_OUTPUT = 16,
    RHSP_NACK_GPIO_7_NOT_OUTPUT = 17,
    RHSP_NACK_GPIO_ALL_NOT_OUTPUT = 18,
    // NACK 19 is reserved

    // 20-29 are for GPIO input
    RHSP_NACK_GPIO_0_NOT_INPUT = 20,
    RHSP_NACK_GPIO_1_NOT_INPUT = 21,
    RHSP_NACK_GPIO_2_NOT_INPUT = 22,
    RHSP_NACK_GPIO_3_NOT_INPUT = 23,
    RHSP_NACK_GPIO_4_NOT_INPUT = 24,
    RHSP_NACK_GPIO_5_NOT_INPUT = 25,
    RHSP_NACK_GPIO_6_NOT_INPUT = 26,
    RHSP_NACK_GPIO_7_NOT_INPUT = 27,
    RHSP_NACK_GPIO_ALL_NOT_INPUT = 28,
    // NACK 29 is reserved

    // 30-39 are for servos
    RHSP_NACK_SERVO_NOT_CONFIGURED_BEFORE_ENABLE = 30,
    RHSP_NACK_SERVO_CANT_RUN_BATTERY_TOO_LOW = 31,
    // NACKs 32-39 are reserved

    // 40-49 are for i2c
    RHSP_NACK_I2C_MASTER_BUSY = 40,
    RHSP_NACK_I2C_PENDING = 41,
    RHSP_NACK_I2C_NOTHING_PENDING = 42,
    RHSP_NACK_I2C_QUERY_MISMATCH = 43,
    RHSP_NACK_I2C_TIMEOUT_SDA_STUCK = 44,
    RHSP_NACK_I2C_TIMEOUT_SCL_STUCK = 45,
    RHSP_NACK_I2C_TIMEOUT = 46,
    // NACKs 47-49 are reserved

    // 50-59 are for motors
    RHSP_NACK_MOTOR_NOT_CONFIGURED_BEFORE_ENABLE = 50,
    RHSP_NACK_MOTOR_INVALID_COMMAND_FOR_MODE = 51,
    RHSP_NACK_MOTOR_CANT_RUN_BATTERY_TOO_LOW = 52,
    // NACKs 53-59 are reserved

    //There are a few codes at the end for debugging
    RHSP_NACK_COMMAND_IMPLEMENTATION_PENDING = 253,
    RHSP_NACK_COMMAND_ROUTING_ERROR = 254,
    RHSP_NACK_UNKNOWN_COMMAND_ID = 255
} RHSP_NACK;

//Use an enum because for some reason consts aren't actually read only and this needs to size an array, but enums are so ...
enum : uint16_t {
    RHSP_PAYLOAD_MAX_SIZE = 512,
    RHSP_HEADER_SIZE = 10,
    RHSP_CHECKSUM_SIZE = 1,    
    RHSP_BUFFER_SIZE = RHSP_PAYLOAD_MAX_SIZE + RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE
};

const uint16_t RHSP_PACKET_MIN_SIZE = RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE;
const uint16_t RHSP_MAGIC_NUMBER = 0x4B44;
const uint8_t RHSP_MAGIC_NUMBER_BYTE_ONE = RHSP_MAGIC_NUMBER & 0xFF;
const uint8_t RHSP_MAGIC_NUMBER_BYTE_TWO = RHSP_MAGIC_NUMBER >> 8;
const uint8_t RHSP_BROADCAST_ADDRESS = 0xFF;
const uint8_t RHSP_CONTROLLER_ADDRESS = 0x00;

union {
    uint8_t buffer[RHSP_PAYLOAD_MAX_SIZE + RHSP_HEADER_SIZE + RHSP_CHECKSUM_SIZE];

    struct {
        uint16_t magicNumber;
        //The endianness of this may be backwards
        uint16_t packetSize;
        uint8_t destAddress;
        //TEST: Is the src address always zero or does the parent hub repeating packets change its address?
        uint8_t srcAddress;
        uint8_t messageNumber;
        uint8_t referenceNumber;
        //The endianness of this may be backwards
        RHSP_COMMAND command;
        uint8_t payload[];
        //Payload
        //CRC
    } decoded __attribute__((packed));
} packet;

void clearBuffer(void) {
    for(int i = 0; i < RHSP_BUFFER_SIZE; i++) {
        packet.buffer[i] = 0;
    }
}

uint8_t calcChecksum(void) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < packet.decoded.packetSize - 1; i++) {
        sum += packet.buffer[i];
    }
    return sum;
}

uint8_t getPacketChecksum(void) {
    return packet.buffer[packet.decoded.packetSize-1];
}

int8_t rhsp_init() {
    //Maybe not needed, but better to be safe than sorry
    clearBuffer();
    return 0;
}

typedef enum : uint8_t {
    RHSP_PARSE_RESULT_SUCCESS = 0,
    RHSP_PARSE_RESULT_NOT_RHSP,
    RHSP_PARSE_RESULT_INCORRECT_DEST,
    RHSP_PARSE_RESULT_INVALID_CHECKSUM,
    RHSP_PARSE_RESULT_INVALID_COMMAND,
    RHSP_PARSE_RESULT_INVALID_PAYLOAD,
} RHSP_PARSE_RESULT;

const char * toString_PARSE_RESULT(RHSP_PARSE_RESULT result) {
    switch (result) {
        case RHSP_PARSE_RESULT_SUCCESS:
            return "Packet parseing successful.";
        case RHSP_PARSE_RESULT_NOT_RHSP:
            return "Packet not RHSP (no magic number)";
        case RHSP_PARSE_RESULT_INCORRECT_DEST:
            return "Packet not bound for this hub";
        case RHSP_PARSE_RESULT_INVALID_CHECKSUM:
            return "Packet checksum incorrect";
        case RHSP_PARSE_RESULT_INVALID_COMMAND:
            return "Command be handled due to lack of handler or because command is unexpected in this circumstance";
        case RHSP_PARSE_RESULT_INVALID_PAYLOAD:
            return "Payload data is incorrect";
        };
    return "toString_PARSE_RESULT incomplete";
}

void printBuffer(void) {
    for(int i = 0; i < packet.decoded.packetSize; i++) {
        debugUART_printU8Hex(packet.buffer[i]);
        debugUART_printChar(' ');
    }
    debugUART_printChar('\n');
}

void sendPacket(void) {
    packet.decoded.packetSize += RHSP_PACKET_MIN_SIZE;
    packet.decoded.destAddress = packet.decoded.srcAddress;
    packet.decoded.srcAddress = rhspAddress;
    packet.decoded.referenceNumber = packet.decoded.messageNumber;
    packet.buffer[packet.decoded.packetSize-1] = calcChecksum();
    debugUART_printString("Sending response: ");
    printBuffer();
    
    rhspUART_send(packet.buffer, packet.decoded.packetSize);

    // for(int i = 0; i < packet.decoded.packetSize; i++) {
    //     uart_send(REGISTER_UART_0_BASE, packet.buffer[i]);
    // }
}

void sendReadPacket(void) {
    packet.decoded.command = packet.decoded.command | 0x8000;
    sendPacket();
}

void sendNACK(RHSP_NACK nackCode) {
    packet.decoded.command = RHSP_COMMAND_NACK;
    packet.decoded.packetSize = 1;
    packet.decoded.payload[0] = nackCode;

    sendPacket();
}

typedef enum : uint8_t {
    RHSP_MODULE_STATUS_KEEP_ALIVE_TIMEOUT    = 0b000001,
    RHSP_MODULE_STATUS_DEVICE_RESET          = 0b000010,
    RHSP_MODULE_STATUS_FAIL_SAFE             = 0b000100,
    RHSP_MODULE_STATUS_CONTROLLER_OVER_TEMP  = 0b001000,
    RHSP_MODULE_STATUS_BATTERY_LOW           = 0b010000,
    RHSP_MODULE_STATUS_HIB_FAULT             = 0b100000,
} RHSP_MODULE_STATUS;

//ADD: Checks for all the rest of the statuses and motor alerts
RHSP_MODULE_STATUS moduleStatus = RHSP_MODULE_STATUS_DEVICE_RESET;
uint8_t motorAlerts = 0;

void sendACK(void) {
    packet.decoded.command = RHSP_COMMAND_ACK;
    packet.decoded.packetSize = 1;
    //FIX: Check what triggers needs attention
    packet.decoded.payload[0] = moduleStatus != 0 || motorAlerts != 0;

    sendPacket();
}


RHSP_PARSE_RESULT handlePacket(void) {
    debugUART_printString("\nPacket received: ");
    printBuffer();

    //This probably isn't needed
    if(packet.decoded.magicNumber != RHSP_MAGIC_NUMBER) {
        return RHSP_PARSE_RESULT_NOT_RHSP;
    }

    if(getPacketChecksum() != calcChecksum()) {
        return RHSP_PARSE_RESULT_INVALID_CHECKSUM;
    }

    if(packet.decoded.destAddress != rhspAddress && packet.decoded.destAddress != RHSP_BROADCAST_ADDRESS) {
        //ADD: Pass along packets not bound for this hub
        return RHSP_PARSE_RESULT_INCORRECT_DEST;
    }

    packet.decoded.packetSize -= RHSP_PACKET_MIN_SIZE;

    switch(packet.decoded.command) {
    case RHSP_COMMAND_GET_MODULE_STATUS:

        uint8_t resetStatus = 0;
        if(packet.decoded.packetSize != 1 || (resetStatus = packet.decoded.payload[0]) < 0 || resetStatus > 1) {
            sendNACK(RHSP_NACK_PARAM_0_WRONG);
            return RHSP_PARSE_RESULT_INVALID_PAYLOAD;
        }
         
        packet.decoded.packetSize = 2;
        packet.decoded.payload[0] = moduleStatus;
        packet.decoded.payload[1] = motorAlerts;
        
        sendReadPacket();

        if(resetStatus) {
            moduleStatus = 0;
            motorAlerts = 0;
        }

        break;
    case RHSP_COMMAND_KEEP_ALIVE:
        sendACK();
        break;

    //FIX: Disable setting led color because we will never use it and it adds complexity
    case RHSP_COMMAND_SET_MODULE_LED_COLOR:
        if(packet.decoded.packetSize != 3) {
            sendNACK(RHSP_NACK_PARAM_0_WRONG);
            return RHSP_PARSE_RESULT_INVALID_PAYLOAD;
        }

        led_setColor(packet.decoded.payload[0], packet.decoded.payload[1], packet.decoded.payload[2]);
        sendACK();

    case RHSP_COMMAND_FAIL_SAFE:
        //ADD: Content of fail safe command
        sendACK();
        break;
    case RHSP_COMMAND_QUERY_INTERFACE:
        break;
    
    case RHSP_COMMAND_DISCOVERY:
        if(packet.decoded.packetSize != 0) {
            sendNACK(RHSP_NACK_PARAM_0_WRONG);
            return RHSP_PARSE_RESULT_INVALID_PAYLOAD;
        }
        
        packet.decoded.packetSize = 1;
        packet.decoded.payload[0] = 1;

        //ADD: Make the discovery command look for RS485 Children

        sendReadPacket();
        break;

    case RHSP_COMMAND_SET_NEW_MODULE_ADDRESS:
        uint8_t newAddress = 0;
        if(packet.decoded.packetSize != 1 && ((newAddress = packet.decoded.payload[0]) == RHSP_BROADCAST_ADDRESS || newAddress == RHSP_CONTROLLER_ADDRESS)) {
            sendNACK(RHSP_NACK_PARAM_0_WRONG);
            return RHSP_PARSE_RESULT_INVALID_PAYLOAD;
        }

        //ADD: Write new address to EEPROM
        rhspAddress = newAddress;
        break;


    //These will never be implemented
    case RHSP_COMMAND_GET_MODULE_LED_COLOR:
    case RHSP_COMMAND_SET_MODULE_LED_PATTERN:
    case RHSP_COMMAND_GET_MODULE_LED_PATTERN:
    case RHSP_COMMAND_START_DOWNLOAD:
    case RHSP_COMMAND_DOWNLOAD_CHUNK:
    case RHSP_COMMAND_DEBUG_LOG_LEVEL:
        packet.decoded.command = RHSP_COMMAND_NACK;
        packet.decoded.packetSize = RHSP_PACKET_MIN_SIZE + 1;
        packet.decoded.payload[0] = RHSP_NACK_COMMAND_IMPLEMENTATION_PENDING;

        sendPacket();
        break;

    case RHSP_COMMAND_NACK:
    case RHSP_COMMAND_ACK:
    default:
        return RHSP_PARSE_RESULT_INVALID_COMMAND;
        //Never should be here because these are only responses
        break;
    }

    return RHSP_PARSE_RESULT_SUCCESS;
}

uint16_t currPacketLoc = 0;
int8_t rhsp_tick(void) {
    uint8_t byte = 0;
    //Check to make sure there is any data in the FIFO of the serial port
    //FIX: Make sure to return early here if packet buffer is busy with a DMA transmission 
    if(uart_read(REGISTER_UART_0_BASE, &byte) < 0) {
        return 0;
    }

    // debugUART_printString("Data: 0x");
    // debugUART_printU8Hex(byte);
    // debugUART_printChar('\n');

    //This makes sure that we are always aligned with the start of a packet
    switch (currPacketLoc) {
        case 0:
            if(byte != RHSP_MAGIC_NUMBER_BYTE_ONE) {
                currPacketLoc = 0;
                return 0;
            }
            break;
        case 1:
            if(byte != RHSP_MAGIC_NUMBER_BYTE_TWO) {
                currPacketLoc = 0;
                return 0;
            }
            break;    
    }

    packet.buffer[currPacketLoc] = byte;
    currPacketLoc++;

    if(currPacketLoc > 3) {
        if(packet.decoded.packetSize < RHSP_PACKET_MIN_SIZE) {
            currPacketLoc = 0;
        } else if(currPacketLoc == packet.decoded.packetSize) {
            debugUART_printStringConst(toString_PARSE_RESULT(handlePacket()));
            debugUART_printString("\n\n");
            currPacketLoc = 0;
        }
    }
    
    return 0;
}
