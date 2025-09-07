#ifndef MOTOR_H
#define MOTOR_H

#include "rhsp/rhsp.h"
#include <stdint.h>

typedef enum : uint8_t {
    MOTOR_MODE_OPEN_LOOP = 0,
    MOTOR_MODE_REGULATED_VELOCITY = 1,
    MOTOR_MODE_REGULATED_POSITION = 2,
} MOTOR_MODE;

typedef enum : uint8_t {
    MOTOR_ZERO_POWER_BEHAVIOR_BRAKE = 0,
    MOTOR_ZERO_POWER_BEHAVIOR_COAST = 1,
} MOTOR_ZERO_POWER_BEHAVIOR;

//This is an enum so it can be used to size an array
enum : uint8_t {
    MOTOR_CHANNEL_COUNT = 4
};

int8_t motor_init(void);

int8_t motor_getEnabled(uint8_t channel, uint8_t * enabled);
RHSP_NACK motor_setEnabled(uint8_t channel, uint8_t enabled);
int8_t motor_getPower(uint8_t channel, int16_t * power);
int8_t motor_setPower(uint8_t channel, int16_t power);



#endif
