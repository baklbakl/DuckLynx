#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

void gpio_enableAHB(void);
void gpio_enableAltPinFunc(const uint32_t baseAddress, const uint8_t pins, const uint8_t altFunction);
void gpio_enableAnalog(const uint32_t baseAddress, const uint8_t pins);

#endif
