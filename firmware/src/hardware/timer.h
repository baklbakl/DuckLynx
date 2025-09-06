#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "register.h"

int8_t timer_init(void);

//This number comes from the fact that the clock speed of the system is 80 MHz and the prescaler on the timer is UINT16_MAX.
static const uint32_t TIMER_TICKS_PER_SECOND = 80000000 / UINT16_MAX;
static const uint32_t TIMER_TICKS_PER_100MS = TIMER_TICKS_PER_SECOND / 10;
static volatile uint32_t * const timer_value = REGISTER_GPTM_W1_GPTMTAV;

#endif
