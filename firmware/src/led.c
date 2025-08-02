// #include "led.h"
#include "debugUART.h"
#include "sysctl.h"
#include "register.h"
#include "gpio.h"

const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_PORTM_INSTANCEMASK = 0b1 << 11;

const uint8_t REGISTER_SYSCTL_PERIPHCTL_WGPTM_OFFSET = 0x5C;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_WGPTM_0_INSTANCEMASK = 0b1 << 0;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_WGPTM_5_INSTANCEMASK = 0b1 << 5;

const uint32_t REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_0 = 0x7;
const uint32_t REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_5 = 0x8;

const uint32_t REGISTER_GPIO_M_BASE = REGISTER_GPIO_BASE + 0x5F000;

const uint32_t REGISTER_GPTM_BASE = 0x40030000;

typedef enum : uint8_t {
    REGISTER_GPTM_TIMER_A = 0b01,
    REGISTER_GPTM_TIMER_B = 0b10
} REGISTER_GPTM_TIMER;
const uint8_t REGISTER_GPTM_TIMER_AB_OFFSET = 0x04;

const uint32_t REGISTER_GPTM_GPTMCFG_OFFSET = 0x0;
//There is a field in GPTMCFG called GPTMCFG
const uint32_t REGISTER_GPTM_GPTMCFG_GPTMCFG_MASK = 0b111;
const uint32_t REGISTER_GPTM_GPTMCFG_GPTMCFG_SPLIT = 0x4;

//Timer n mode register
const uint32_t REGISTER_GPTM_GPTMTAMR_OFFSET = 0x04;
const uint32_t REGISTER_GPTM_GPTMTnMR_TnMR_MASK = 0b11;
const uint32_t REGISTER_GPTM_GPTMTnMR_TnMR_PERIODIC = 0x02;
const uint32_t REGISTER_GPTM_GPTMTnMR_TnAMS = 0b1 << 3;

const uint32_t REGISTER_GPTM_GPTMCTL_OFFSET = 0x0C;
const uint32_t REGISTER_GPTM_GPTMCTL_TAEN = 0b1;
const uint32_t REGISTER_GPTM_GPTMCTL_TAPWML = 0b1 << 6;
const uint32_t REGISTER_GPTM_GPTMCTL_TBEN = 0b1 << 8;
const uint32_t REGISTER_GPTM_GPTMCTL_TBPWML = 0b1 << 14;

const uint32_t REGISTER_GPTM_GPTMTAILR_OFFSET = 0x028;
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

    for(
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


int8_t led_init(void) {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET, REGISTER_SYSCTL_PERIPHCTL_GPIO_PORTM_INSTANCEMASK);
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_WGPTM_OFFSET, REGISTER_SYSCTL_PERIPHCTL_WGPTM_0_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_WGPTM_5_INSTANCEMASK);

    // setupLedPWMTimer(REGISTER_GPTM_W0_BASE, REGISTER_GPTM_TIMER_A);
    setupLedPWMTimer(REGISTER_GPTM_W0_BASE, REGISTER_GPTM_TIMER_A | REGISTER_GPTM_TIMER_B);
    setupLedPWMTimer(REGISTER_GPTM_W5_BASE, REGISTER_GPTM_TIMER_B);

    gpio_enableAltPinFunc(REGISTER_GPIO_M_BASE, led_PIN_RED, REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_0);
    gpio_enableAltPinFunc(REGISTER_GPIO_M_BASE,  led_PIN_GREEN, REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_0);
    gpio_enableAltPinFunc(REGISTER_GPIO_M_BASE, led_PIN_BLUE, REGISTER_GPIO_GPIOPCTL_GPIOM_WGPTM_5);

    //Set the LED pins to output
    // *REGISTER_GPIO_M_GPIODIR |= led_PINS;
    //Connect the pad to the GPIO
    // *REGISTER_GPIO_M_GPIODEN |= led_PINS;
    
    // led_off();
    
    return 0;
}

void led_setColor(uint8_t red, uint8_t green, uint8_t blue) {
    *REGISTER_GPTM_W0_GPTMTAMATCHR = red;
    *REGISTER_GPTM_W0_GPTMTBMATCHR = green;
    *REGISTER_GPTM_W5_GPTMTBMATCHR = blue;
    
    // *(led_GPIO_DATA) = (((red & 1) *  led_PIN_RED)) | ((green & 1) * led_PIN_GREEN) | ((blue & 1) * led_PIN_BLUE);


}

void led_on(void){
    led_setColor(255, 255, 255);
}

void led_off(void) {
    led_setColor(0, 0, 0);
}
