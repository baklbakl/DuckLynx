#include "gpio.h"
#include "debugUART.h"
#include "register.h"
#include <stdint.h>

//Switch all gpio ports to AHB
volatile uint32_t * const REGISTER_SYSCTL_GPIOHBCTL = (uint32_t *)(REGISTER_SYSCTL_BASE + 0x06C);
const uint32_t REGISTER_SYSCTL_GPIOHBCTL_ALLPORTS = 0xFFF;
void gpio_enableAHB(void) {
    *REGISTER_SYSCTL_GPIOHBCTL |= REGISTER_SYSCTL_GPIOHBCTL_ALLPORTS;   
}

//GPIO registers
const uint32_t REGISTER_GPIO_AFSEL_OFFSET = 0x420;

const uint32_t REGISTER_GPIO_GPIOPCTL_OFFSET = 0x52C;
const uint32_t REGISTER_GPIO_GPIOPCTL_MASK = 0b1111;

void gpio_enableAltPinFunc(const uint32_t baseAddress, const uint8_t pins, const uint8_t altFunction) {
    //Enable the alternate function mode and disable GPIO mode
    *(volatile uint32_t * const)(baseAddress + REGISTER_GPIO_AFSEL_OFFSET) |= pins;

    volatile uint32_t * const PCTL = (uint32_t * const)(baseAddress + REGISTER_GPIO_GPIOPCTL_OFFSET);

    uint32_t oldPCTL = *PCTL;

    uint8_t pinsCounter = pins;
    uint8_t count = 0;

    while(pinsCounter > 0) {
        if(pinsCounter & 1) {
            oldPCTL &= ~(REGISTER_GPIO_GPIOPCTL_MASK << (count * 4U));
            oldPCTL |= (uint32_t)(altFunction << (count * 4U));
        }
        count++;
        pinsCounter /= 2;
    }

    *PCTL = oldPCTL;

    //Enable the digital out
    *(volatile uint32_t * const)(baseAddress + REGISTER_GPIO_GPIODEN_OFFSET) |= pins;
}
