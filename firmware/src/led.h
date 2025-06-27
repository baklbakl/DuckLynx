#pragma once

#ifndef LED_H
#define LED_H
#include <stdint.h>

int led_init(void);
void led_setColor(uint32_t red, uint32_t green, uint32_t blue);
void led_on(void);
void led_off(void);

#endif
