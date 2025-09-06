#include "hardware/motor.h"
#include "debugUART.h"
#include "hardware/gpio.h"
#include "hardware/register.h"
#include "sysctl.h"

const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_SHFIT = 17;
const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_MASK = 0b111 << REGISTER_SYSCTL_RCC_PWMDIV_SHFIT;
const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_DIV_16 = 0x3 << REGISTER_SYSCTL_RCC_PWMDIV_SHFIT;
const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_DIV_64 = 0x5 << REGISTER_SYSCTL_RCC_PWMDIV_SHFIT;
const uint32_t REGISTER_SYSCTL_RCC_USEPWMDIV = 0b1 << 20;

const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_F_INSTANCEMASK = 0b1 << 5;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_H_INSTANCEMASK = 0b1 << 7;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_K_INSTANCEMASK = 0b1 << 9;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_N_INSTANCEMASK = 0b1 << 12;

const uint8_t REGISTER_SYSCTL_PERIPHCTL_PWM_OFFSET = 0x40;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_PWM_0_INSTANCEMASK = 0b1 << 0;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_PWM_1_INSTANCEMASK = 0b1 << 1;

const uint32_t REGISTER_GPIO_GPIOODR_OFFSET = 0x50C;
const uint32_t REGISTER_GPIO_GPIOPCTL_H_PWM = 0x4;

const uint8_t MOTOR_0_INA_PIN = REGISTER_GPIO_PIN_6;
const uint8_t MOTOR_0_INB_PIN = REGISTER_GPIO_PIN_2;
const uint8_t MOTOR_0_PWM_PIN = REGISTER_GPIO_PIN_0;
const uint8_t MOTOR_0_EN_PIN = REGISTER_GPIO_PIN_1;

const uint32_t REGISTER_GPIO_F_BASE = REGISTER_GPIO_BASE + 0x59000;
volatile uint32_t * const REGISTER_GPIO_F_GPIODR = (uint32_t *)(REGISTER_GPIO_F_BASE + REGISTER_GPIO_GPIODR_OFFSET + (MOTOR_0_INB_PIN << 2));
volatile uint32_t * const REGISTER_GPIO_F_GPIODIR = (uint32_t *)(REGISTER_GPIO_F_BASE + REGISTER_GPIO_GPIODIR_OFFSET);
volatile uint32_t * const REGISTER_GPIO_F_GPIODEN = (uint32_t *)(REGISTER_GPIO_F_BASE + REGISTER_GPIO_GPIODEN_OFFSET);

const uint32_t REGISTER_GPIO_H_BASE = REGISTER_GPIO_BASE + 0x5B000;
// volatile uint32_t * const REGISTER_GPIO_H_GPIODR = (uint32_t *)(REGISTER_GPIO_H_BASE + REGISTER_GPIO_GPIODR_OFFSET + (PWM_PIN << 2));
// volatile uint32_t * const REGISTER_GPIO_H_GPIODIR = (uint32_t *)(REGISTER_GPIO_H_BASE + REGISTER_GPIO_GPIODIR_OFFSET);
// volatile uint32_t * const REGISTER_GPIO_H_GPIODEN = (uint32_t *)(REGISTER_GPIO_H_BASE + REGISTER_GPIO_GPIODEN_OFFSET);

const uint32_t REGISTER_GPIO_K_BASE = REGISTER_GPIO_BASE + 0x5D000;
volatile uint32_t * const REGISTER_GPIO_K_GPIODR = (uint32_t *)(REGISTER_GPIO_K_BASE + REGISTER_GPIO_GPIODR_OFFSET + (MOTOR_0_INA_PIN << 2));
volatile uint32_t * const REGISTER_GPIO_K_GPIODIR = (uint32_t *)(REGISTER_GPIO_K_BASE + REGISTER_GPIO_GPIODIR_OFFSET);
volatile uint32_t * const REGISTER_GPIO_K_GPIODEN = (uint32_t *)(REGISTER_GPIO_K_BASE + REGISTER_GPIO_GPIODEN_OFFSET);

const uint32_t REGISTER_GPIO_N_BASE = REGISTER_GPIO_BASE + 0x60000;
volatile uint32_t * const REGISTER_GPIO_N_GPIODR = (uint32_t *)(REGISTER_GPIO_N_BASE + REGISTER_GPIO_GPIODR_OFFSET + (MOTOR_0_EN_PIN << 2));
volatile uint32_t * const REGISTER_GPIO_N_GPIODIR = (uint32_t *)(REGISTER_GPIO_N_BASE + REGISTER_GPIO_GPIODIR_OFFSET);
volatile uint32_t * const REGISTER_GPIO_N_GPIOODR = (uint32_t *)(REGISTER_GPIO_N_BASE + REGISTER_GPIO_GPIOODR_OFFSET);
volatile uint32_t * const REGISTER_GPIO_N_GPIODEN = (uint32_t *)(REGISTER_GPIO_N_BASE + REGISTER_GPIO_GPIODEN_OFFSET);

//ADD: GPIO ports M and G

typedef struct {
    uint32_t gpioBase;
    uint8_t pinBitmask;
    volatile uint32_t * const DR;
} Pin;

typedef struct {
    Pin INA;
    Pin INB;
    Pin EN;
    Pin PWM;
} MotorPins;

MotorPins motorPins[] = {
    {
        {
            REGISTER_GPIO_K_BASE,
            REGISTER_GPIO_PIN_6
        },
        {
            REGISTER_GPIO_F_BASE,
            REGISTER_GPIO_PIN_2,
        },
        {
            REGISTER_GPIO_N_BASE,
            REGISTER_GPIO_PIN_1
        },
        {
            REGISTER_GPIO_H_BASE,
            REGISTER_GPIO_PIN_0
        }
    },
    {

    },
    {

    },
    {

    },
};

const uint32_t REGISTER_PWM_BASE = 0x40028000;
const uint32_t REGISTER_PWM_PWMENABLE_OFFSET = 0x8;
const uint32_t REGISTER_PWM_PWMENABLE_PWM0EN = 0b00000001;
const uint32_t REGISTER_PWM_PWMENABLE_PWM1EN = 0b00000010;
const uint32_t REGISTER_PWM_PWMENABLE_PWM2EN = 0b00000100;
const uint32_t REGISTER_PWM_PWMENABLE_PWM3EN = 0b00001000;
const uint32_t REGISTER_PWM_PWMENABLE_PWM4EN = 0b00010000;
const uint32_t REGISTER_PWM_PWMENABLE_PWM5EN = 0b00100000;
const uint32_t REGISTER_PWM_PWMENABLE_PWM6EN = 0b01000000;
const uint32_t REGISTER_PWM_PWMENABLE_PWM7EN = 0b10000000;

const uint32_t REGISTER_PWM_PWMENABLE_ALL = REGISTER_PWM_PWMENABLE_PWM0EN | REGISTER_PWM_PWMENABLE_PWM1EN |
                                            REGISTER_PWM_PWMENABLE_PWM2EN | REGISTER_PWM_PWMENABLE_PWM3EN | 
                                            REGISTER_PWM_PWMENABLE_PWM4EN | REGISTER_PWM_PWMENABLE_PWM5EN | 
                                            REGISTER_PWM_PWMENABLE_PWM6EN | REGISTER_PWM_PWMENABLE_PWM7EN;

const uint32_t REGISTER_PWM_PWM0LOAD_OFFSET = 0x50;
const uint32_t REGISTER_PWM_PWM0LOAD_MASK = 0xFFFF;
const uint32_t REGISTER_PWM_PWM0CTL_OFFSET = 0x40;
const uint32_t REGISTER_PWM_PWM0CTL_ENABLE = 0b1 << 0;
const uint32_t REGISTER_PWM_PWM0CTL_COUNTER_OFFSET = 1;
const uint32_t REGISTER_PWM_PWM0CTL_COUNTER_UP_DOWN = 1 << REGISTER_PWM_PWM0CTL_COUNTER_OFFSET;
const uint32_t REGISTER_PWM_PWM0CTL_COUNTER_DOWN = 0 << REGISTER_PWM_PWM0CTL_COUNTER_OFFSET;

const uint32_t REGISTER_PWM_PWM0CMPA_OFFSET = 0x58;
const uint32_t REGISTER_PWM_PWM0CMPB_OFFSET = 0x5C;
const uint32_t REGISTER_PWM_PWM0CMPn_MASK = 0xFFFF;
const uint32_t REGISTER_PWM_PWM0GENA_OFFSET = 0x60;
const uint32_t REGISTER_PWM_PWM0GENB_OFFSET = 0x64;

const uint32_t REGISTER_PWM_PWMnGENn_ACTn_MASK = 0b11;
const uint32_t REGISTER_PWM_PWMnGENn_ACTZERO_OFFSET = 0;
const uint32_t REGISTER_PWM_PWMnGENn_ACTCMPAU_OFFSET = 4;
const uint32_t REGISTER_PWM_PWMnGENn_ACTCMPAD_OFFSET = 6;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_NOTHING = 0x0;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_INVERT = 0x1;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_LOW = 0x2;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_HIGH = 0x3;

const uint32_t REGISTER_PWM_0_BASE = REGISTER_PWM_BASE + 0x0;
volatile uint32_t * const REGISTER_PWM_0_PWMENABLE = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWMENABLE_OFFSET);
volatile uint32_t * const REGISTER_PWM_0_PWM0LOAD = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWM0LOAD_OFFSET);
volatile uint32_t * const REGISTER_PWM_0_PWM0CTL = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWM0CTL_OFFSET);
volatile uint32_t * const REGISTER_PWM_0_PWM0CMPA = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWM0CMPA_OFFSET);
volatile uint32_t * const REGISTER_PWM_0_PWM0CMPB = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWM0CMPB_OFFSET);
volatile uint32_t * const REGISTER_PWM_0_PWM0GENA = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWM0GENA_OFFSET);
volatile uint32_t * const REGISTER_PWM_0_PWM0GENB = (uint32_t *)(REGISTER_PWM_0_BASE + REGISTER_PWM_PWM0GENB_OFFSET);

// const uint32_t REGISTER_PWM_1_0BASE = REGISTER_PWM_BASE + 0x1000;

const uint16_t PWM_RELOAD_VALUE = 500;

typedef struct {
    uint8_t enabled;
    MOTOR_MODE mode;
    int16_t power;
    MOTOR_ZERO_POWER_BEHAVIOR zeroPowerBehavior;
} Motor;

Motor motors[MOTOR_CHANNEL_COUNT];

typedef struct {


} MotorGPIOPorts;

int8_t motor_init(void) {
    for (uint8_t i = 0; i < MOTOR_CHANNEL_COUNT; i++) {
        Motor *motor = &motors[i];
        motor->enabled = 0;
        motor->mode = 0;
        motor->power = 0;
        motor->zeroPowerBehavior = MOTOR_ZERO_POWER_BEHAVIOR_COAST;
    }

    *REGISTER_SYSCTL_RCC = (*REGISTER_SYSCTL_RCC & ~REGISTER_SYSCTL_RCC_PWMDIV_MASK) | REGISTER_SYSCTL_RCC_PWMDIV_DIV_16 | REGISTER_SYSCTL_RCC_USEPWMDIV;

    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_PWM_OFFSET, REGISTER_SYSCTL_PERIPHCTL_PWM_0_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_PWM_1_INSTANCEMASK);
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET, REGISTER_SYSCTL_PERIPHCTL_GPIO_F_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_GPIO_H_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_GPIO_K_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_GPIO_N_INSTANCEMASK);

    gpio_enableAltPinFunc(REGISTER_GPIO_H_BASE, MOTOR_0_PWM_PIN, REGISTER_GPIO_GPIOPCTL_H_PWM);

    *REGISTER_PWM_0_PWMENABLE &= ~REGISTER_PWM_PWMENABLE_ALL;
    //Maybe use count down because then the enable line doesn't have to be worried about, but it may never get to 100% duty cycle. TESTa
    *REGISTER_PWM_0_PWM0CTL = (*REGISTER_PWM_0_PWM0CTL & ~REGISTER_PWM_PWM0CTL_ENABLE) | REGISTER_PWM_PWM0CTL_COUNTER_DOWN;
    *REGISTER_PWM_0_PWM0GENA = 
                        (
                            *REGISTER_PWM_0_PWM0GENA
                            & ~(REGISTER_PWM_PWMnGENn_ACTn_MASK << REGISTER_PWM_PWMnGENn_ACTZERO_OFFSET)
                            & ~(REGISTER_PWM_PWMnGENn_ACTn_MASK << REGISTER_PWM_PWMnGENn_ACTCMPAU_OFFSET)
                        ) 
                        | REGISTER_PWM_PWMnGENn_ACTn_HIGH << REGISTER_PWM_PWMnGENn_ACTZERO_OFFSET
                        | REGISTER_PWM_PWMnGENn_ACTn_LOW << REGISTER_PWM_PWMnGENn_ACTCMPAD_OFFSET
    ;
                    
    //This comes from the fact that we want a 10 KHz pwm clock, the system clock is 80 MHz, and we need to be able to get down to 50 Hz for the servos. (80000000 / 16) / 10000 = 500
    *REGISTER_PWM_0_PWM0LOAD = PWM_RELOAD_VALUE & REGISTER_PWM_PWM0LOAD_MASK;

    // *REGISTER_PWM_0_PWM0CMPA = 250;

    *REGISTER_PWM_0_PWM0CTL |= REGISTER_PWM_PWM0CTL_ENABLE;

    /*
    > mrw 0x40028000 //PWMCTL sync
    0x0
    > mrw 0x40028008 //Enable (only some motors enabled?)
    0x3
    > mrw 0x4002800C //Invert (only servos inverted)
    0xcc
    > mrw 0x40028040 // PWM0CTL Enable, count up / down
    0x10003
    > mrw 0x40028050 //Reload value
    0x3e
    > mrw 0x40028060 //GENA
    0xb0
    > mrw 0x40028064
    0xb00
    */

    *REGISTER_GPIO_F_GPIODIR |= MOTOR_0_INB_PIN;
    *REGISTER_GPIO_K_GPIODIR |= MOTOR_0_INA_PIN;
    *REGISTER_GPIO_N_GPIODIR |= MOTOR_0_EN_PIN;

    *REGISTER_GPIO_F_GPIODR |= MOTOR_0_INB_PIN;
    *REGISTER_GPIO_K_GPIODR &= ~MOTOR_0_INA_PIN;
    *REGISTER_GPIO_N_GPIODR &= ~MOTOR_0_EN_PIN;

    *REGISTER_GPIO_N_GPIOODR |= MOTOR_0_EN_PIN;
    
    *REGISTER_GPIO_F_GPIODEN |= MOTOR_0_INB_PIN;
    *REGISTER_GPIO_K_GPIODEN |= MOTOR_0_INA_PIN;
    *REGISTER_GPIO_N_GPIODEN |= MOTOR_0_EN_PIN;

    return 0;
}

// void motor_start(void) {
//     *REGISTER_PWM_0_PWMENABLE |= REGISTER_PWM_PWMENABLE_PWM0EN;
// }

// void motor_stop(void) {
//     *REGISTER_PWM_0_PWMENABLE &= ~REGISTER_PWM_PWMENABLE_PWM0EN;
// }

int8_t motor_setEnabled(uint8_t channel, uint8_t enabled) {
    if(channel >= MOTOR_CHANNEL_COUNT) {
        return 1;
    }

    Motor * motor = &motors[channel];
    if(motor->enabled == enabled) {
        return 0;
    }

    motor->enabled = enabled;

    //FIX: Add the rest of the channels
    if(channel != 0) {
        return 0;
    } 

    if(enabled) {
        *REGISTER_GPIO_N_GPIODR |= MOTOR_0_EN_PIN;
    } else {
        *REGISTER_GPIO_N_GPIODR &= ~MOTOR_0_EN_PIN;
    }

    
    return 0;
}

int8_t motor_getEnabled(uint8_t channel, uint8_t * enabled) {
    if(channel >= MOTOR_CHANNEL_COUNT) {
        return 1;
    }

    *enabled = motors[channel].enabled;
    return 0;
}

int8_t motor_setPower(uint8_t channel, int16_t power) {
    if(channel >= MOTOR_CHANNEL_COUNT) {
        return 1;
    }

    Motor * motor = &motors[channel];
    if(motor->power == power) {
        return 0;
    }

    motor->power = power;

    //FIX: Add the rest of the channels
    if(channel != 0) {
        return 0;
    }

    if(power == 0) {
        *REGISTER_PWM_0_PWMENABLE &= ~REGISTER_PWM_PWMENABLE_PWM0EN;
    } else {

        if(power > 0) {
            *REGISTER_GPIO_F_GPIODR &= ~MOTOR_0_INB_PIN;
            *REGISTER_GPIO_K_GPIODR |= MOTOR_0_INA_PIN;
        } else if(power < 0) {
            *REGISTER_GPIO_F_GPIODR |= MOTOR_0_INB_PIN;
            *REGISTER_GPIO_K_GPIODR &= ~MOTOR_0_INA_PIN;

            //If we didn't do this when we try to invert INT16_MIN we would just get INT16_MIN
            if(power == INT16_MIN) {
                power++;
            }
            
            power = -power;
        }

        //Scale the power int16_t for the pwm compariator to be within the range of 0 to (PWM_RELOAD_VALUE - 1). The max value is (PWM_RELOAD_VALUE - 1) because if the 
        // comparator is set to the reload value then the PWM signal is always set to whatever the reload event sets it to. In this case the reload
        // event sets the pin high. We exploit this fact to allow for full power operation, but because the counter can only count down and reset even must
        // set the signal high the counter value has to be the opposite of the desired power. We use the PWMENABLE register to disable the PWM signal and set it low
        // when needed.
        *REGISTER_PWM_0_PWM0CMPA = (PWM_RELOAD_VALUE - 1) - power * ((uint32_t) (PWM_RELOAD_VALUE - 1)) / INT16_MAX;
        *REGISTER_PWM_0_PWMENABLE |= REGISTER_PWM_PWMENABLE_PWM0EN;
    }

    return 0;
}

int8_t motor_getPower(uint8_t channel, int16_t * power) {
    if(channel >= MOTOR_CHANNEL_COUNT) {
        return 1;
    }

    *power = motors[channel].power;
    return 0;
}
