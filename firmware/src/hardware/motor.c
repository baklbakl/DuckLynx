#include "hardware/motor.h"
#include "debugUART.h"
#include "hardware/gpio.h"
#include "hardware/register.h"
#include "sysctl.h"

//
// Sysctl
//

const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_SHFIT = 17;
const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_MASK = 0b111 << REGISTER_SYSCTL_RCC_PWMDIV_SHFIT;
const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_DIV_16 = 0x3 << REGISTER_SYSCTL_RCC_PWMDIV_SHFIT;
const uint32_t REGISTER_SYSCTL_RCC_PWMDIV_DIV_64 = 0x5 << REGISTER_SYSCTL_RCC_PWMDIV_SHFIT;
const uint32_t REGISTER_SYSCTL_RCC_USEPWMDIV = 0b1 << 20;

const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_F_INSTANCEMASK = 0b1 << 5;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_K_INSTANCEMASK = 0b1 << 9;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_N_INSTANCEMASK = 0b1 << 12;

const uint8_t REGISTER_SYSCTL_PERIPHCTL_PWM_OFFSET = 0x40;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_PWM_0_INSTANCEMASK = 0b1 << 0;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_PWM_1_INSTANCEMASK = 0b1 << 1;

//
// GPIO
//

const uint32_t REGISTER_GPIO_GPIOODR_OFFSET = 0x50C;
const uint32_t REGISTER_GPIO_GPIOPCTL_H_PWM_M0 = 0x4;
const uint32_t REGISTER_GPIO_GPIOPCTL_G_PWM_M1 = 0x5;

const uint32_t REGISTER_GPIO_F_BASE = REGISTER_GPIO_BASE + 0x59000;
const uint32_t REGISTER_GPIO_K_BASE = REGISTER_GPIO_BASE + 0x5D000;
const uint32_t REGISTER_GPIO_N_BASE = REGISTER_GPIO_BASE + 0x60000;

//
// PWM
//

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

const uint32_t REGISTER_PWM_PWMnLOAD_OFFSET = 0x10;
const uint32_t REGISTER_PWM_PWM0LOAD_MASK = 0xFFFF;
const uint32_t REGISTER_PWM_PWMnCTL_OFFSET = 0x0;
const uint32_t REGISTER_PWM_PWMnCTL_ENABLE = 0b1 << 0;
const uint32_t REGISTER_PWM_PWMnCTL_COUNTER_OFFSET = 1;
const uint32_t REGISTER_PWM_PWMnCTL_COUNTER_UP_DOWN = 1 << REGISTER_PWM_PWMnCTL_COUNTER_OFFSET;
const uint32_t REGISTER_PWM_PWMnCTL_COUNTER_DOWN = 0 << REGISTER_PWM_PWMnCTL_COUNTER_OFFSET;

const uint32_t REGISTER_PWM_PWMnCMPA_OFFSET = 0x18;
const uint32_t REGISTER_PWM_PWMnCMPB_OFFSET = 0x1C;
const uint32_t REGISTER_PWM_PWMnCMPn_MASK = 0xFFFF;
const uint32_t REGISTER_PWM_PWMnGENA_OFFSET = 0x20;
const uint32_t REGISTER_PWM_PWMnGENB_OFFSET = 0x24;

const uint32_t REGISTER_PWM_PWMnGENn_ACTn_MASK = 0b11;
const uint32_t REGISTER_PWM_PWMnGENn_ACTZERO_OFFSET = 0;
const uint32_t REGISTER_PWM_PWMnGENn_ACTCMPAU_OFFSET = 4;
const uint32_t REGISTER_PWM_PWMnGENn_ACTCMPAD_OFFSET = 6;
const uint32_t REGISTER_PWM_PWMnGENn_ACTCMPBU_OFFSET = 8;
const uint32_t REGISTER_PWM_PWMnGENn_ACTCMPBD_OFFSET = 10;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_NOTHING = 0x0;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_INVERT = 0x1;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_LOW = 0x2;
const uint32_t REGISTER_PWM_PWMnGENn_ACTn_HIGH = 0x3;

//Starting offset
const uint32_t REGISTER_PWM_GEN0_OFFSET = 0x40;
//Offset from one generator to the next
const uint32_t REGISTER_PWM_GEN_TO_GEN_OFFSET = 0x40;
const uint32_t REGISTER_PWM_0_BASE = REGISTER_PWM_BASE + 0x0;
const uint32_t REGISTER_PWM_1_BASE = REGISTER_PWM_BASE + 0x1000;

//This comes from the fact that we want a 10 KHz pwm clock, the system clock is 80 MHz,
// and we need to be able to get down to 50 Hz for the servos. (80000000 / 16) / 10000 = 500
const uint16_t PWM_RELOAD_VALUE = 500;

typedef struct {
    uint32_t gpioBase;
    uint8_t pinBitmask;
} Pin;

typedef struct {
    Pin INA;
    Pin INB;
    Pin EN;
    struct {
        uint32_t gpioBase;
        uint8_t pinBitmask;
        uint32_t pwmBase;
        //Each PWM module has four possible generators in it
        // uint32_t pwmNOffset;
        uint8_t pwmBitmask;
    } PWM;
} MotorPins;

const MotorPins motorPinsConsts[] = {
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
            REGISTER_GPIO_PIN_0,
            REGISTER_PWM_0_BASE,
            REGISTER_PWM_PWMENABLE_PWM0EN
        }
    },
    {
        {
            REGISTER_GPIO_K_BASE,
            REGISTER_GPIO_PIN_7
        },
        {
            REGISTER_GPIO_F_BASE,
            REGISTER_GPIO_PIN_3,
        },
        {
            REGISTER_GPIO_N_BASE,
            REGISTER_GPIO_PIN_0
        },
        {
            REGISTER_GPIO_H_BASE,
            REGISTER_GPIO_PIN_1,
            REGISTER_PWM_0_BASE,
            REGISTER_PWM_PWMENABLE_PWM1EN
        }
    },
    {
        {
            REGISTER_GPIO_G_BASE,
            REGISTER_GPIO_PIN_6,
        },
        {
            REGISTER_GPIO_K_BASE,
            REGISTER_GPIO_PIN_2
        },
        {
            REGISTER_GPIO_M_BASE,
            REGISTER_GPIO_PIN_4
        },
        {
            REGISTER_GPIO_G_BASE,
            REGISTER_GPIO_PIN_4,
            REGISTER_PWM_1_BASE,
            REGISTER_PWM_PWMENABLE_PWM2EN
        }
    },
    {
        {
            REGISTER_GPIO_G_BASE,
            REGISTER_GPIO_PIN_7,
        },
        {
            REGISTER_GPIO_K_BASE,
            REGISTER_GPIO_PIN_3
        },
        {
            REGISTER_GPIO_M_BASE,
            REGISTER_GPIO_PIN_5
        },
        {
            REGISTER_GPIO_G_BASE,
            REGISTER_GPIO_PIN_5,
            REGISTER_PWM_1_BASE,
            REGISTER_PWM_PWMENABLE_PWM3EN
        }
    },
};

typedef struct {
    uint8_t enabled;
    MOTOR_MODE mode;
    int16_t power;
    MOTOR_ZERO_POWER_BEHAVIOR zeroPowerBehavior;
    volatile uint32_t * gpioDR_INA;
    volatile uint32_t * gpioDR_INB;
    volatile uint32_t * gpioDR_EN;
    volatile uint32_t * pwmCMPn;
    volatile uint32_t * pwmEnable;
} Motor;

Motor motors[MOTOR_CHANNEL_COUNT];

int8_t motor_init(void) {
    *REGISTER_SYSCTL_RCC = (*REGISTER_SYSCTL_RCC & ~REGISTER_SYSCTL_RCC_PWMDIV_MASK) | REGISTER_SYSCTL_RCC_PWMDIV_DIV_16 | REGISTER_SYSCTL_RCC_USEPWMDIV;

    sysctl_enablePeripheral(
        REGISTER_SYSCTL_PERIPHCTL_PWM_OFFSET, 
        REGISTER_SYSCTL_PERIPHCTL_PWM_0_INSTANCEMASK
                     | REGISTER_SYSCTL_PERIPHCTL_PWM_1_INSTANCEMASK
    );

    sysctl_enablePeripheral(
        REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET,
        REGISTER_SYSCTL_PERIPHCTL_GPIO_F_INSTANCEMASK
                     | REGISTER_SYSCTL_PERIPHCTL_GPIO_G_INSTANCEMASK
                     | REGISTER_SYSCTL_PERIPHCTL_GPIO_H_INSTANCEMASK
                     | REGISTER_SYSCTL_PERIPHCTL_GPIO_K_INSTANCEMASK
                     | REGISTER_SYSCTL_PERIPHCTL_GPIO_M_INSTANCEMASK
                     | REGISTER_SYSCTL_PERIPHCTL_GPIO_N_INSTANCEMASK
    );

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
    > mrw 0x40028064 //GENB
    0xb00
    */

    for (uint8_t i = 0; i < MOTOR_CHANNEL_COUNT; i++) {
    //This may set some registers more than once per PWM generator or module, but it was easier to write without a bunch of extra checks
    // for (uint8_t i = 0; i < 3; i++) {
        Motor *motor = &motors[i];
        motor->enabled = 0;
        motor->mode = 0;
        motor->power = INT16_MIN;
        motor->zeroPowerBehavior = MOTOR_ZERO_POWER_BEHAVIOR_COAST;
    
        MotorPins pins = motorPinsConsts[i];

        if(pins.PWM.pwmBase == REGISTER_PWM_0_BASE && pins.PWM.gpioBase == REGISTER_GPIO_H_BASE) {
            gpio_enableAltPinFunc(pins.PWM.gpioBase, pins.PWM.pinBitmask, REGISTER_GPIO_GPIOPCTL_H_PWM_M0);    

        } else if(pins.PWM.pwmBase == REGISTER_PWM_1_BASE && pins.PWM.gpioBase == REGISTER_GPIO_G_BASE) {
            gpio_enableAltPinFunc(pins.PWM.gpioBase, pins.PWM.pinBitmask, REGISTER_GPIO_GPIOPCTL_G_PWM_M1);    

        } else {
            //Something went very wrong ...
            return 1;
        }

        //Disable PWM output
        motor->pwmEnable = (uint32_t *)(pins.PWM.pwmBase + REGISTER_PWM_PWMENABLE_OFFSET);
        *(motor->pwmEnable) &= ~pins.PWM.pwmBitmask;

        uint32_t pwmNBase = pins.PWM.pwmBase + REGISTER_PWM_GEN0_OFFSET;

        //Each pair of PWM outputs from a module conresponds to a generator. To derive the offset into the pwm module's address space,
        // add the generator to generator offset until the pin's generator is found.
        for(uint8_t genMask = 0b11; genMask != 0; genMask = genMask << 2) {
            if(genMask & pins.PWM.pwmBitmask) {
                break;
            }
            pwmNBase += REGISTER_PWM_GEN_TO_GEN_OFFSET;
        }

        // uint32_t pwmNBase = pins.PWM.pwmBase + pins.PWM.pwmNOffset;

        volatile uint32_t * const PWMnCTL = (uint32_t *)(pwmNBase + REGISTER_PWM_PWMnCTL_OFFSET);
        *PWMnCTL = (*PWMnCTL & ~REGISTER_PWM_PWMnCTL_ENABLE) | REGISTER_PWM_PWMnCTL_COUNTER_DOWN;

        volatile uint32_t * PWMnGENn = 0; 
        uint32_t ACTCMPnD_offset = 0;

        //Even pins use generator A and odd pins us generator B
        if(pins.PWM.pwmBitmask & (REGISTER_PWM_PWMENABLE_PWM0EN | REGISTER_PWM_PWMENABLE_PWM2EN | REGISTER_PWM_PWMENABLE_PWM4EN | REGISTER_PWM_PWMENABLE_PWM6EN)) {
            PWMnGENn = (uint32_t *)(pwmNBase + REGISTER_PWM_PWMnGENA_OFFSET);
            motor->pwmCMPn = (uint32_t *)(pwmNBase + REGISTER_PWM_PWMnCMPA_OFFSET);
            ACTCMPnD_offset = REGISTER_PWM_PWMnGENn_ACTCMPAD_OFFSET;

        } else if(pins.PWM.pwmBitmask & (REGISTER_PWM_PWMENABLE_PWM1EN | REGISTER_PWM_PWMENABLE_PWM3EN | REGISTER_PWM_PWMENABLE_PWM5EN | REGISTER_PWM_PWMENABLE_PWM7EN)) {
            PWMnGENn = (uint32_t *)(pwmNBase + REGISTER_PWM_PWMnGENB_OFFSET); 
            motor->pwmCMPn = (uint32_t *)(pwmNBase + REGISTER_PWM_PWMnCMPB_OFFSET);
            ACTCMPnD_offset = REGISTER_PWM_PWMnGENn_ACTCMPBD_OFFSET;

        } else {
            //Something went very wrong ...
            return 1;
        }

        *PWMnGENn = 
            (
                *PWMnGENn
                & ~(REGISTER_PWM_PWMnGENn_ACTn_MASK << REGISTER_PWM_PWMnGENn_ACTZERO_OFFSET)
                & ~(REGISTER_PWM_PWMnGENn_ACTn_MASK << ACTCMPnD_offset)
            ) 
            | REGISTER_PWM_PWMnGENn_ACTn_HIGH << REGISTER_PWM_PWMnGENn_ACTZERO_OFFSET
            | REGISTER_PWM_PWMnGENn_ACTn_LOW << ACTCMPnD_offset
        ;

        *((volatile uint32_t * const)(pwmNBase + REGISTER_PWM_PWMnLOAD_OFFSET)) = PWM_RELOAD_VALUE & REGISTER_PWM_PWM0LOAD_MASK;
        *PWMnCTL |= REGISTER_PWM_PWMnCTL_ENABLE;

        //Set all the GPIOs to outputs
        *((volatile uint32_t * const)(pins.INA.gpioBase + REGISTER_GPIO_GPIODIR_OFFSET)) |= pins.INA.pinBitmask;
        *((volatile uint32_t * const)(pins.INB.gpioBase + REGISTER_GPIO_GPIODIR_OFFSET)) |= pins.INB.pinBitmask;
        *((volatile uint32_t * const)(pins.EN.gpioBase + REGISTER_GPIO_GPIODIR_OFFSET)) |= pins.EN.pinBitmask;

        //Set all of the pins low and cache the DR for future use. It is okay that the driver is set to brake to ground because the driver is disabled and the PWM line is low
        motor->gpioDR_INA = (uint32_t *)(pins.INA.gpioBase + REGISTER_GPIO_GPIODR_OFFSET + (pins.INA.pinBitmask << 2));
        *motor->gpioDR_INA = 0;
        
        motor->gpioDR_INB = (uint32_t *)(pins.INB.gpioBase + REGISTER_GPIO_GPIODR_OFFSET + (pins.INB.pinBitmask << 2));
        *motor->gpioDR_INB = 0;
        
        motor->gpioDR_EN = (uint32_t *)(pins.EN.gpioBase + REGISTER_GPIO_GPIODR_OFFSET + (pins.EN.pinBitmask << 2));
        *motor->gpioDR_EN = 0;

        //Open drain on enable pin
        *((volatile uint32_t * const)(pins.EN.gpioBase + REGISTER_GPIO_GPIOODR_OFFSET)) |= pins.EN.pinBitmask;

        //Enable the outputs
        *((volatile uint32_t * const)(pins.INA.gpioBase + REGISTER_GPIO_GPIODEN_OFFSET)) |= pins.INA.pinBitmask;
        *((volatile uint32_t * const)(pins.INB.gpioBase + REGISTER_GPIO_GPIODEN_OFFSET)) |= pins.INB.pinBitmask;
        *((volatile uint32_t * const)(pins.EN.gpioBase + REGISTER_GPIO_GPIODEN_OFFSET)) |= pins.EN.pinBitmask;
    }

    return 0;
}


RHSP_NACK motor_setEnabled(uint8_t channel, uint8_t enabled) {
    if(channel >= MOTOR_CHANNEL_COUNT) {
        return RHSP_NACK_PARAM_0_WRONG;
    }

    Motor * motor = &motors[channel];
    if(motor->power == INT16_MIN && enabled) {
        return RHSP_NACK_MOTOR_NOT_CONFIGURED_BEFORE_ENABLE;
    }

    if(motor->enabled == enabled) {
        return RHSP_NACK_NO_NACK;
    }

    motor->enabled = enabled;

    if(enabled) {
        *(motor->gpioDR_EN) |= motorPinsConsts[channel].EN.pinBitmask;
    } else {
        *(motor->gpioDR_EN) &= ~motorPinsConsts[channel].EN.pinBitmask;
    }
    
    return RHSP_NACK_NO_NACK;
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

    //INT16_MIN is never actually send to the motors. INT16_MIN + 1  is sent instead. The value is used to indicate that a motor hasn't had its power set yet. 
    // Also, if we didn't do this when we try to invert INT16_MIN we would just get INT16_MIN.   
    if(power == INT16_MIN) {
        power++;
    }

    Motor * motor = &motors[channel];
    if(motor->power == power) {
        return 0;
    }

    motor->power = power;

    if(power == 0) {
        //ADD: Zero power behavior brake
        *(motor->pwmEnable) &= ~motorPinsConsts[channel].PWM.pwmBitmask;
    } else {
        if(power > 0) {
            *(motor->gpioDR_INA) |= motorPinsConsts[channel].INA.pinBitmask;
            *(motor->gpioDR_INB) &= ~motorPinsConsts[channel].INB.pinBitmask;

        } else if(power < 0) {
            *(motor->gpioDR_INA) &= ~motorPinsConsts[channel].INA.pinBitmask;
            *(motor->gpioDR_INB) |= motorPinsConsts[channel].INB.pinBitmask;
      
            power = -power;
        }

        // debugUART_printString("Setting power to: ");
        // debugUART_printWord(power);
        // debugUART_printString(" at address 0x");
        // debugUART_printWordHex((uint32_t) motor->pwmCMPn);
        // debugUART_printChar('\n');

        //Scale the power int16_t for the pwm compariator to be within the range of 0 to (PWM_RELOAD_VALUE - 1). The max value is (PWM_RELOAD_VALUE - 1) because if the 
        // comparator is set to the reload value then the PWM signal is always set to whatever the reload event sets it to. In this case the reload
        // event sets the pin high. We exploit this fact to allow for full power operation, but because the counter can only count down and reset even must
        // set the signal high the counter value has to be the opposite of the desired power. We use the PWMENABLE register to disable the PWM signal and set it low
        // when needed.
        *(motor->pwmCMPn) = (PWM_RELOAD_VALUE - 1) - power * ((uint32_t) (PWM_RELOAD_VALUE - 1)) / INT16_MAX;
        *(motor->pwmEnable) |= motorPinsConsts[channel].PWM.pwmBitmask;
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
