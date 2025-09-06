#include "debugUART.h"

#include "hardware/gpio.h"
#include "hardware/register.h"
#include "hardware/sysctl.h"
#include "hardware/uart.h"

const uint32_t REGISTER_SYSCTL_PERIPHCTL_UART_4_INSTANCEMASK = 0b1 << 4;
const uint32_t REGISTER_UART_4_BASE = REGISTER_UART_BASE + 0x4000;

const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_J_INSTANCEMASK = 0b1 << 8;
const uint32_t REGISTER_GPIO_J_BASE = REGISTER_GPIO_BASE + 0x5C000;

const uint8_t debugUART_pins = REGISTER_GPIO_PIN_0 | REGISTER_GPIO_PIN_1;

int8_t debugUART_init(void) {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_UART_OFFSET, REGISTER_SYSCTL_PERIPHCTL_UART_4_INSTANCEMASK);
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET, REGISTER_SYSCTL_PERIPHCTL_GPIO_J_INSTANCEMASK);
    
    gpio_enableAltPinFunc(REGISTER_GPIO_J_BASE, debugUART_pins, REGISTER_GPIO_GPIOPCTL_UART);

    //For 115200 baud and a system clock of 80 MHz the integer divisor is 43 and the fractional component is 26. 
    // To compute see datasheet page 926.
    uart_configure(REGISTER_UART_4_BASE, 43, 26);

    return 0;
}

void debugUART_printChar(char c) {
    uart_printChar(REGISTER_UART_4_BASE, c);
}

void debugUART_printString(char * string) {
    uart_printString(REGISTER_UART_4_BASE, string);
}

//This function is here just to make the compiler happys
void debugUART_printStringConst(const char *string) {
    debugUART_printString((char *) string);
}

void printHexDigit(char c) {
    if(c <= 9) {
        c += 0x30;
    } else {
        c += 0x37;
    }
    debugUART_printChar(c);
}

void debugUART_printU8Hex(uint8_t c) {
    for(uint8_t byteI = 1; byteI <= 1; byteI--) {
        printHexDigit((c >> (byteI * 4)) & 0b1111);
    }
}

void debugUART_printWordHex(uint32_t word) {
    for(uint32_t byteI = 7; byteI <= 7; byteI--) {
        printHexDigit((word >> (byteI * 4)) & 0b1111);
    }
}

void debugUART_printWord(uint32_t word) {
    int divisor = 1;

    uint32_t wordCopy = word;
    while (wordCopy > 9) {
        wordCopy /= 10;
        divisor *= 10;
    }
    
    do {
        debugUART_printChar((word / divisor % 10) + 0x30);
        divisor /= 10;
    } while(divisor > 0);
}
