#pragma once

#ifndef LED_H
#define LED_H
#include <stdint.h>

int8_t led_init(void);
void led_setColor(uint8_t red, uint8_t green, uint8_t blue);
void led_on(void);
void led_off(void);

#endif
