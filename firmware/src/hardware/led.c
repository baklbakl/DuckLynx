#include "led.h"
#include "debugUART.h"
#include "sysctl.h"
#include "register.h"
#include "gpio.h"

const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_M_INSTANCEMASK = 0b1 << 11;

const uint32_t REGISTER_SYSCTL_PERIPHCTL_WGPTM_0_INSTANCEMASK = 0b1 << 0;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_WGPTM_5_INSTANCEMASK = 0b1 << 5;

const uint32_t REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_0 = 0x7;
const uint32_t REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_5 = 0x8;

const uint32_t REGISTER_GPIO_M_BASE = REGISTER_GPIO_BASE + 0x5F000;

typedef enum : uint8_t {
    REGISTER_GPTM_TIMER_A = 0b01,
    REGISTER_GPTM_TIMER_B = 0b10
} REGISTER_GPTM_TIMER;
const uint8_t REGISTER_GPTM_TIMER_AB_OFFSET = 0x04;

const uint32_t REGISTER_GPTM_GPTMTnMR_TnMR_PERIODIC = 0x02 << REGISTER_GPTM_GPTMTnMR_TnMR_OFFSET;
const uint32_t REGISTER_GPTM_GPTMTnMR_TnAMS = 0b1 << 3;

const uint32_t REGISTER_GPTM_GPTMTAMATCHR_OFFSET = 0x030;
const uint32_t REGISTER_GPTM_GPTMTBMATCHR_OFFSET = REGISTER_GPTM_GPTMTAMATCHR_OFFSET + REGISTER_GPTM_TIMER_AB_OFFSET;

const uint32_t REGISTER_GPTM_W0_BASE = REGISTER_GPTM_BASE + 0x00006000;
//RED
volatile uint32_t * const REGISTER_GPTM_W0_GPTMTAMATCHR = (uint32_t *)(REGISTER_GPTM_W0_BASE + REGISTER_GPTM_GPTMTAMATCHR_OFFSET);
//GREEN
volatile uint32_t * const REGISTER_GPTM_W0_GPTMTBMATCHR = (uint32_t *)(REGISTER_GPTM_W0_BASE + REGISTER_GPTM_GPTMTBMATCHR_OFFSET);

const uint32_t REGISTER_GPTM_W5_BASE = REGISTER_GPTM_BASE + 0x0001f000;
//BLUE
volatile uint32_t * const REGISTER_GPTM_W5_GPTMTBMATCHR = (uint32_t *)(REGISTER_GPTM_W5_BASE + REGISTER_GPTM_GPTMTBMATCHR_OFFSET);

void setupLedPWMTimer(const uint32_t baseAddress, const REGISTER_GPTM_TIMER timer) {
    // Split the timer into its two halfs
    volatile uint32_t * const GPTMCFG = (uint32_t *)(baseAddress + REGISTER_GPTM_GPTMCFG_OFFSET);
    *GPTMCFG = (*GPTMCFG & ~REGISTER_GPTM_GPTMCFG_GPTMCFG_MASK) | REGISTER_GPTM_GPTMCFG_GPTMCFG_SPLIT;

    for (
        uint8_t timerOffset = (timer & REGISTER_GPTM_TIMER_A ? 0 : REGISTER_GPTM_TIMER_AB_OFFSET);
        timerOffset <= ((timer & REGISTER_GPTM_TIMER_B) ? REGISTER_GPTM_TIMER_AB_OFFSET : 0);
        timerOffset += REGISTER_GPTM_TIMER_AB_OFFSET
    ) {
        // debugUART_printString("ran on timer. Baseaddress: ");
        // debugUART_printWordHex(baseAddress);
        // debugUART_printString(" offset: ");
        // debugUART_printWord(timerOffset);
        // debugUART_printChar('\n');

        volatile uint32_t * const GPTMTAMR = (uint32_t *)(baseAddress + REGISTER_GPTM_GPTMTAMR_OFFSET + timerOffset);
        //Enable PWM mode by enabling the alternate mode and putting the timer in periodic mode
        *GPTMTAMR = (*GPTMTAMR & ~REGISTER_GPTM_GPTMTnMR_TnMR_MASK) | REGISTER_GPTM_GPTMTnMR_TnMR_PERIODIC | REGISTER_GPTM_GPTMTnMR_TnAMS;
        
        //Set the reload value (frequency)
        *(volatile uint32_t * const)(baseAddress + REGISTER_GPTM_GPTMTAILR_OFFSET + timerOffset) = 0x100;
        //Set the duty cycle to 0
        *(volatile uint32_t * const)(baseAddress + REGISTER_GPTM_GPTMTAMATCHR_OFFSET + timerOffset) = 0x00;
    }

    //Enable the timer(s)
    uint32_t enabledTimers = 0;
    if(timer & REGISTER_GPTM_TIMER_A) {
        enabledTimers |= REGISTER_GPTM_GPTMCTL_TAEN | REGISTER_GPTM_GPTMCTL_TAPWML;
    }
    if(timer & REGISTER_GPTM_TIMER_B) {
        enabledTimers |= REGISTER_GPTM_GPTMCTL_TBEN | REGISTER_GPTM_GPTMCTL_TBPWML;
    }

    
    *(volatile uint32_t * const)(baseAddress + REGISTER_GPTM_GPTMCTL_OFFSET) |= enabledTimers;
}

const uint8_t led_PIN_RED = REGISTER_GPIO_PIN_6;
const uint8_t led_PIN_GREEN = REGISTER_GPIO_PIN_7;
const uint8_t led_PIN_BLUE = REGISTER_GPIO_PIN_3;
const uint8_t led_PINS = led_PIN_RED | led_PIN_GREEN | led_PIN_BLUE; //NOLINT(hicpp-signed-bitwise)

//Address bus masking allows us to write to the whole register, while not effecting the contents of the other bits for all three of the LED functions. See datasheet page 669.
// volatile uint32_t * const led_GPIO_DATA = (uint32_t *)(REGISTER_GPIO_M_GPIODATA + (led_PINS << 2));

volatile LED_MODE led_mode = LED_MODE_RESET;
LED_MODE led_oldMode;
led_Pattern * led_oldPattern;
volatile uint32_t led_lastStepTime;
volatile led_Pattern * led_currentPattern;
volatile uint8_t led_currentPatternStep;

volatile led_PatternStep userPatternSteps[LED_MAX_PATTERN_STEPS];
volatile led_Pattern led_userPattern = {
    userPatternSteps,
    0
};

const led_PatternStep noBatteryPatternSteps[] = {
    {
        3,
        0x00,
        0x20,
        0xFF,
    },
    {
        3,
        0x00,
        0x00,
        0x00,
    },
};
const led_Pattern led_noBatteryPattern = {
    noBatteryPatternSteps,
    sizeof(noBatteryPatternSteps) / sizeof(led_PatternStep)
};

const led_PatternStep timeoutPatternSteps[] = {
    {
        3,
        0x7F,
        0x00,
        0x00,
    },
    {
        3,
        0x00,
        0x00,
        0x00,
    },
};
const led_Pattern led_timeoutPattern = {
    timeoutPatternSteps,
    sizeof(timeoutPatternSteps) / sizeof(led_PatternStep)
};

const led_PatternStep connectedPatternSteps[] = {
    {
        0,
        0x00,
        0x20,
        0x00,
    },
};
const led_Pattern led_connectedPattern = {
    connectedPatternSteps,
    sizeof(connectedPatternSteps) / sizeof(led_PatternStep)
};

const led_PatternStep resetPatternSteps[] = {
    {
        0,
        0x20,
        0x00,
        0x00,
    },
};
const led_Pattern led_resetPattern = {
    resetPatternSteps,
    sizeof(resetPatternSteps) / sizeof(led_PatternStep)
};

const uint8_t led_noBatteryPatternStepCount = sizeof(led_noBatteryPattern) / sizeof(led_PatternStep);

int8_t led_init(void) {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET, REGISTER_SYSCTL_PERIPHCTL_GPIO_M_INSTANCEMASK);
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_WGPTM_OFFSET, REGISTER_SYSCTL_PERIPHCTL_WGPTM_0_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_WGPTM_5_INSTANCEMASK);

    // setupLedPWMTimer(REGISTER_GPTM_W0_BASE, REGISTER_GPTM_TIMER_A);
    setupLedPWMTimer(REGISTER_GPTM_W0_BASE, REGISTER_GPTM_TIMER_A | REGISTER_GPTM_TIMER_B);
    setupLedPWMTimer(REGISTER_GPTM_W5_BASE, REGISTER_GPTM_TIMER_B);

    gpio_enableAltPinFunc(REGISTER_GPIO_M_BASE, led_PIN_RED, REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_0);
    gpio_enableAltPinFunc(REGISTER_GPIO_M_BASE,  led_PIN_GREEN, REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_0);
    gpio_enableAltPinFunc(REGISTER_GPIO_M_BASE, led_PIN_BLUE, REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_5);
    
    for(uint8_t i = 0; i < LED_MAX_PATTERN_STEPS; i++) {
        led_PatternStep * step = &led_userPattern.steps[i];
        step->tenths = 0;
        step->red = 0;
        step->green = 0;
        step->blue = 0;
    }
    led_mode = LED_MODE_RESET;
    led_setupPattern(led_mode, &led_resetPattern);
    
    return 0;
}

void led_setColor(const uint8_t red, const uint8_t green, const uint8_t blue) {
    debugUART_printString("Setting color to: {r: ");
    debugUART_printU8Hex(red);
    debugUART_printString(", g: ");
    debugUART_printU8Hex(green);
    debugUART_printString(", b: ");
    debugUART_printU8Hex(blue);
    debugUART_printString("}\n");

    //These registers are 32 bits wide, so we don't need to worry about preserving the other bits
    *REGISTER_GPTM_W0_GPTMTAMATCHR = red;
    *REGISTER_GPTM_W0_GPTMTBMATCHR = green;
    *REGISTER_GPTM_W5_GPTMTBMATCHR = blue;
}

void led_getColor(uint8_t * red, uint8_t * green, uint8_t * blue) {
    *red = *REGISTER_GPTM_W0_GPTMTAMATCHR;
    *green = *REGISTER_GPTM_W0_GPTMTBMATCHR;
    *blue = *REGISTER_GPTM_W5_GPTMTBMATCHR;
}

void led_on(void){
    led_setColor(255, 255, 255);
}

void led_off(void) {
    led_setColor(0, 0, 0);
}

void led_dumpPattern(void) {
    for(uint8_t i = 0; i < LED_MAX_PATTERN_STEPS; i++) {
        led_PatternStep step = led_userPattern.steps[i];

        debugUART_printString("Step ");
        debugUART_printWord(i);
        debugUART_printString(": {r: ");
        debugUART_printU8Hex(step.red);
        debugUART_printString(", g: ");
        debugUART_printU8Hex(step.green);
        debugUART_printString(", b: ");
        debugUART_printU8Hex(step.blue);
        debugUART_printString("} for ");
        debugUART_printWord(step.tenths / 10);
        debugUART_printChar('.');
        debugUART_printWord(step.tenths % 10);
        debugUART_printString("s\n");

    }
}

void led_setupPatternWithTime(const LED_MODE mode, const led_Pattern *pattern, const uint32_t time) {
    if(led_mode == LED_MODE_NO_BATTERY) {
        led_oldMode = mode;
        led_oldPattern = pattern;

    } else {
        led_mode = mode;
        led_currentPattern = pattern;
        led_currentPatternStep = 0;
        led_lastStepTime = time;
        
        led_PatternStep step = led_currentPattern->steps[led_currentPatternStep];
        led_setColor(step.red, step.green, step.blue);
    }
}

void led_setupPattern(const LED_MODE mode, const led_Pattern *pattern) {
    led_setupPatternWithTime(mode, pattern, *timer_value);
}



// void led_doNextPatternStep(void) {
//     led_currentPatternStep++;
//     if(led_currentPatternStep >= led_currentPattern->totalSteps) {
//         led_currentPatternStep = 0;
//     }
    
//     led_PatternStep step = led_currentPattern->steps[led_currentPatternStep];
//     led_setColor(step.red, step.green, step.blue);
// }
