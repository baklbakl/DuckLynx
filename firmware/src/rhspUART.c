#include "rhspUART.h"

#include "sysctl.h"
#include "register.h"
#include "gpio.h"
#include "UART.h"

const uint32_t REGISTER_SYSCTL_PERIPHCTL_UART_0_INSTANCEMASK = 0b1 << 0;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_A_INSTANCEMASK = 0b1 << 0;

const uint32_t REGISTER_GPIO_A_BASE = REGISTER_GPIO_BASE + 0x54000;

const uint32_t REGISTER_UART_0_BASE = REGISTER_UART_BASE + 0x0;

const uint8_t rhsp_pins = REGISTER_GPIO_PIN_0 | REGISTER_GPIO_PIN_1;



int rhspUART_init(void) {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_UART_OFFSET, REGISTER_SYSCTL_PERIPHCTL_UART_0_INSTANCEMASK);
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET, REGISTER_SYSCTL_PERIPHCTL_GPIO_A_INSTANCEMASK);
    
    gpio_enableAltPinFunc(REGISTER_GPIO_A_BASE, rhsp_pins, REGISTER_GPIO_GPIOPCTL_UART);

    //For 460800 baud and a system clock of 80 MHz the integer divisor is 10 and the fractional component is 54. 
    // To compute see datasheet page 926.
    UART_configure(REGISTER_UART_0_BASE, 10, 54);

    return 0;
}

void rhspUART_printSomething(void) {
    UART_printString(REGISTER_UART_0_BASE, "Hello FTDI!\n");
} 

