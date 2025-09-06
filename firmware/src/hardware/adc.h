#ifndef  ADC_H
#define ADC_H

#include "hardware/timer.h"
#include <stdint.h>

int8_t adc_init();
//Reads the raw value of an analog channel. Returns UINT16_MAX on failure
uint16_t adc_readRaw(uint8_t ain);

typedef enum : uint8_t {
    ADC_CHANNEL_USER_0 = 0x0,
    ADC_CHANNEL_USER_1 = 0x1,
    ADC_CHANNEL_USER_2 = 0x2,
    ADC_CHANNEL_USER_3 = 0x3,
    ADC_CHANNEL_GPIO_CURRENT = 0x4,
    ADC_CHANNEL_I2C_CURRENT = 0x5,
    ADC_CHANNEL_SERVO_CURRENT = 0x6,
    ADC_CHANNEL_BATTERY_CURRENT = 0x7,
    ADC_CHANNEL_MOTOR_0_CURRENT = 0x8,
    ADC_CHANNEL_MOTOR_1_CURRENT = 0x9,
    ADC_CHANNEL_MOTOR_2_CURRENT = 0xA,
    ADC_CHANNEL_MOTOR_3_CURRENT = 0xB,
    ADC_CHANNEL_FIVE_VOLTS = 0xC,
    ADC_CHANNEL_BATTERY_VOLTS = 0xD,
    ADC_CHANNEL_CONTROLLER_TEMPERATURE = 0xE,
} ADC_CHANNEL;

int8_t adc_readMapped(ADC_CHANNEL channel, int16_t * outValue, uint8_t raw);

//Result is in deci-degC (0.1 degrees C)
uint16_t adc_readTemp();

void adc_checkBatteryVoltage(uint32_t currentTime);

static const uint32_t ADC_BATTERY_VOLTAGE_READ_INTERVAL = TIMER_TICKS_PER_100MS * 5;
//7000 mv, so 7v
static const int16_t ADC_LOW_BATTERY_CUTOFF_VOLTAGE = 7000;
extern uint32_t adc_lastBatteryVoltageReadTime;


#endif
