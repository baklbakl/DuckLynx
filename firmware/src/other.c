#include "other.h"
#include "hardware/motor.h"
#include "rhsp/rhsp.h"

void failsafe(void) {
    rhsp_moduleStatus |= RHSP_MODULE_STATUS_FAIL_SAFE;

    for (uint8_t i = 0; i < MOTOR_CHANNEL_COUNT; i++) {
        motor_setEnabled(i, 0);
    }
}

void __attribute__((naked)) delay(uint32_t ticks) {
    __asm("    subs    r0, #1\n"
          "    bne     sysctl_delay\n"
          "    bx      lr");
}
