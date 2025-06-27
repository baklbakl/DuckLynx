#include "sysctl.h"

#include "register.h"

const uint32_t REGISTER_SYSCTL_PERIPHCTL_CLOCK_GATING_BASE = REGISTER_SYSCTL_BASE + 0x600;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_READY_BASE = REGISTER_SYSCTL_BASE + 0xA00;

void sysctl_waitForReadyPeripheral(const uint8_t peripheralOffset, const uint32_t instanceBitMask) {
      //Wait for the peripheral to ready
      volatile uint32_t * const peripheralReady = (volatile uint32_t * const)(REGISTER_SYSCTL_PERIPHCTL_READY_BASE + peripheralOffset);
      while(!(*peripheralReady & instanceBitMask)) {
          //Busy wait
      }
}

int sysctl_enablePeripheral(const uint8_t peripheralOffset, const uint32_t instanceBitMask) {
    //Enable the clock gating to the peripheral
    *(volatile uint32_t * const)(REGISTER_SYSCTL_PERIPHCTL_CLOCK_GATING_BASE + peripheralOffset) |= instanceBitMask;

    sysctl_waitForReadyPeripheral(peripheralOffset, instanceBitMask);

    return 0;
}

void __attribute__((naked)) sysctl_delay(uint32_t ticks) {
    __asm("    subs    r0, #1\n"
          "    bne     sysctl_delay\n"
          "    bx      lr");
}
