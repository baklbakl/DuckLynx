#ifndef SYSCTL_H
#define SYSCTL_H
#include "stdint.h"

void sysctl_waitForReadyPeripheral(uint8_t peripheralOffset, uint32_t instanceBitMask);
int sysctl_enablePeripheral(uint8_t peripheralOffset, uint32_t instanceBitMask);
void sysctl_delay(uint32_t ticks);

#endif
