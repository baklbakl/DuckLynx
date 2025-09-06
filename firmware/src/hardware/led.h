#pragma once

#include "hardware/timer.h"
#ifndef LED_H
#define LED_H
#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint8_t tenths;
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} led_PatternStep;

typedef struct {
    led_PatternStep * steps;
    uint8_t totalSteps;
} led_Pattern;

typedef enum {
    LED_MODE_NO_BATTERY,
    LED_MODE_CONNECTED,
    LED_MODE_RESET,
    LED_MODE_TIMEOUT,
    LED_MODE_USER,
    // LED_MODE_USER_PATTERN,

} LED_MODE;

int8_t led_init(void);
void led_setColor(const uint8_t red, const uint8_t green, const uint8_t blue);
void led_getColor(uint8_t * red, uint8_t * green, uint8_t * blue);
void led_on(void);
void led_off(void);
void led_dumpPattern(void);
void led_setupPatternWithTimeForce(const LED_MODE mode, const led_Pattern *pattern, const uint32_t time);
void led_setupPatternWithTime(const LED_MODE mode, const led_Pattern *pattern, const uint32_t time);
void led_setupPattern(const LED_MODE mode, const led_Pattern *pattern);

enum : uint8_t {
    LED_MAX_PATTERN_STEPS = 16
};

extern volatile LED_MODE led_mode;

extern volatile uint32_t led_lastStepTime;
extern volatile led_Pattern * led_currentPattern;
extern volatile uint8_t led_currentPatternStep;

extern const led_Pattern led_noBatteryPattern;
extern LED_MODE led_oldMode;
extern led_Pattern * led_oldPattern;

extern const led_Pattern led_timeoutPattern;
extern const led_Pattern led_connectedPattern;
extern const led_Pattern led_resetPattern;
extern volatile led_Pattern led_userPattern;

#endif
